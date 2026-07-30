#include "nn_accelerator.h"

#include "hidden_layer.h"
#include "sigmoid.h"
#include "output_layer.h"
#include "softmax.h"
#include "predictor.h"



void nn_inference(

    const nn_data_t input[INPUT_SIZE],

    const nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS],

    const nn_data_t output_weights
        [OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS],

    nn_result_t *result

)
{

    hidden_layer_t hidden;


    output_layer_t output;



    /*
     * Layer 1:
     *
     * 784 inputs
     * 784x2 weights
     *
     */

    hidden_layer_compute(

        &hidden,

        input,

        hidden_weights

    );



    for(int i=0;i<HIDDEN_NODES;i++)
    {
        result->hidden_pre[i] =
            hidden.output[i];
    }



    /*
     * Activation
     */

    sigmoid_vector(

        result->hidden_pre,

        result->hidden_post

    );



    /*
     * Layer 2:
     *
     * 2 inputs
     * 2x2 weights
     */

    output_layer_compute(

        &output,

        result->hidden_post,

        output_weights

    );


    for(int i=0;i<OUTPUT_NODES;i++)
    {
        result->output_score[i] =
            output.score[i];
    }



    /*
     * Softmax
     */

    softmax(

        result->output_score,

        result->probability

    );



    /*
     * Prediction
     */

    result->prediction =
        predict(result->probability);



    result->cycles =
        hidden.cycles
        +
        output.cycles;

}
