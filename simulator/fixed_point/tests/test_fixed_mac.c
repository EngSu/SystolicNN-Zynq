#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "fixed_point.h"

#define INPUT_SIZE_LOCAL 784
#define TOLERANCE 1e-4

/*
 * Fixed-point MAC test
 *
 * Uses:
 *   - first MNIST image
 *   - actual hidden-layer weights
 *
 * Floating-point reference:
 *     sum(x[i] * w[i])
 *
 * Fixed-point:
 *     Q1.15 x Q1.15
 *              |
 *              v
 *           Q2.30
 *              |
 *              v
 *        64-bit accumulation
 */

static double floating_mac(
    const nn_data_t input[],
    const nn_data_t weights[],
    int size)
{
    double sum = 0.0;

    for(int i = 0; i < size; i++)
    {
        sum += input[i] * weights[i];
    }

    return sum;
}


static double fixed_mac(
    const nn_data_t input[],
    const nn_data_t weights[],
    int size)
{
    int64_t accumulator = 0;

    for(int i = 0; i < size; i++)
    {
        q15_t input_fixed =
            float_to_q15(input[i]);

        q15_t weight_fixed =
            float_to_q15(weights[i]);

        int32_t product =
            (int32_t)input_fixed *
            (int32_t)weight_fixed;

        accumulator += product;
    }

    /*
     * Q2.30 -> floating point
     *
     * Each product contains 30 fractional bits.
     */
    return (double)accumulator / (double)(1LL << 30);
}


int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Fixed-Point MAC Test\n");
    printf("============================================================\n\n");


    /*
     * ------------------------------------------------------------
     * Allocate data
     * ------------------------------------------------------------
     */

    nn_data_t input[INPUT_SIZE_LOCAL];

    nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS];


    /*
     * ------------------------------------------------------------
     * Load first MNIST image
     * ------------------------------------------------------------
     */

    if(load_input_image(
            INPUT_FILE,
            input) != STATUS_OK)
    {
        printf("ERROR: Could not load input image.\n");
        return 1;
    }


    /*
     * ------------------------------------------------------------
     * Load hidden weights
     * ------------------------------------------------------------
     */

    if(load_hidden_weights(
            HIDDEN_WEIGHT_FILE,
            hidden_weights) != STATUS_OK)
    {
        printf("ERROR: Could not load hidden weights.\n");
        return 1;
    }


    /*
     * ------------------------------------------------------------
     * Prepare individual weight vectors
     *
     * hidden_weights[i][0] -> neuron 0
     * hidden_weights[i][1] -> neuron 1
     * ------------------------------------------------------------
     */

    nn_data_t weights_h0[INPUT_SIZE_LOCAL];
    nn_data_t weights_h1[INPUT_SIZE_LOCAL];

    for(int i = 0; i < INPUT_SIZE_LOCAL; i++)
    {
        weights_h0[i] = hidden_weights[i][0];
        weights_h1[i] = hidden_weights[i][1];
    }


    /*
     * ------------------------------------------------------------
     * Floating-point reference
     * ------------------------------------------------------------
     */

    double floating_h0 =
        floating_mac(
            input,
            weights_h0,
            INPUT_SIZE_LOCAL
        );

    double floating_h1 =
        floating_mac(
            input,
            weights_h1,
            INPUT_SIZE_LOCAL
        );


    /*
     * ------------------------------------------------------------
     * Fixed-point MAC
     * ------------------------------------------------------------
     */

    double fixed_h0 =
        fixed_mac(
            input,
            weights_h0,
            INPUT_SIZE_LOCAL
        );

    double fixed_h1 =
        fixed_mac(
            input,
            weights_h1,
            INPUT_SIZE_LOCAL
        );


    /*
     * ------------------------------------------------------------
     * Calculate errors
     * ------------------------------------------------------------
     */

    double error_h0 =
        fixed_h0 - floating_h0;

    double error_h1 =
        fixed_h1 - floating_h1;


    double abs_error_h0 =
        fabs(error_h0);

    double abs_error_h1 =
        fabs(error_h1);


    /*
     * ------------------------------------------------------------
     * Display results
     * ------------------------------------------------------------
     */

    printf("First MNIST image\n");
    printf("------------------------------\n\n");

    printf("Hidden neuron 0:\n");
    printf("  Floating-point = %.12f\n", floating_h0);
    printf("  Fixed-point    = %.12f\n", fixed_h0);
    printf("  Error          = %+.12e\n", error_h0);
    printf("  Absolute error = %.12e\n\n", abs_error_h0);


    printf("Hidden neuron 1:\n");
    printf("  Floating-point = %.12f\n", floating_h1);
    printf("  Fixed-point    = %.12f\n", fixed_h1);
    printf("  Error          = %+.12e\n", error_h1);
    printf("  Absolute error = %.12e\n\n", abs_error_h1);


    /*
     * ------------------------------------------------------------
     * Verification
     * ------------------------------------------------------------
     */

    int pass =
        (abs_error_h0 <= TOLERANCE) &&
        (abs_error_h1 <= TOLERANCE);


    printf("------------------------------\n");

    if(pass)
    {
        printf("Fixed-point MAC test: PASS\n");
    }
    else
    {
        printf("Fixed-point MAC test: FAIL\n");
    }

    printf("------------------------------\n");


    return pass ? 0 : 1;
}
