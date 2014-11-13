/*
  Copyright (C) 2014 Joe Kokosa
*/

#include <string.h>

#include "jack_stuff.h"

#define JACK_CLIENT_NAME "reFactor"
#define SHORT_INPUT_PORT "input"
#define LONG_INPUT_PORT "reFactor:input"
#define SHORT_OUTPUT_PORT "output"
#define LONG_OUTPUT_PORT "reFactor:output"

/* JACK data */
jack_client_t *client;

jack_port_t *inputPort;
jack_port_t *outputPort;

jack_nframes_t nframes;
jack_ringbuffer_t *recv_buffer;
jack_ringbuffer_t *send_buffer;

const char **port_names_input;
const char **port_names_output;

pthread_mutex_t msg_thread_lock = PTHREAD_MUTEX_INITIALIZER;

int process(jack_nframes_t nframes, void *arg)
{
	//int chn;
	size_t i;
	jack_thread_info_t *info = (jack_thread_info_t *) arg;

	void *buffer;
	jack_nframes_t num_messages;
	jack_nframes_t msg_num;

	/* First receive data */
	buffer = jack_port_get_buffer(inputPort, nframes);

	num_messages = jack_midi_get_event_count(buffer);

	for (msg_num = 0; msg_num < num_messages; ++msg_num) {
		jack_midi_event_t event;
		int result;

		result = jack_midi_event_get(&event, buffer, msg_num);

		if (result == 0
		    && jack_ringbuffer_write_space(recv_buffer) >=
		    sizeof(sysex_msg)) {
			sysex_msg m;
			m.size = event.size;

			if ((event.buffer[0] == 0xF0) || (waiting_for_more)) {
				memcpy(m.buffer, event.buffer,
				       MAX(sizeof(m.buffer), event.size));
				jack_ringbuffer_write(recv_buffer, (void *)&m,
						      sizeof(sysex_msg));

				waiting_for_more = 1;
				for (i = 0; i < event.size; i++) {
					if (m.buffer[i] == 0xF7) {
						waiting_for_more = 0;
					}
				}

			}
		}
	}

	/* Now do the send if necessary */
	void *port_buf = jack_port_get_buffer(outputPort, nframes);

	jack_midi_clear_buffer(port_buf);

	const int mqlen =
	    jack_ringbuffer_read_space(send_buffer) / sizeof(sysex_msg);

	sysex_msg m;

	if (mqlen > 0) {
		jack_ringbuffer_read(send_buffer, (char *)&m,
				     sizeof(sysex_msg));

		unsigned char *another_buffer =
		    jack_midi_event_reserve(port_buf, 0, m.size);

		int i;

		for (i = 0; i < m.size; ++i)
			another_buffer[i] = m.buffer[i];
	}

	return 0;
}

int send_immediate(char *buffer, int bsize)
{
	pthread_mutex_lock(&msg_thread_lock);

	sysex_msg m;

	m.size = bsize;

	int i;
	int n;

	if (bsize > sizeof(m.buffer))
		n = sizeof(m.buffer);
	else
		n = bsize;

	for (i = 0; i < n; ++i) {
		m.buffer[i] = buffer[i];
	}

	jack_ringbuffer_write(send_buffer, (void *)&m, sizeof(sysex_msg));

	pthread_mutex_unlock(&msg_thread_lock);
}

void jack_end()
{
	jack_deactivate(client);
	jack_client_close(client);	// close the client
	jack_ringbuffer_free(recv_buffer);
	jack_ringbuffer_free(send_buffer);

	client = NULL;
}

void jack_shutdown()
{
	jack_ringbuffer_free(recv_buffer);
	jack_ringbuffer_free(send_buffer);

	fprintf(stderr, "JACK shutdown\n");

	outputPort = NULL;
	inputPort = NULL;;

	if (client != NULL) {
		jack_end();
	}
}

char *input_port_name(int port)
{
	int i;

	if ((port_names_input =
	     jack_get_ports(client, NULL, "midi", JackPortIsInput)) == NULL) {
		fprintf(stderr, "Cannot find any ports to send to\n");
		exit(1);
	}

	for (i = 0; port >= i; i++) {
		if (port_names_input[i] == NULL) {
			break;
		}
	}

	if (i > port)
		return port_names_input[port];
	else
		return "DONE";
}

char *output_port_name(int port)
{
	int i;

	if ((port_names_output =
	     jack_get_ports(client, NULL, "midi", JackPortIsOutput)) == NULL) {
		fprintf(stderr, "Cannot find any ports to send to\n");
		exit(1);
	}

	for (i = 0; port >= i; i++) {
		if (port_names_output[i] == NULL) {
			break;
		}
	}

	if (i > port)
		return port_names_output[port];
	else
		return "DONE";
}

int connect_to_input(char *port_name)
{
	int result;

	fprintf(stderr, "Connect to input Port : %s requested\n", port_name);

	// first disconnect any existing connections
	jack_port_disconnect(client, inputPort);

	// now connect try to connect to requested port
	result = jack_connect(client, port_name, LONG_INPUT_PORT);

	return result;
}

int connect_to_output(char *port_name)
{
	int result;

	fprintf(stderr, "Connect to output Port : %s requested\n", port_name);

	// first disconnect any existing connections
	jack_port_disconnect(client, outputPort);

	// now connect try to connect to requested port
	result = jack_connect(client, LONG_OUTPUT_PORT, port_name);

	return result;
}

int setup_ports()
{
	inputPort =
	    jack_port_register(client, "input", JACK_DEFAULT_MIDI_TYPE,
			       JackPortIsInput, 0);
	if (inputPort == NULL) {
		fprintf(stderr, "%s failed to register input port",
			JACK_CLIENT_NAME);
	}

	outputPort =
	    jack_port_register(client, "output", JACK_DEFAULT_MIDI_TYPE,
			       JackPortIsOutput, 0);
	if (outputPort == NULL) {
		fprintf(stderr, "%s failed to register output port",
			JACK_CLIENT_NAME);
	}

	return 0;
}

int init()
{
	client = jack_client_open(JACK_CLIENT_NAME, JackNullOption, NULL);

	if (client == NULL) {
		fprintf(stderr, "jack server not running?\n");
		exit(1);
	}

	recv_buffer =
	    jack_ringbuffer_create(DEFAULT_RB_SIZE * sizeof(sysex_msg));

	if (recv_buffer) {
		jack_ringbuffer_mlock(recv_buffer);
	} else {
		fprintf(stderr, "reFactor: Error allocating recv_buffer\n");
		exit(1);
	}

	send_buffer =
	    jack_ringbuffer_create(DEFAULT_RB_SIZE * sizeof(sysex_msg));

	if (send_buffer) {
		jack_ringbuffer_mlock(send_buffer);
	} else {
		fprintf(stderr, "reFactor: Error allocating send_buffer\n");
		exit(1);
	}

	if (jack_set_process_callback(client, process, NULL)) {
		fprintf(stderr, "reFactor: error setting callback\n");
	}

	jack_on_shutdown(client, jack_shutdown, NULL);

	if (jack_activate(client)) {
		fprintf(stderr, "cannot activate client");
	}

	setup_ports();

	return 0;
}

int get_current_patch(char *buffer, int bsize)
{
	int level = 0;

	pthread_mutex_lock(&msg_thread_lock);

	const int num_messages =
	    jack_ringbuffer_read_space(recv_buffer) / sizeof(sysex_msg);

	int msg_num;
	for (msg_num = 0; msg_num < num_messages; ++msg_num) {
		size_t j;
		sysex_msg m;

		jack_ringbuffer_read(recv_buffer, (char *)&m,
				     sizeof(sysex_msg));

		for (j = 0; j < m.size && j < sizeof(m.buffer); ++j) {
			if (level < bsize) {
				buffer[level++] = m.buffer[j];
			}
		}
	}

	pthread_mutex_unlock(&msg_thread_lock);

	return level;
}
