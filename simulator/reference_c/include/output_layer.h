#ifndef OUTPUT_LAYER_H
#define OUTPUT_LAYER_H

#include "common.h"


/*
 * Output layer simulator.
 *
 * Input:
 *   hidden activation vector
 *
 * Weight:
 *   2 x 2 matrix
 *
 * Output:
 *   two class scores
 */

typedef struct
{
    nn_data_t score[OUTPUT_NODES];

    int cycles;

} output_layer_t;



void output_layer_reset(
    output_layer_t *layer);



void output_layer_compute(
    output_layer_t *layer,

    const nn_data_t hidden[HIDDEN_NODES],

    const nn_data_t weights[
        OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS]
);


#endif
