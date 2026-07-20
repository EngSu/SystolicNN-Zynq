#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include "common.h"

/*
 * Input Buffer
 *
 * Stores one MNIST image.
 *
 * Hardware equivalent:
 *
 * BRAM / distributed RAM
 *
 */

typedef struct
{
    nn_data_t data[INPUT_SIZE];

} InputBuffer;

void input_buffer_init(InputBuffer *buffer);

status_t input_buffer_load(
    InputBuffer *buffer,
    nn_data_t *input);

nn_data_t input_buffer_read(
    InputBuffer *buffer,
    int address);

#endif