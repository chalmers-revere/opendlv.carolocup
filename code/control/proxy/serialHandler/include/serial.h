#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <glib.h>
#include "Protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{

	void (*on_read)(uint8_t b);

	void (*on_write)(uint8_t b);

	int fd;

	bool run;

	GAsyncQueue *incoming_queue;
	GAsyncQueue *outgoing_queue;

	pthread_t incoming_thread;
	pthread_t outgoing_thread;

	GMutex read_mutex;
	GMutex write_mutex;

} serial_state;

serial_state *serial_new();

void serial_free(serial_state *state);

bool serial_open(serial_state *state, const char *serialport, int baud);

void serial_handshake(serial_state *state, uint8_t hb);

void *serial_incoming_thread_routine(void *_state);

void *serial_outgoing_thread_routine(void *_state);

void serial_start(serial_state *state);

void serial_stop(serial_state *state);

bool serial_receive(serial_state *state, protocol_data *data);

void serial_send(serial_state *state, protocol_data data);

#ifdef __cplusplus
}
#endif

#endif
