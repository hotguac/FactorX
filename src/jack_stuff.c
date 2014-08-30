/*
  Copyright (C) 2001 Paul Davis
  Copyright (C) 2003 Jack O'Quin
    
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  * 2002/08/23 - modify for libsndfile 1.0.0 <andy@alsaplayer.org>
  * 2003/05/26 - use ringbuffers - joq
    
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sndfile.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <getopt.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include "jack_stuff.h"

#define JACK_CLIENT_NAME "reFactor"

/* JACK data */
jack_client_t *client;

jack_port_t *inputPort;
jack_port_t *outputPort;

jack_nframes_t nframes;
jack_ringbuffer_t *ring_buffer;

const char **port_names;

pthread_mutex_t msg_thread_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t data_ready = PTHREAD_COND_INITIALIZER;

long overruns = 0;

int firstTime = 1;

unsigned long long
time_us ()
{
    struct timeval time;
    gettimeofday (&time, NULL);
    return time.tv_sec * 1000 * 1000 + time.tv_usec;
}

int
process (jack_nframes_t nframes, void *arg)
{
    int chn;
    size_t i;
    jack_thread_info_t *info = (jack_thread_info_t *) arg;

    void *buffer;
    jack_nframes_t num_messages;
    jack_nframes_t msg_num;

    buffer = jack_port_get_buffer (inputPort, nframes);

    num_messages = jack_midi_get_event_count (buffer);

    for (msg_num = 0; msg_num < num_messages; ++msg_num)
      {
	  jack_midi_event_t event;
	  int result;

	  result = jack_midi_event_get (&event, buffer, msg_num);

	  if (result == 0 && jack_ringbuffer_write_space (ring_buffer) >= sizeof (sysex_msg))
	    {
		sysex_msg m;
		m.size = event.size;

		if ((event.buffer[0] == 0xF0) || (waiting_for_more))
		  {
		      memcpy (m.buffer, event.buffer, MAX (sizeof (m.buffer), event.size));
		      jack_ringbuffer_write (ring_buffer, (void *) &m, sizeof (sysex_msg));

		      waiting_for_more = 1;
		      for (i = 0; i < event.size; i++) {
			  if (m.buffer[i] == 0xF7) {
			      waiting_for_more = 0;
			  }
		      }

		  }
	    }
      }

    /* Tell the disk thread there is work to do.  If it is already
     * running, the lock will not be available.  We can't wait
     * here in the process() thread, but we don't need to signal
     * in that case, because the disk thread will read all the
     * data queued before waiting again. */
    if (pthread_mutex_trylock (&msg_thread_lock) == 0)
      {
	  pthread_cond_signal (&data_ready);
	  pthread_mutex_unlock (&msg_thread_lock);
      }

    return 0;
}

void
jack_end ()
{
    jack_deactivate (client);
    jack_client_close (client);	// close the client
    jack_ringbuffer_free (ring_buffer);

    client = NULL;
}

void
jack_shutdown ()
{
    jack_ringbuffer_free(ring_buffer);

    fprintf (stderr, "JACK shutdown\n");

    outputPort = NULL;
    inputPort = NULL;;

    if (client != NULL)
      {
	  jack_end ();
      }
}

int
get_midi_ports ()
{
    int i;

    if ((port_names = jack_get_ports (client, NULL, "midi", JackPortIsInput)) == NULL)
      {
	  fprintf (stderr, "Cannot find any ports to send to\n");
	  exit (1);
      }

    for (i = 0; port_names[i] != NULL; i++)
      {
	  fprintf (stderr, "refactor: found input port  %s\n", port_names[i]);
	  if (i > 10)
	    {
		break;
	    }
      }

    return 0;
}

int
setup_ports ()
{
    fprintf (stderr, "\treFactor: in setup_ports()\n");
 
    get_midi_ports ();

    inputPort = jack_port_register (client, "input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    if (inputPort == NULL)
      {
	  fprintf (stderr, "%s failed to register input port", JACK_CLIENT_NAME);
      }

    outputPort = jack_port_register (client, "output", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    if (outputPort == NULL)
      {
	  fprintf (stderr, "%s failed to register output port", JACK_CLIENT_NAME);
      }

    return 0;
}

int
init ()
{
    fprintf (stderr, "\treFactor: in init()\n");
 
    client = jack_client_open (JACK_CLIENT_NAME, JackNullOption, NULL);

    if (client == NULL)
      {
	  fprintf (stderr, "jack server not running?\n");
	  exit (1);
      }

    ring_buffer = jack_ringbuffer_create (DEFAULT_RB_SIZE * sizeof (sysex_msg));
    
    if (ring_buffer) {
	fprintf(stderr,"reFactor: created ring_buffer\n");
	jack_ringbuffer_mlock(ring_buffer);
    } else {
	fprintf(stderr, "reFactor: Error allocating ringbuffer\n");
	exit(1);
    }

    if (jack_set_process_callback (client, process, NULL)) {
	fprintf(stderr, "reFactor: error setting callback\n");
    }

    jack_on_shutdown (client, jack_shutdown, NULL);

    if (jack_activate (client))
      {
	  fprintf (stderr, "cannot activate client");
      }

    setup_ports ();

    return 0;
}

int
get_current_patch (char* buffer, int bsize)
{
    int level = 0;

    printf ("in jack_stuff.c get_current_patch\n");
    pthread_mutex_lock (&msg_thread_lock);

    const int mqlen = jack_ringbuffer_read_space (ring_buffer) / sizeof (sysex_msg);
    fprintf (stderr, "mqlen = %d\n", mqlen);

    int i;
    for (i = 0; i < mqlen; ++i)
      {
	  size_t j;
	  sysex_msg m;
	  
	  fprintf (stderr, "\treFactor: in get_current_patch for loop()\n");

	  jack_ringbuffer_read (ring_buffer, (char *) &m, sizeof (sysex_msg));

	  for (j = 0; j < m.size && j < sizeof (m.buffer); ++j)
	    {
		if (level < bsize)
		    {
			buffer[level++] = m.buffer[j];
		    }
		printf (" %02x", m.buffer[j]);
	    }

	  printf ("\nProcessed %d of %d\n", i+1, mqlen);
      }

    fflush (stdout);

    pthread_mutex_unlock (&msg_thread_lock);

    printf("returning from jack_stuff.c get_current_patch()\n");

    return level;
}

/// copied from mididuino.googlecode.com/midi-jack.h
// end of copied code
