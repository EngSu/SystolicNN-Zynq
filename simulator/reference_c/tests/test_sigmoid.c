#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "sigmoid.h"

#define EPSILON 1e-6

int main(void)
{

    printf("\n");
    printf("==============================\n");
    printf(" Sigmoid Unit Test\n");
    printf("==============================\n\n");

    assert(fabs(sigmoid(0.0) - 0.5) < EPSILON);

    assert(fabs(sigmoid(1.0) - 0.7310585786) < EPSILON);

    assert(fabs(sigmoid(-1.0) - 0.2689414214) < EPSILON);

    printf("Scalar sigmoid passed\n");

    nn_data_t input[HIDDEN_NODES] =
        {
            0.0,
            1.0};

    nn_data_t output[HIDDEN_NODES];

    sigmoid_vector(input, output);

    assert(fabs(output[0] - 0.5) < EPSILON);

    assert(fabs(output[1] - 0.7310585786) < EPSILON);

    printf("Vector sigmoid passed\n");

    printf("\nAll sigmoid tests PASSED\n");

    return 0;
}