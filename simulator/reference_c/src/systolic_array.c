#include "systolic_array.h"

void systolic_array_reset(
    SystolicArray *array)
{

    for (int i = 0; i < NUM_PE; i++)
    {
        pe_reset(&array->pe[i]);
    }
}

void systolic_array_load(
    SystolicArray *array,
    int pe_index,
    nn_data_t input,
    nn_data_t weight)
{

    if (pe_index < NUM_PE)
    {

        pe_load_input(
            &array->pe[pe_index],
            input);

        pe_load_weight(
            &array->pe[pe_index],
            weight);
    }
}

void systolic_array_tick(
    SystolicArray *array)
{

    for (int i = 0; i < NUM_PE; i++)
    {

        array->pe[i].enable = true;

        pe_tick(
            &array->pe[i]);
    }
}

nn_data_t systolic_array_get_output(
    SystolicArray *array,
    int pe_index)
{

    return pe_get_output(
        &array->pe[pe_index]);
}