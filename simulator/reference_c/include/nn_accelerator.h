#ifndef NN_ACCELERATOR_H
#define NN_ACCELERATOR_H

#include "common.h"


typedef struct
{

    int cycles;

    nn_data_t hidden_pre[HIDDEN_NODES];

    nn_data_t hidden_post[HIDDEN_NODES];

    nn_data_t output_score[OUTPUT_NODES];

    nn_data_t probability[OUTPUT_NODES];

    int prediction;


} nn_result_t;



void nn_inference(

    const nn_data_t input[INPUT_SIZE],

    const nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS],

    const nn_data_t output_weights
        [OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS],

    nn_result_t *result

);



#endif
