#include <stdio.h>

#include "../include/systolic_array.h"

int main()
{

    float input[INPUT_SIZE];

    float weights[INPUT_SIZE][NUM_PE];

    /*
       Test data
       Later replace with files
    */

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        input[i] = 1.0;

        weights[i][0] = 0.5;

        weights[i][1] = 0.25;
    }

    SystolicArray array;

    systolic_reset(&array);

    systolic_compute(
        &array,
        input,
        weights);

    printf(
        "Hidden neuron 0 = %f\n",
        array.pe[0].partial_sum);

    printf(
        "Hidden neuron 1 = %f\n",
        array.pe[1].partial_sum);

    return 0;
}