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
#include <getopt.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include "jack_stuff.h"


/* JACK data */
//unsigned int nports;

const char **port_names;
//jack_port_t **ports;

jack_client_t *client;

jack_port_t *inputPort;
jack_port_t *outputPort;

jack_nframes_t nframes;
jack_ringbuffer_t *rb;

pthread_mutex_t disk_thread_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  data_ready = PTHREAD_COND_INITIALIZER;

long overruns = 0;

int
process (jack_nframes_t nframes, void *arg)
{
  int chn;
  size_t i;
  jack_thread_info_t *info = (jack_thread_info_t *) arg;

  /* Tell the disk thread there is work to do.  If it is already
   * running, the lock will not be available.  We can't wait
   * here in the process() thread, but we don't need to signal
   * in that case, because the disk thread will read all the
   * data queued before waiting again. */
  if (pthread_mutex_trylock (&disk_thread_lock) == 0) {
    pthread_cond_signal (&data_ready);
    pthread_mutex_unlock (&disk_thread_lock);
  }  

  return 0;
}

void
jack_shutdown (void *arg)
{
  fprintf (stderr, "JACK shutdown\n");

  abort();
}


int
get_midi_ports()
{
  int i;

  if ((port_names = jack_get_ports (client, NULL, "midi", JackPortIsInput)) == NULL) {
    fprintf (stderr, "Cannot find any ports to send to\n");
    exit(1);
  }
 
  for (i = 0; port_names[i] != NULL; i++) {
    fprintf (stderr, "refactor: found input port  %s\n", port_names[i]);
    if (i > 10) {
      break;
    }
  }
  
  return 0;
}

int
setup_ports()
{
  get_midi_ports();

  inputPort = jack_port_register (client, "input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
  outputPort = jack_port_register (client, "output", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

  return 0;
}

int
init () 
{
  client = jack_client_open ("reFactor", JackNullOption, NULL);
  
  if (client == NULL) {
    fprintf (stderr, "jack server not running?\n");
    exit (1);
  }

  jack_set_process_callback (client, process, NULL);
  jack_on_shutdown (client, jack_shutdown, NULL);

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
  }

  setup_ports ();

  return 0;
}

int
get_current_patch()
{
}
