#include "input_buffer.h"

#include <string.h>

void input_buffer_init(InputBuffer *buffer)
{

    memset(buffer->data,
           0,
           sizeof(buffer->data));
}

status_t input_buffer_load(
    InputBuffer *buffer,
    nn_data_t *input)
{

    if (buffer == NULL || input == NULL)
        return STATUS_ERROR;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        buffer->data[i] = input[i];
    }

    return STATUS_OK;
}

nn_data_t input_buffer_read(
    InputBuffer *buffer,
    int address)
{

    if (address < 0 || address >= INPUT_SIZE)
        return 0;

    return buffer->data[address];
}