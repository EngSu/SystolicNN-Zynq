#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"
#include "fixed_point.h"

#define NUM_IMAGES 2115

/*
 * ------------------------------------------------------------
 * Floating-point MAC
 * ------------------------------------------------------------
 */

static double floating_mac(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    double accumulator = 0.0;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        accumulator +=
            input[i] * weights[i][neuron];
    }

    return accumulator;
}

/*
 * ------------------------------------------------------------
 * Quantized-input only
 *
 * Input  -> Q1.15
 * Weight -> floating point
 *
 * This isolates the effect of input quantization.
 * ------------------------------------------------------------
 */

static double quantized_input_mac(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    double accumulator = 0.0;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        q15_t input_fixed =
            float_to_q15(input[i]);

        double input_quantized =
            q15_to_float(input_fixed);

        accumulator +=
            input_quantized *
            weights[i][neuron];
    }

    return accumulator;
}

/*
 * ------------------------------------------------------------
 * Quantized-weight only
 *
 * Input  -> floating point
 * Weight -> Q1.15
 *
 * This isolates the effect of weight quantization.
 * ------------------------------------------------------------
 */

static double quantized_weight_mac(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    double accumulator = 0.0;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        q15_t weight_fixed =
            float_to_q15(weights[i][neuron]);

        double weight_quantized =
            q15_to_float(weight_fixed);

        accumulator +=
            input[i] *
            weight_quantized;
    }

    return accumulator;
}

/*
 * ------------------------------------------------------------
 * Quantized-input + quantized-weight
 *
 * Input  -> Q1.15
 * Weight -> Q1.15
 * Product -> Q2.30
 * Accumulator -> int64_t
 *
 * This represents the actual fixed-point MAC.
 * ------------------------------------------------------------
 */

static double quantized_both_mac(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    int64_t accumulator = 0;

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        q15_t input_fixed =
            float_to_q15(input[i]);

        q15_t weight_fixed =
            float_to_q15(weights[i][neuron]);

        int32_t product =
            (int32_t)input_fixed *
            (int32_t)weight_fixed;

        accumulator += product;
    }

    /*
     * Q2.30 -> floating point
     */
    return (double)accumulator / 1073741824.0;
}

/*
 * ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------
 */

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Q1.15 Quantization Error Source Analysis\n");
    printf("============================================================\n\n");

    printf("Network hidden layer: 784 -> 2\n");
    printf("Images tested: %d\n\n", NUM_IMAGES);

    /*
     * --------------------------------------------------------
     * Load hidden weights
     * --------------------------------------------------------
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
     * --------------------------------------------------------
     * Open input dataset
     * --------------------------------------------------------
     */

    FILE *fp = fopen(INPUT_FILE, "r");

    if (fp == NULL)
    {
        printf("ERROR: Could not open input file:\n");
        printf("%s\n", INPUT_FILE);
        return 1;
    }

    /*
     * --------------------------------------------------------
     * Error statistics
     *
     * Each neuron has four error measurements:
     *
     *   input quantization only
     *   weight quantization only
     *   both quantized
     *
     * --------------------------------------------------------
     */

    double max_input_h0 = 0.0;
    double max_input_h1 = 0.0;

    double max_weight_h0 = 0.0;
    double max_weight_h1 = 0.0;

    double max_both_h0 = 0.0;
    double max_both_h1 = 0.0;

    double sum_input_h0 = 0.0;
    double sum_input_h1 = 0.0;

    double sum_weight_h0 = 0.0;
    double sum_weight_h1 = 0.0;

    double sum_both_h0 = 0.0;
    double sum_both_h1 = 0.0;

    /*
     * --------------------------------------------------------
     * Input buffer
     * --------------------------------------------------------
     */

    nn_data_t input[INPUT_SIZE];

    /*
     * --------------------------------------------------------
     * Process all images
     * --------------------------------------------------------
     */

    for (int image = 0;
         image < NUM_IMAGES;
         image++)
    {
        /*
         * ----------------------------------------------------
         * Load one image
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
         * Neuron 0
         * ----------------------------------------------------
         */

        double golden_h0 =
            floating_mac(
                input,
                hidden_weights,
                0);

        double input_h0 =
            quantized_input_mac(
                input,
                hidden_weights,
                0);

        double weight_h0 =
            quantized_weight_mac(
                input,
                hidden_weights,
                0);

        double both_h0 =
            quantized_both_mac(
                input,
                hidden_weights,
                0);

        double error_input_h0 =
            fabs(input_h0 - golden_h0);

        double error_weight_h0 =
            fabs(weight_h0 - golden_h0);

        double error_both_h0 =
            fabs(both_h0 - golden_h0);

        /*
         * ----------------------------------------------------
         * Neuron 1
         * ----------------------------------------------------
         */

        double golden_h1 =
            floating_mac(
                input,
                hidden_weights,
                1);

        double input_h1 =
            quantized_input_mac(
                input,
                hidden_weights,
                1);

        double weight_h1 =
            quantized_weight_mac(
                input,
                hidden_weights,
                1);

        double both_h1 =
            quantized_both_mac(
                input,
                hidden_weights,
                1);

        double error_input_h1 =
            fabs(input_h1 - golden_h1);

        double error_weight_h1 =
            fabs(weight_h1 - golden_h1);

        double error_both_h1 =
            fabs(both_h1 - golden_h1);

        /*
         * ----------------------------------------------------
         * Accumulate statistics
         * ----------------------------------------------------
         */

        sum_input_h0 += error_input_h0;
        sum_input_h1 += error_input_h1;

        sum_weight_h0 += error_weight_h0;
        sum_weight_h1 += error_weight_h1;

        sum_both_h0 += error_both_h0;
        sum_both_h1 += error_both_h1;

        /*
         * Maximum errors
         */

        if (error_input_h0 > max_input_h0)
            max_input_h0 = error_input_h0;

        if (error_input_h1 > max_input_h1)
            max_input_h1 = error_input_h1;

        if (error_weight_h0 > max_weight_h0)
            max_weight_h0 = error_weight_h0;

        if (error_weight_h1 > max_weight_h1)
            max_weight_h1 = error_weight_h1;

        if (error_both_h0 > max_both_h0)
            max_both_h0 = error_both_h0;

        if (error_both_h1 > max_both_h1)
            max_both_h1 = error_both_h1;

        /*
         * ----------------------------------------------------
         * First-image detailed report
         * ----------------------------------------------------
         */

        if (image == 0)
        {
            printf("--------------------------------------------\n");
            printf(" First image error decomposition\n");
            printf("--------------------------------------------\n\n");

            printf("Hidden neuron 0:\n");
            printf(
                "  Floating-point           = %.12f\n",
                golden_h0);

            printf(
                "  Input quantized only     = %.12f\n",
                input_h0);

            printf(
                "  Weight quantized only    = %.12f\n",
                weight_h0);

            printf(
                "  Both quantized           = %.12f\n\n",
                both_h0);

            printf(
                "  Input quantization error = %.12e\n",
                error_input_h0);

            printf(
                "  Weight quantization err. = %.12e\n",
                error_weight_h0);

            printf(
                "  Combined Q1.15 error     = %.12e\n\n",
                error_both_h0);

            printf("Hidden neuron 1:\n");
            printf(
                "  Floating-point           = %.12f\n",
                golden_h1);

            printf(
                "  Input quantized only     = %.12f\n",
                input_h1);

            printf(
                "  Weight quantized only    = %.12f\n",
                weight_h1);

            printf(
                "  Both quantized           = %.12f\n\n",
                both_h1);

            printf(
                "  Input quantization error = %.12e\n",
                error_input_h1);

            printf(
                "  Weight quantization err. = %.12e\n",
                error_weight_h1);

            printf(
                "  Combined Q1.15 error     = %.12e\n\n",
                error_both_h1);
        }

        /*
         * ----------------------------------------------------
         * Progress
         * ----------------------------------------------------
         */

        if ((image + 1) % 500 == 0)
        {
            printf(
                "Analyzed %d / %d images\n",
                image + 1,
                NUM_IMAGES);
        }
    }

    fclose(fp);

    /*
     * --------------------------------------------------------
     * Average errors
     * --------------------------------------------------------
     */

    double avg_input_h0 =
        sum_input_h0 / NUM_IMAGES;

    double avg_input_h1 =
        sum_input_h1 / NUM_IMAGES;

    double avg_weight_h0 =
        sum_weight_h0 / NUM_IMAGES;

    double avg_weight_h1 =
        sum_weight_h1 / NUM_IMAGES;

    double avg_both_h0 =
        sum_both_h0 / NUM_IMAGES;

    double avg_both_h1 =
        sum_both_h1 / NUM_IMAGES;

    /*
     * --------------------------------------------------------
     * Final results
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================================\n");
    printf(" Q1.15 Quantization Error Results\n");
    printf("============================================================\n\n");

    printf("                         H0                 H1\n");
    printf("------------------------------------------------------------\n");

    printf(
        "Input quantization max   %.12e   %.12e\n",
        max_input_h0,
        max_input_h1);

    printf(
        "Input quantization avg   %.12e   %.12e\n\n",
        avg_input_h0,
        avg_input_h1);

    printf(
        "Weight quantization max  %.12e   %.12e\n",
        max_weight_h0,
        max_weight_h1);

    printf(
        "Weight quantization avg  %.12e   %.12e\n\n",
        avg_weight_h0,
        avg_weight_h1);

    printf(
        "Combined Q1.15 max       %.12e   %.12e\n",
        max_both_h0,
        max_both_h1);

    printf(
        "Combined Q1.15 avg       %.12e   %.12e\n",
        avg_both_h0,
        avg_both_h1);

    printf("\n");
    printf("============================================================\n");
    printf(" Analysis complete\n");
    printf("============================================================\n\n");

    return 0;
}
