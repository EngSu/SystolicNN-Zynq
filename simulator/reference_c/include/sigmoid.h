#ifndef SIGMOID_H
#define SIGMOID_H

#include "common.h"


/*
 * Floating point sigmoid function.
 *
 * Later replaced by FPGA LUT model.
 */

nn_data_t sigmoid(nn_data_t x);


/*
 * Apply sigmoid activation to hidden layer outputs.
 */

void sigmoid_vector(
    const nn_data_t input[HIDDEN_NODES],
    nn_data_t output[HIDDEN_NODES]
);


#endif
