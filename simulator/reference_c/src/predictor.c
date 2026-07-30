#include "predictor.h"



int predict(
    const nn_data_t probability[OUTPUT_NODES]
)
{

    int prediction = 0;


    nn_data_t max_value =
        probability[0];


    for(int i = 1; i < OUTPUT_NODES; i++)
    {

        if(probability[i] > max_value)
        {
            max_value = probability[i];

            prediction = i;
        }

    }


    return prediction;

}
