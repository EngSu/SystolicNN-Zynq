#ifndef HIDDEN_LAYER_H
#define HIDDEN_LAYER_H


#include "common.h"



typedef struct
{

    nn_data_t output[HIDDEN_NODES];

    int cycles;


} hidden_layer_t;




void hidden_layer_reset(
    hidden_layer_t *layer
);



void hidden_layer_compute(

    hidden_layer_t *layer,

    const nn_data_t input[INPUT_SIZE],

    const nn_data_t weights
    [HIDDEN_WEIGHT_ROWS]
    [HIDDEN_WEIGHT_COLS]

);



#endif
