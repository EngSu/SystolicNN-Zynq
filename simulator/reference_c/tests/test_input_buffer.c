#include <stdio.h>

#include "input_buffer.h"

int main(void)
{
    InputBuffer input;

    input_buffer_init(&input);

    nn_data_t image[INPUT_SIZE];

    for (int i = 0; i < INPUT_SIZE; i++)
        image[i] = i;

    input_buffer_load(&input, image);

    printf("%f\n", input_buffer_read(&input, 0));
    printf("%f\n", input_buffer_read(&input, 1));
    printf("%f\n", input_buffer_read(&input, 2));
    printf("%f\n", input_buffer_read(&input, 783));

    return 0;
}