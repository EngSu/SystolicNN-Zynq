#ifndef SYSTOLIC_ARRAY_H
#define SYSTOLIC_ARRAY_H

#include "common.h"
#include "pe.h"

#define NUM_PE 2

typedef struct
{

    ProcessingElement pe[NUM_PE];

} SystolicArray;

/*
 * Initialize systolic array
 */

void systolic_array_reset(
    SystolicArray *array);

/*
 * Load one input and weight into PE
 */

void systolic_array_load(
    SystolicArray *array,
    int pe_index,
    nn_data_t input,
    nn_data_t weight);

/*
 * Execute one clock cycle
 */

void systolic_array_tick(
    SystolicArray *array);

/*
 * Read PE result
 */

nn_data_t systolic_array_get_output(
    SystolicArray *array,
    int pe_index);

#endif