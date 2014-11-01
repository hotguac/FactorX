/*
  Copyright (C) 2014 Joe Kokosa

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include <inttypes.h>
#include <sndfile.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#ifndef __jack_stuff_h__
#define __jack_stuff_h__

#ifdef __cplusplus
#define BEGIN_C_DECLS extern "C" {
#define END_C_DECLS   }
#else /* !__cplusplus */
#define BEGIN_C_DECLS
#define END_C_DECLS
#endif /* __cplusplus */

typedef struct {
	int overruns;
	unsigned int size;
	char buffer[1024];
} sysex_msg;

typedef struct _thread_info {
	pthread_t thread_id;
	SNDFILE *sf;

	jack_nframes_t duration;
	jack_nframes_t rb_size;
	jack_client_t *client;

	unsigned int channels;
	int bitdepth;
	char *path;
	volatile int can_capture;
	volatile int can_process;
	volatile int status;
} jack_thread_info_t;

BEGIN_C_DECLS void *disk_thread(void *);

int process(jack_nframes_t, void *);

void jack_shutdown();

int get_current_patch(char *buffer, int bsize);

int init();

int send_immediate(char *buffer, int bsize);

char *input_port_name(int port);
char *output_port_name(int port);

int connect_to_input(char *buffer);
int connect_to_output(char *buffer);

END_C_DECLS
/* Synchronization between process thread and disk thread. */
//#define DEFAULT_RB_SIZE 16384 /* ringbuffer size in frames */
#define DEFAULT_RB_SIZE 20	/* ringbuffer size in frames */

#ifndef MAX
#define MAX(a,b) ( (a) < (b) ? (b) : (a) )
#endif

static uint64_t waiting_for_more = 0;

#endif /* __jack_h__ */
