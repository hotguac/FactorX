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
unsigned int nports;

const char **port_names;

jack_port_t **ports;
jack_default_audio_sample_t **in;
jack_nframes_t nframes;

const size_t sample_size = sizeof(jack_default_audio_sample_t);

jack_ringbuffer_t *rb;
pthread_mutex_t disk_thread_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  data_ready = PTHREAD_COND_INITIALIZER;

long overruns = 0;

void *
disk_thread (void *arg)
{
	jack_thread_info_t *info = (jack_thread_info_t *) arg;

	static jack_nframes_t total_captured = 0;

	jack_nframes_t samples_per_frame = info->channels;

	size_t bytes_per_frame = samples_per_frame * sample_size;

	void *framebuf = malloc (bytes_per_frame);

	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_mutex_lock (&disk_thread_lock);

	info->status = 0;

	while (1) {

		/* Write the data one frame at a time.  This is
		 * inefficient, but makes things simpler. */
		while (info->can_capture &&
		       (jack_ringbuffer_read_space (rb) >= bytes_per_frame)) {

			jack_ringbuffer_read (rb, framebuf, bytes_per_frame);

			if (sf_writef_float (info->sf, framebuf, 1) != 1) {
				char errstr[256];
				sf_error_str (0, errstr, sizeof (errstr) - 1);

				fprintf (stderr,
					 "cannot write sndfile (%s)\n",
					 errstr);

				info->status = EIO; /* write failed */

				goto done;
			}
				
			if (++total_captured >= info->duration) {
				printf ("disk thread finished\n");

				goto done;
			}
		}

		/* wait until process() signals more data */
		pthread_cond_wait (&data_ready, &disk_thread_lock);
	}

 done:
	pthread_mutex_unlock (&disk_thread_lock);
	free (framebuf);
	return 0;
}
	
int
process (jack_nframes_t nframes, void *arg)
{
	int chn;
	size_t i;
	jack_thread_info_t *info = (jack_thread_info_t *) arg;

	/* Do nothing until we're ready to begin. */
	if ((!info->can_process) || (!info->can_capture))
		return 0;

	for (chn = 0; chn < nports; chn++)
		in[chn] = jack_port_get_buffer (ports[chn], nframes);

	/* Sndfile requires interleaved data.  It is simpler here to
	 * just queue interleaved samples to a single ringbuffer. */
	for (i = 0; i < nframes; i++) {
		for (chn = 0; chn < nports; chn++) {

			if (jack_ringbuffer_write (rb, (void *) (in[chn]+i),
					      sample_size)
			    < sample_size)
				overruns++;
		}
	}

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
	// exit (0);
	abort();
}

void
setup_disk_thread (jack_thread_info_t *info)
{
	SF_INFO sf_info;
	int short_mask;
	
	sf_info.samplerate = jack_get_sample_rate (info->client);
	sf_info.channels = info->channels;
	
	switch (info->bitdepth) {
		case 8: short_mask = SF_FORMAT_PCM_U8;
		  	break;

		case 16: short_mask = SF_FORMAT_PCM_16;
			 break;

		case 24: short_mask = SF_FORMAT_PCM_24;
			 break;

		case 32: short_mask = SF_FORMAT_PCM_32;
			 break;

		default: short_mask = SF_FORMAT_PCM_16;
			 break;
	}		 
	sf_info.format = SF_FORMAT_WAV|short_mask;

	if ((info->sf = sf_open (info->path, SFM_WRITE, &sf_info)) == NULL) {
		char errstr[256];

		sf_error_str (0, errstr, sizeof (errstr) - 1);
		fprintf (stderr, "cannot open sndfile \"%s\" for output (%s)\n", info->path, errstr);
		jack_client_close (info->client);

		exit (1);
	}

	if (info->duration == 0) {
		info->duration = JACK_MAX_FRAMES;
	} else {
		info->duration *= sf_info.samplerate;
	}

	info->can_capture = 0;

	pthread_create (&info->thread_id, NULL, disk_thread, info);
}

void
run_disk_thread (jack_thread_info_t *info)
{
	info->can_capture = 1;
	pthread_join (info->thread_id, NULL);

	sf_close (info->sf);

	if (overruns > 0) {
		fprintf (stderr,
			 "jackrec failed with %ld overruns.\n", overruns);

		fprintf (stderr, " try a bigger buffer than -B %"
			 PRIu32 ".\n", info->rb_size);

		info->status = EPIPE;
	}
}

void
get_jack_ports(jack_thread_info_t *info)
{
  int i;

         if ((port_names = jack_get_ports (info->client, "PulseAudio", NULL, JackPortIsOutput)) == NULL) {
	   fprintf (stderr, "Cannot find any physical capture ports\n");
           exit(1);
         }
 
	 for (i = 0; port_names[i] != NULL; i++) {
	   fprintf (stderr, "refactor: found capture port  %s\n", port_names[i]);
	   if (i > 10) {
	     break;
	   }
	 }

}


void
setup_ports (int sources, jack_thread_info_t *info)
{
	unsigned int i;
	size_t in_size;

	get_jack_ports(info);

	/* Allocate data structures that depend on the number of ports. */
	nports = 2;
	ports = (jack_port_t **) malloc (sizeof (jack_port_t *) * nports);

	in_size =  nports * sizeof (jack_default_audio_sample_t *);

	in = (jack_default_audio_sample_t **) malloc (in_size);

	rb = jack_ringbuffer_create (nports * sample_size * info->rb_size);

	/* When JACK is running realtime, jack_activate() will have
	 * called mlockall() to lock our pages into memory.  But, we
	 * still need to touch any newly allocated pages before
	 * process() starts using them.  Otherwise, a page fault could
	 * create a delay that would force JACK to shut us down. */
	memset(in, 0, in_size);
	memset(rb->buf, 0, rb->size);

	for (i = 0; i < nports; i++) {
		char name[64];

		sprintf (name, "input%d", i+1);

		if ((ports[i] = jack_port_register (info->client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0)) == 0) {
			fprintf (stderr, "cannot register input port \"%s\"!\n", name);
			jack_client_close (info->client);
			exit (1);
		}
	}

	for (i = 0; i < nports; i++) {
		if (jack_connect (info->client, port_names[i], jack_port_name (ports[i]))) {
		  fprintf (stderr, "cannot connect input port %s to %s\n", port_names[i],jack_port_name (ports[i]) );
			jack_client_close (info->client);

			exit (1);
		} 
	}

	info->can_process = 1;		/* process() can start, now */
}

int
old_main (char *filename, int seconds, int bitdepth)

{
	jack_client_t *client;
	jack_thread_info_t thread_info;

	int c;
	int longopt_index = 0;

	extern int optind, opterr;

	int show_usage = 0;

	char *optstring = "d:f:b:B:h";

	struct option long_options[] = {
		{ "help", 0, 0, 'h' },
		{ "duration", 1, 0, 'd' },
		{ "file", 1, 0, 'f' },
		{ "bitdepth", 1, 0, 'b' },
		{ "bufsize", 1, 0, 'B' },
		{ 0, 0, 0, 0 }
	};

	memset (&thread_info, 0, sizeof (thread_info));
	thread_info.rb_size = DEFAULT_RB_SIZE;

	thread_info.duration = seconds;
	thread_info.bitdepth = bitdepth;
	thread_info.path = filename;

	opterr = 0;
	
	if ((client = jack_client_open ("jackrec", JackNullOption, NULL)) == 0) {
		fprintf (stderr, "jack server not running?\n");
		exit (1);
	}

	thread_info.client = client;
	thread_info.channels = 2;
	thread_info.can_process = 0;

	setup_disk_thread (&thread_info);

	jack_set_process_callback (client, process, &thread_info);
	jack_on_shutdown (client, jack_shutdown, &thread_info);

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
	}

	setup_ports (2, &thread_info);

	run_disk_thread (&thread_info);

	jack_client_close (client);

	jack_ringbuffer_free (rb);

	jack_free(port_names);

	return 0;
}
