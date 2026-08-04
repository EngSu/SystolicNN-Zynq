#include <stdio.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "hidden_layer.h"

#define TOLERANCE 1e-9

int main(void)
{
    printf("==============================\n");
    printf(" Hidden Layer Verification Test\n");
    printf("==============================\n\n");

    /*
     * ------------------------------------------------------------
     * Allocate input and weight storage
     * ------------------------------------------------------------
     */

    nn_data_t input[INPUT_SIZE];

    nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS];

    /*
     * ------------------------------------------------------------
     * Load input
     * ------------------------------------------------------------
     */

    status_t status;

    status = load_input_image(
        INPUT_FILE,
        input);

    if (status != STATUS_OK)
    {
        printf("ERROR: Failed to load input image.\n");
        return 1;
    }

    /*
     * ------------------------------------------------------------
     * Load hidden-layer weights
     * ------------------------------------------------------------
     */

    status = load_hidden_weights(
        HIDDEN_WEIGHT_FILE,
        hidden_weights);

    if (status != STATUS_OK)
    {
        printf("ERROR: Failed to load hidden weights.\n");
        return 1;
    }

    /*
     * ------------------------------------------------------------
     * Run simulator hidden layer
     * ------------------------------------------------------------
     */

    hidden_layer_t layer;

    hidden_layer_compute(
        &layer,
        input,
        hidden_weights);

    /*
     * ------------------------------------------------------------
     * Independent golden-model calculation
     *
     * This calculation intentionally does NOT use
     * hidden_layer_compute().
     *
     * It provides an independent reference against which
     * the simulator implementation is verified.
     * ------------------------------------------------------------
     */

    nn_data_t golden[HIDDEN_NODES] = {0.0};

    for (int neuron = 0;
         neuron < HIDDEN_NODES;
         neuron++)
    {

        for (int i = 0;
             i < INPUT_SIZE;
             i++)
        {

            golden[neuron] +=
                input[i] *
                hidden_weights[i][neuron];
        }
    }

    /*
     * ------------------------------------------------------------
     * Display results
     * ------------------------------------------------------------
     */

    printf("Simulator output:\n");

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {
        printf(
            "H%d = %.9f\n",
            i,
            layer.output[i]);
    }

    printf("\nGolden-model output:\n");

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {
        printf(
            "H%d = %.9f\n",
            i,
            golden[i]);
    }

    /*
     * ------------------------------------------------------------
     * Numerical verification
     * ------------------------------------------------------------
     */

    int pass = 1;

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {

        double error =
            fabs(
                layer.output[i] -
                golden[i]);

        printf(
            "\nH%d error = %.12e",
            i,
            error);

        if (error > TOLERANCE)
        {
            pass = 0;

            printf("  FAIL");
        }
        else
        {
            printf("  PASS");
        }
    }

    /*
     * ------------------------------------------------------------
     * Verify cycle count
     *
     * Current implementation performs:
     *
     * 784 inputs × 2 hidden neurons
     * = 1568 MAC operations
     * ------------------------------------------------------------
     */

    int expected_cycles =
        INPUT_SIZE * HIDDEN_NODES;

    printf(
        "\n\nSimulator cycles  = %d",
        layer.cycles);

    printf(
        "\nExpected cycles   = %d",
        expected_cycles);

    if (layer.cycles != expected_cycles)
    {
        printf("  FAIL\n");
        pass = 0;
    }
    else
    {
        printf("  PASS\n");
    }

    /*
     * ------------------------------------------------------------
     * Final result
     * ------------------------------------------------------------
     */

    printf("\n==============================\n");

    if (pass)
    {
        printf(" HIDDEN LAYER TEST: PASS\n");
        printf("==============================\n");

        return 0;
    }
    else
    {
        printf(" HIDDEN LAYER TEST: FAIL\n");
        printf("==============================\n");

        return 1;
    }
}
