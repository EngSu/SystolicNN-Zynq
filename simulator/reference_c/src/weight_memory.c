#include "weight_memory.h"

#include <string.h>

void weight_memory_init(
    WeightMemory *memory)
{

    memset(memory,
           0,
           sizeof(WeightMemory));
}

nn_data_t get_hidden_weight(
    WeightMemory *memory,
    int input,
    int neuron)
{

    return memory->hidden_weights[input][neuron];
}

nn_data_t get_output_weight(
    WeightMemory *memory,
    int hidden,
    int output)
{

    return memory->output_weights[hidden][output];
}