#ifndef SOFTMAX_H
#define SOFTMAX_H

#include "common.h"


/*
 * Softmax converts output scores into probabilities.
 *
 * probability[i] =
 *
 * exp(score[i]) /
 * sum(exp(score))
 *
 */


void softmax(
    const nn_data_t input[OUTPUT_NODES],
    nn_data_t output[OUTPUT_NODES]
);


#endif
