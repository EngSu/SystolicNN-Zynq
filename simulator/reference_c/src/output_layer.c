#include <stdio.h>

#include "output_layer.h"



void output_layer_reset(output_layer_t *layer)
{

    for(int i = 0; i < OUTPUT_NODES; i++)
    {
        layer->score[i] = 0.0;
    }

    layer->cycles = 0;

}



void output_layer_compute(

    output_layer_t *layer,

    const nn_data_t hidden[HIDDEN_NODES],

    const nn_data_t weights
        [OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS]

)
{


    output_layer_reset(layer);



    /*
     * Hardware behavior:
     *
     * Cycle 0:
     *   PE computes contribution from hidden[0]
     *
     * Cycle 1:
     *   PE computes contribution from hidden[1]
     *
     */


    for(int i = 0; i < HIDDEN_NODES; i++)
    {

        for(int j = 0; j < OUTPUT_NODES; j++)
        {

            layer->score[j] +=
                hidden[i] *
                weights[i][j];

        }


        layer->cycles++;

    }

}
