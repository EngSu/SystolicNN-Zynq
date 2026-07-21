#include <math.h>

#include "softmax.h"



void softmax(
    const nn_data_t input[OUTPUT_NODES],
    nn_data_t output[OUTPUT_NODES]
)
{

    nn_data_t exp_sum = 0.0;


    /*
     * Calculate exponentials
     */

    nn_data_t exp_values[OUTPUT_NODES];


    for(int i = 0; i < OUTPUT_NODES; i++)
    {
        exp_values[i] = exp(input[i]);

        exp_sum += exp_values[i];
    }



    /*
     * Normalize
     */

    for(int i = 0; i < OUTPUT_NODES; i++)
    {
        output[i] =
            exp_values[i] /
            exp_sum;
    }

}
