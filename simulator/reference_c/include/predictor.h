#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "common.h"


/*
 * Selects the class with maximum probability.
 *
 * Return:
 * 0 -> class 0
 * 1 -> class 1
 */

int predict(
    const nn_data_t probability[OUTPUT_NODES]
);


#endif
