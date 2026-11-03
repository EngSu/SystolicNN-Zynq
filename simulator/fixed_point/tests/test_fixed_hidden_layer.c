#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"
#include "fixed_point.h"

#define NUM_IMAGES 2115
#define TOLERANCE 1e-4

/*
 * ============================================================
 * Fixed-Point Hidden Neuron
 * ============================================================
 *
 * Input:
 *     input[i]       : floating-point value
 *     weights[i][n]  : floating-point weight
 *
 * Quantization:
 *     float -> Q1.15
 *
 * Multiplication:
 *
 *     Q1.15 x Q1.15 = Q2.30
 *
 * Accumulation:
 *     int64_t
 *
 * Conversion:
 *
 *     Q2.30 -> floating point
 *
 * The returned value is therefore the fixed-point
 * approximation of the hidden-neuron pre-activation.
 *
 * ============================================================
 */

static double fixed_hidden_neuron(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    /*
     * Q2.30 products are accumulated in 64 bits
     * to avoid overflow during the 784-element dot product.
     */
    int64_t accumulator = 0;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        /*
         * Convert input from floating point to Q1.15.
         */
        q15_t input_fixed =
            float_to_q15(input[i]);

        /*
         * Convert weight from floating point to Q1.15.
         */
        q15_t weight_fixed =
            float_to_q15(weights[i][neuron]);

        /*
         * Q1.15 x Q1.15 = Q2.30
         *
         * Explicitly cast to int32_t before multiplication.
         */
        int32_t product =
            (int32_t)input_fixed *
            (int32_t)weight_fixed;

        /*
         * Accumulate the Q2.30 product.
         */
        accumulator += product;
    }

    /*
     * Convert Q2.30 accumulator back to floating point.
     *
     * 2^30 = 1073741824
     */
    return (double)accumulator / (double)(1LL << 30);
}

/*
 * ============================================================
 * Main
 * ============================================================
 */

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Fixed-Point Hidden Layer Verification\n");
    printf("============================================================\n\n");

    printf("Network hidden layer: 784 -> 2\n");
    printf("Format: Q1.15 input / Q1.15 weights\n");
    printf("Accumulator: int64_t\n");
    printf("Images tested: %d\n\n", NUM_IMAGES);

    /*
     * ========================================================
     * Load hidden-layer weights
     * ========================================================
     */

    nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS];

    if (load_hidden_weights(
            HIDDEN_WEIGHT_FILE,
            hidden_weights) != STATUS_OK)
    {
        printf("ERROR: Could not load hidden weights.\n");
        return 1;
    }

    /*
     * ========================================================
     * Open complete MNIST input dataset
     * ========================================================
     */

    FILE *fp = fopen(INPUT_FILE, "r");

    if (fp == NULL)
    {
        printf("ERROR: Could not open input file:\n");
        printf("%s\n", INPUT_FILE);
        return 1;
    }

    /*
     * ========================================================
     * Error statistics
     * ========================================================
     */

    double max_error_h0 = 0.0;
    double max_error_h1 = 0.0;

    double sum_error_h0 = 0.0;
    double sum_error_h1 = 0.0;

    double max_relative_h0 = 0.0;
    double max_relative_h1 = 0.0;

    int h0_failures = 0;
    int h1_failures = 0;

    /*
     * ========================================================
     * Input buffer
     * ========================================================
     */

    nn_data_t input[INPUT_SIZE];

    /*
     * ========================================================
     * Process every image
     * ========================================================
     */

    for (int image = 0;
         image < NUM_IMAGES;
         image++)
    {
        /*
         * ----------------------------------------------------
         * Load one complete 784-element image
         * ----------------------------------------------------
         */

        for (int i = 0;
             i < INPUT_SIZE;
             i++)
        {
            if (fscanf(
                    fp,
                    "%lf",
                    &input[i]) != 1)
            {
                printf(
                    "ERROR: Could not read image %d, "
                    "pixel %d.\n",
                    image + 1,
                    i);

                fclose(fp);
                return 1;
            }
        }

        /*
         * ----------------------------------------------------
         * Floating-point golden hidden layer
         * ----------------------------------------------------
         *
         * This is the reference calculation:
         *
         *     H0 = sum(input[i] * weight[i][0])
         *     H1 = sum(input[i] * weight[i][1])
         *
         * No quantization is performed here.
         * ----------------------------------------------------
         */

        double golden_h0 = 0.0;
        double golden_h1 = 0.0;

        for (int i = 0;
             i < INPUT_SIZE;
             i++)
        {
            golden_h0 +=
                input[i] *
                hidden_weights[i][0];

            golden_h1 +=
                input[i] *
                hidden_weights[i][1];
        }

        /*
         * ----------------------------------------------------
         * Fixed-point hidden layer
         * ----------------------------------------------------
         *
         * The same dot products are now calculated using
         * Q1.15 inputs and Q1.15 weights.
         * ----------------------------------------------------
         */

        double fixed_h0 =
            fixed_hidden_neuron(
                input,
                hidden_weights,
                0);

        double fixed_h1 =
            fixed_hidden_neuron(
                input,
                hidden_weights,
                1);

        /*
         * ----------------------------------------------------
         * Absolute errors
         * ----------------------------------------------------
         */

        double error_h0 =
            fabs(fixed_h0 - golden_h0);

        double error_h1 =
            fabs(fixed_h1 - golden_h1);

        /*
         * ----------------------------------------------------
         * Relative errors
         * ----------------------------------------------------
         *
         * Relative error:
         *
         *     |fixed - golden| / |golden|
         *
         * If the golden value is extremely close to zero,
         * relative error becomes numerically unstable.
         *
         * Therefore, values below 1e-12 are treated as zero.
         * ----------------------------------------------------
         */

        double relative_h0 = 0.0;
        double relative_h1 = 0.0;

        if (fabs(golden_h0) > 1e-12)
        {
            relative_h0 =
                error_h0 / fabs(golden_h0);
        }

        if (fabs(golden_h1) > 1e-12)
        {
            relative_h1 =
                error_h1 / fabs(golden_h1);
        }

        /*
         * ----------------------------------------------------
         * Update error statistics
         * ----------------------------------------------------
         */

        sum_error_h0 += error_h0;
        sum_error_h1 += error_h1;

        if (error_h0 > max_error_h0)
        {
            max_error_h0 = error_h0;
        }

        if (error_h1 > max_error_h1)
        {
            max_error_h1 = error_h1;
        }

        if (relative_h0 > max_relative_h0)
        {
            max_relative_h0 = relative_h0;
        }

        if (relative_h1 > max_relative_h1)
        {
            max_relative_h1 = relative_h1;
        }

        /*
         * ----------------------------------------------------
         * Tolerance checks
         * ----------------------------------------------------
         */

        if (error_h0 > TOLERANCE)
        {
            h0_failures++;
        }

        if (error_h1 > TOLERANCE)
        {
            h1_failures++;
        }

        /*
         * ----------------------------------------------------
         * Display first image
         * ----------------------------------------------------
         */

        if (image == 0)
        {
            printf("--------------------------------------------\n");
            printf(" First image verification\n");
            printf("--------------------------------------------\n\n");

            printf(
                "Hidden neuron 0:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n"
                "  Relative error = %.12e\n\n",
                golden_h0,
                fixed_h0,
                error_h0,
                relative_h0);

            printf(
                "Hidden neuron 1:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n"
                "  Relative error = %.12e\n\n",
                golden_h1,
                fixed_h1,
                error_h1,
                relative_h1);
        }

        /*
         * ----------------------------------------------------
         * Progress indicator
         * ----------------------------------------------------
         */

        if ((image + 1) % 500 == 0)
        {
            printf(
                "Verified %d / %d images\n",
                image + 1,
                NUM_IMAGES);
        }
    }

    /*
     * ========================================================
     * Close input file
     * ========================================================
     */

    fclose(fp);

    /*
     * ========================================================
     * Calculate average absolute errors
     * ========================================================
     */

    double average_error_h0 =
        sum_error_h0 / NUM_IMAGES;

    double average_error_h1 =
        sum_error_h1 / NUM_IMAGES;

    /*
     * ========================================================
     * Final report
     * ========================================================
     */

    printf("\n");
    printf("============================================================\n");
    printf(" Fixed-Point Hidden Layer Results\n");
    printf("============================================================\n\n");

    printf(
        "Images tested             = %d\n",
        NUM_IMAGES);

    printf(
        "Tolerance                 = %.1e\n\n",
        TOLERANCE);

    printf(
        "H0 maximum absolute error = %.12e\n",
        max_error_h0);

    printf(
        "H1 maximum absolute error = %.12e\n\n",
        max_error_h1);

    printf(
        "H0 average absolute error = %.12e\n",
        average_error_h0);

    printf(
        "H1 average absolute error = %.12e\n\n",
        average_error_h1);

    printf(
        "H0 maximum relative error = %.12e\n",
        max_relative_h0);

    printf(
        "H1 maximum relative error = %.12e\n\n",
        max_relative_h1);

    printf(
        "H0 failures               = %d\n",
        h0_failures);

    printf(
        "H1 failures               = %d\n\n",
        h1_failures);

    /*
     * ========================================================
     * Overall verification result
     * ========================================================
     */

    if (h0_failures == 0 &&
        h1_failures == 0)
    {
        printf("--------------------------------------------\n");
        printf(" Fixed-point hidden layer: PASS\n");
        printf("--------------------------------------------\n");
    }
    else
    {
        printf("--------------------------------------------\n");
        printf(" Fixed-point hidden layer: FAIL\n");
        printf("--------------------------------------------\n");
    }

    printf("\n");

    /*
     * Return non-zero when verification fails.
     *
     * This is useful for automated regression testing.
     */

    return (h0_failures == 0 &&
            h1_failures == 0)
               ? 0
               : 1;
}