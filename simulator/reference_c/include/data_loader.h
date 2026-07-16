#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#define INPUT_SIZE 784
#define HIDDEN_SIZE 2

int load_inputs(
    const char *filename,
    float input[INPUT_SIZE]);

int load_hidden_weights(
    const char *filename,
    float weights[INPUT_SIZE][HIDDEN_SIZE]);

#endif