#include <math.h>

#include "sigmoid.h"


/*
 * Scalar sigmoid
 *
 * y = 1 / (1 + e^-x)
 */

nn_data_t sigmoid(nn_data_t x)
{
    return 1.0 / (1.0 + exp(-x));
}


/*
 * Vector sigmoid
 *
 * Hidden layer contains 2 neurons.
 */

void sigmoid_vector(
    const nn_data_t input[HIDDEN_NODES],
    nn_data_t output[HIDDEN_NODES])
{

    for(int i = 0; i < HIDDEN_NODES; i++)
    {
        output[i] = sigmoid(input[i]);
    }

}
