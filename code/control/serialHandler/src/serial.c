#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <glib.h>
#include "arduino.h"

serial_state *serial_new()
{
    serial_state *state = (serial_state *) malloc(sizeof(serial_state));

    state->fd = -1;

    state->run = false;

    state->incoming_queue = g_async_queue_new();
    state->outgoing_queue = g_async_queue_new();

    g_mutex_init(&state->read_mutex);
    g_mutex_init(&state->write_mutex);

    return state;
}

void serial_free(serial_state *state)
{
    g_async_queue_unref(state->incoming_queue);
    g_async_queue_unref(state->outgoing_queue);

    free(state);
}

bool serial_open(serial_state *state, const char *serialport, int baud)
{
    int fd = serialport_init(serialport, baud);

    if (fd == -1)
    {
        throw "No Port Created!";
    } else
    {
        state->fd = fd;
        return true;
    }
}

void serial_handshake(serial_state *state, uint8_t hb)
{
    uint8_t b = 0;
    do
    {
        serialport_readbyte(state->fd, &b);
    } while (b != hb);
}

void *serial_incoming_thread_routine(void *_state)
{
    serial_state *state = (serial_state *) _state;

    do
    {
        g_mutex_lock(&state->read_mutex);

        uint8_t b;
        int n = serialport_readbyte(state->fd, &b);
        if (n != -1)
        {
            state->on_read(b);

//            if (state->incoming_frame_t == FRAME_T1) {
//                protocol_frame _frame;
//                _frame.a = b;
//
//                protocol_data data = protocol_decode_t1(_frame);
//                protocol_data *_data = (protocol_data *) malloc(sizeof(protocol_data));
//
//                _data->id = data.id;
//                _data->value = data.value;
//
//                g_async_queue_push(state->incoming_queue, _data);
//            }
//
//            if (state->incoming_frame_t == FRAME_T2) {
//
//                protocol_frame _frame;
//                _frame.a = b;
//
//                protocol_data data = protocol_decode_t2(_frame);
//                protocol_data *_data = (protocol_data *) malloc(sizeof(protocol_data));
//
//                _data->id = data.id;
//                _data->value = data.value * 3;
//                if (data.id == 6) {
//                    if (_data->value == 3) {
//                        _data->value = 1;
//                    } else if (_data->value == 6) {
//                        _data->value = 2;
//                    } else if (_data->value == 9) {
//                        _data->value = 3;
//                    } else if (_data->value == 12) {
//                        _data->value = 4;
//                    }else if (_data->value == 15) {
//                        _data->value = 5;
//                    }
//                }
//
//                g_async_queue_push(state->incoming_queue, _data);
//            }
        }

        g_mutex_unlock(&state->read_mutex);

        sleep(.01);

    } while (state->run);

    return NULL;
}

void *serial_outgoing_thread_routine(void *_state)
{
    serial_state *state = (serial_state *) _state;

    do
    {
        protocol_data *data = (protocol_data *) g_async_queue_try_pop(state->outgoing_queue);
        if (data == NULL)
        {
            continue;
        }

        g_mutex_lock(&state->write_mutex);

//        if (state->outgoing_frame_t == FRAME_T1) {
//            protocol_frame frame = protocol_encode_t1(*data);
//
//            serialport_writebyte(state->fd, frame.a);
//
//            state->on_write(frame.a);
//        }
//
//        if (state->outgoing_frame_t == FRAME_T2) {
//            protocol_frame frame = protocol_encode_t2(*data);
//
//            serialport_writebyte(state->fd, frame.a);
//
//            state->on_write(frame.a);
//        }

        g_mutex_unlock(&state->write_mutex);

        free(data);

        sleep(.01);

    } while (state->run);

    return NULL;
}

void serial_start(serial_state *state)
{
    state->run = true;

    pthread_create(&state->incoming_thread,
                   NULL,
                   &serial_incoming_thread_routine,
                   state);

    pthread_create(&state->outgoing_thread,
                   NULL,
                   &serial_outgoing_thread_routine,
                   state);
}

void serial_stop(serial_state *state)
{
    state->run = false;

    pthread_join(state->incoming_thread, NULL);
    pthread_join(state->outgoing_thread, NULL);
}

bool serial_receive(serial_state *state, protocol_data *data)
{
    protocol_data *_data = (protocol_data *) g_async_queue_try_pop(state->incoming_queue);
    if (data == NULL)
    {
        return false;
    }

    data->id = _data->id;
    data->value = _data->value;

    free(_data);

    return true;
}

void serial_send(serial_state *state, protocol_data data)
{
    protocol_data *_data = (protocol_data *) malloc(sizeof(protocol_data));
    _data->id = data.id;
    _data->value = data.value;

    g_async_queue_push(state->outgoing_queue, _data);
}