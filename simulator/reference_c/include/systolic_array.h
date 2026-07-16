#ifndef SYSTOLIC_ARRAY_H
#define SYSTOLIC_ARRAY_H

#include "pe.h"

#define INPUT_SIZE 784
#define NUM_PE 2

typedef struct
{

    PE pe[NUM_PE];

} SystolicArray;

void systolic_reset(
    SystolicArray *array);

void systolic_compute(
    SystolicArray *array,
    float input[],
    float weights[][NUM_PE]);

#endif