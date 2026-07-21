#include <stdio.h>

#include "hidden_layer.h"



int main()
{

    HiddenLayer layer;


    nn_data_t input[784];

    nn_data_t weights[784][2];



    for(int i=0;i<784;i++)
    {

        input[i]=1.0;


        weights[i][0]=1.0;

        weights[i][1]=2.0;

    }



    hidden_layer_compute(
        &layer,
        input,
        weights);



    printf(
    "Cycles = %d\n",
    layer.cycle);



    printf(
    "Neuron0 = %f\n",
    layer.hidden_sum[0]);



    printf(
    "Neuron1 = %f\n",
    layer.hidden_sum[1]);



    return 0;

}

