#include "../include/systolic_array.h"

void systolic_reset(
    SystolicArray *array)
{

    for (int i = 0; i < NUM_PE; i++)
    {
        pe_reset(&array->pe[i]);
    }
}

void systolic_compute(
    SystolicArray *array,
    float input[],
    float weights[][NUM_PE])
{

    for (int cycle = 0;
         cycle < INPUT_SIZE;
         cycle++)
    {

        for (int pe_index = 0;
             pe_index < NUM_PE;
             pe_index++)
        {

            pe_compute(
                &array->pe[pe_index],
                input[cycle],
                weights[cycle][pe_index]);
        }
    }
}