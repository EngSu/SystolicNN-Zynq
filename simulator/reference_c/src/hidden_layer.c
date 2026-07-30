#include "hidden_layer.h"


void hidden_layer_reset(
    hidden_layer_t *layer
)
{

    for(int i=0;i<HIDDEN_NODES;i++)
    {
        layer->output[i]=0.0;
    }

    layer->cycles=0;

}



void hidden_layer_compute(

    hidden_layer_t *layer,

    const nn_data_t input[INPUT_SIZE],

    const nn_data_t weights
    [HIDDEN_WEIGHT_ROWS]
    [HIDDEN_WEIGHT_COLS]

)
{

    hidden_layer_reset(layer);



    for(int neuron=0;
        neuron<HIDDEN_NODES;
        neuron++)
    {


        for(int i=0;
            i<INPUT_SIZE;
            i++)
        {


            layer->output[neuron]
                += input[i]
                *
                weights[i][neuron];


            layer->cycles++;

        }

    }

}
