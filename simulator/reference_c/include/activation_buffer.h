#ifndef ACTIVATION_BUFFER_H
#define ACTIVATION_BUFFER_H

#include "common.h"

typedef struct
{

    nn_data_t hidden_pre[HIDDEN_NODES];

    nn_data_t hidden_activation[HIDDEN_NODES];

    nn_data_t output_score[OUTPUT_NODES];

    nn_data_t probability[OUTPUT_NODES];

} ActivationBuffer;

void activation_buffer_init(
    ActivationBuffer *buffer);

#endif