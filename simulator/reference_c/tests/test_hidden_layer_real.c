#include <stdio.h>

#include "hidden_layer.h"
#include "data_loader.h"
#include "paths.h"

int main()
{

    NeuralNetworkData data;

    HiddenLayer layer;

    if (load_inputs(
            INPUT_FILE,
            &data))
    {
        printf("Input loading failed\n");
        return -1;
    }

    if (load_hidden_weights(
            HIDDEN_WEIGHT_FILE,
            &data))
    {
        printf("Weight loading failed\n");
        return -1;
    }

    hidden_layer_compute(
        &layer,
        data.input,
        data.hidden_weights);

    printf("====================\n");

    printf("Hidden Layer Result\n");

    printf("====================\n");

    printf(
        "Neuron 0 = %.6f\n",
        layer.hidden_sum[0]);

    printf(
        "Neuron 1 = %.6f\n",
        layer.hidden_sum[1]);

    printf(
        "Latency = %d cycles\n",
        layer.cycle);

    return 0;
}
