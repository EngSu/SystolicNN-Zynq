#ifndef WEIGHT_MEMORY_H
#define WEIGHT_MEMORY_H

#include "common.h"

typedef struct
{

    nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS];

    nn_data_t output_weights
        [OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS];

} WeightMemory;

void weight_memory_init(
    WeightMemory *memory);

nn_data_t get_hidden_weight(
    WeightMemory *memory,
    int input,
    int neuron);

nn_data_t get_output_weight(
    WeightMemory *memory,
    int hidden,
    int output);

#endif