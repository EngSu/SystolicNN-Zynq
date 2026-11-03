#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"
#include "fixed_point.h"
#include "sigmoid_lut.h"

#define NUM_IMAGES 2115

/*
 * ============================================================
 * Fixed-point hidden neuron
 * ============================================================
 *
 * Input:
 *     Q1.15
 *
 * Weight:
 *     Q1.15
 *
 * Product:
 *     32-bit product with 30 fractional bits
 *
 * Accumulator:
 *     int64_t
 *
 * Result:
 *     converted back to floating point for verification
 *
 * ============================================================
 */

static double fixed_hidden_neuron(
    const double input[],
    const double weights[][HIDDEN_WEIGHT_COLS],
    int neuron)
{
    int64_t accumulator = 0;

    for (int i = 0;
         i < INPUT_SIZE;
         i++)
    {
        q15_t input_fixed =
            float_to_q15(input[i]);

        q15_t weight_fixed =
            float_to_q15(
                weights[i][neuron]);

        int32_t product =
            (int32_t)input_fixed *
            (int32_t)weight_fixed;

        accumulator += product;
    }

    /*
     * Q1.15 x Q1.15 produces 30 fractional bits.
     *
     * Divide by 2^30 to reconstruct the
     * accumulated floating-point value.
     */
    return (double)accumulator /
           (double)(1LL << 30);
}

/*
 * ============================================================
 * Fixed-point sigmoid using LUT
 * ============================================================
 *
 * Hidden pre-activation:
 *     converted to Q4.12
 *
 * Sigmoid LUT:
 *     Q4.12 -> Q1.15
 *
 * Returned as double only so it can be compared against
 * the verified floating-point golden model.
 *
 * ============================================================
 */

static double fixed_sigmoid_lut(
    double hidden_pre)
{
    q4_12_t hidden_q412 =
        float_to_q412(hidden_pre);

    q15_t sigmoid_q15 =
        sigmoid_lut(hidden_q412);

    return q15_to_float(sigmoid_q15);
}

/*
 * ============================================================
 * Fixed-point output neuron
 * ============================================================
 *
 * Hidden activation:
 *     Q1.15
 *
 * Output weight:
 *     Q1.15
 *
 * Product:
 *     32-bit product with 30 fractional bits
 *
 * Accumulator:
 *     int64_t
 *
 * ============================================================
 */

static double fixed_output_neuron(
    const double hidden[],
    const double weights[][OUTPUT_WEIGHT_COLS],
    int neuron)
{
    int64_t accumulator = 0;

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {
        q15_t hidden_fixed =
            float_to_q15(hidden[i]);

        q15_t weight_fixed =
            float_to_q15(
                weights[i][neuron]);

        int32_t product =
            (int32_t)hidden_fixed *
            (int32_t)weight_fixed;

        accumulator += product;
    }

    return (double)accumulator /
           (double)(1LL << 30);
}

/*
 * ============================================================
 * Floating-point softmax
 * ============================================================
 *
 * Softmax remains floating point in this experiment.
 *
 * This is intentional:
 *
 *     the current experiment isolates the numerical effect
 *     of the fixed-point datapath and sigmoid LUT.
 *
 * A hardware-oriented softmax implementation can be
 * evaluated separately later.
 *
 * ============================================================
 */

static void softmax_float(
    const double scores[],
    double probabilities[])
{
    double max_score = scores[0];

    for (int i = 1;
         i < OUTPUT_NODES;
         i++)
    {
        if (scores[i] > max_score)
        {
            max_score = scores[i];
        }
    }

    double sum = 0.0;

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        probabilities[i] =
            exp(scores[i] - max_score);

        sum += probabilities[i];
    }

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        probabilities[i] /= sum;
    }
}

/*
 * ============================================================
 * Prediction
 * ============================================================
 */

static int predict_float(
    const double probabilities[])
{
    int prediction = 0;

    for (int i = 1;
         i < OUTPUT_NODES;
         i++)
    {
        if (probabilities[i] >
            probabilities[prediction])
        {
            prediction = i;
        }
    }

    return prediction;
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
    printf(" Sigmoid LUT End-to-End Verification\n");
    printf("============================================================\n\n");

    printf(
        "Network                  : 784 -> 2 -> 2\n");

    printf(
        "Input / weights          : Q1.15\n");

    printf(
        "Hidden pre-activation    : Q4.12\n");

    printf(
        "Sigmoid LUT              : %d entries\n",
        SIGMOID_LUT_SIZE);

    printf(
        "Sigmoid output           : Q1.15\n");

    printf(
        "Accumulator              : int64_t\n");

    printf(
        "Images tested            : %d\n\n",
        NUM_IMAGES);

    /*
     * ========================================================
     * Load weights
     * ========================================================
     */

    nn_data_t hidden_weights
        [HIDDEN_WEIGHT_ROWS]
        [HIDDEN_WEIGHT_COLS];

    nn_data_t output_weights
        [OUTPUT_WEIGHT_ROWS]
        [OUTPUT_WEIGHT_COLS];

    if (load_hidden_weights(
            HIDDEN_WEIGHT_FILE,
            hidden_weights) != STATUS_OK)
    {
        printf(
            "ERROR: Could not load hidden weights.\n");

        return 1;
    }

    if (load_output_weights(
            OUTPUT_WEIGHT_FILE,
            output_weights) != STATUS_OK)
    {
        printf(
            "ERROR: Could not load output weights.\n");

        return 1;
    }

    /*
     * ========================================================
     * Open complete input dataset
     * ========================================================
     */

    FILE *fp =
        fopen(INPUT_FILE, "r");

    if (fp == NULL)
    {
        printf(
            "ERROR: Could not open input file:\n");

        printf(
            "%s\n",
            INPUT_FILE);

        return 1;
    }

    /*
     * ========================================================
     * Statistics
     * ========================================================
     */

    double max_hidden_error
        [HIDDEN_NODES] = {0.0};

    double max_sigmoid_error
        [HIDDEN_NODES] = {0.0};

    double max_output_error
        [OUTPUT_NODES] = {0.0};

    double max_probability_error
        [OUTPUT_NODES] = {0.0};

    double sum_probability_error
        [OUTPUT_NODES] = {0.0};

    int prediction_mismatches = 0;
    int prediction_matches = 0;

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
         * Load one complete image
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
         * ====================================================
         * Floating-point golden model
         * ====================================================
         */

        nn_result_t golden;

        nn_inference(
            input,
            hidden_weights,
            output_weights,
            &golden);

        /*
         * ====================================================
         * Fixed-point hidden pre-activation
         * ====================================================
         */

        double fixed_hidden_pre
            [HIDDEN_NODES];

        for (int i = 0;
             i < HIDDEN_NODES;
             i++)
        {
            fixed_hidden_pre[i] =
                fixed_hidden_neuron(
                    input,
                    hidden_weights,
                    i);

            double error =
                fabs(
                    fixed_hidden_pre[i] -
                    golden.hidden_pre[i]);

            if (error >
                max_hidden_error[i])
            {
                max_hidden_error[i] =
                    error;
            }
        }

        /*
         * ====================================================
         * Sigmoid LUT
         * ====================================================
         */

        double fixed_hidden_post
            [HIDDEN_NODES];

        for (int i = 0;
             i < HIDDEN_NODES;
             i++)
        {
            fixed_hidden_post[i] =
                fixed_sigmoid_lut(
                    fixed_hidden_pre[i]);

            double error =
                fabs(
                    fixed_hidden_post[i] -
                    golden.hidden_post[i]);

            if (error >
                max_sigmoid_error[i])
            {
                max_sigmoid_error[i] =
                    error;
            }
        }

        /*
         * ====================================================
         * Fixed-point output layer
         * ====================================================
         */

        double fixed_output
            [OUTPUT_NODES];

        for (int i = 0;
             i < OUTPUT_NODES;
             i++)
        {
            fixed_output[i] =
                fixed_output_neuron(
                    fixed_hidden_post,
                    output_weights,
                    i);

            double error =
                fabs(
                    fixed_output[i] -
                    golden.output_score[i]);

            if (error >
                max_output_error[i])
            {
                max_output_error[i] =
                    error;
            }
        }

        /*
         * ====================================================
         * Softmax
         * ====================================================
         */

        double fixed_probability
            [OUTPUT_NODES];

        softmax_float(
            fixed_output,
            fixed_probability);

        /*
         * ====================================================
         * Probability error
         * ====================================================
         */

        for (int i = 0;
             i < OUTPUT_NODES;
             i++)
        {
            double error =
                fabs(
                    fixed_probability[i] -
                    golden.probability[i]);

            sum_probability_error[i] +=
                error;

            if (error >
                max_probability_error[i])
            {
                max_probability_error[i] =
                    error;
            }
        }

        /*
         * ====================================================
         * Prediction
         * ====================================================
         */

        int fixed_prediction =
            predict_float(
                fixed_probability);

        int golden_prediction =
            golden.prediction;

        if (fixed_prediction ==
            golden_prediction)
        {
            prediction_matches++;
        }
        else
        {
            prediction_mismatches++;

            /*
             * ------------------------------------------------
             * Detailed mismatch report
             * ------------------------------------------------
             */

            printf("\n");
            printf(
                "============================================================\n");

            printf(
                " LUT CLASSIFICATION MISMATCH\n");

            printf(
                "============================================================\n");

            /*
             * Array index is zero-based.
             *
             * image + 1 is the human-readable dataset sample.
             */

            printf(
                "Image index         = %d\n",
                image);

            printf(
                "Image number        = %d\n",
                image + 1);

            printf(
                "Floating prediction = %d\n",
                golden_prediction);

            printf(
                "LUT prediction      = %d\n",
                fixed_prediction);

            printf(
                "\nFloating-point hidden pre-activation:\n");

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "  H%d = %.12f\n",
                    i,
                    golden.hidden_pre[i]);
            }

            printf(
                "\nFixed-point hidden pre-activation:\n");

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "  H%d = %.12f\n",
                    i,
                    fixed_hidden_pre[i]);
            }

            printf(
                "\nFloating-point sigmoid:\n");

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "  H%d = %.12f\n",
                    i,
                    golden.hidden_post[i]);
            }

            printf(
                "\nLUT sigmoid:\n");

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "  H%d = %.12f\n",
                    i,
                    fixed_hidden_post[i]);
            }

            printf(
                "\nFloating-point output scores:\n");

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "  O%d = %.12f\n",
                    i,
                    golden.output_score[i]);
            }

            printf(
                "\nLUT output scores:\n");

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "  O%d = %.12f\n",
                    i,
                    fixed_output[i]);
            }

            printf(
                "\nFloating-point probabilities:\n");

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "  P%d = %.12f\n",
                    i,
                    golden.probability[i]);
            }

            printf(
                "\nLUT probabilities:\n");

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "  P%d = %.12f\n",
                    i,
                    fixed_probability[i]);
            }

            /*
             * Classification margins.
             *
             * For a two-class classifier this is useful for
             * determining whether the sample is close to the
             * decision boundary.
             */

            double golden_margin =
                fabs(
                    golden.probability[0] -
                    golden.probability[1]);

            double fixed_margin =
                fabs(
                    fixed_probability[0] -
                    fixed_probability[1]);

            printf(
                "\nClassification margin:\n");

            printf(
                "  Floating-point margin = %.12e\n",
                golden_margin);

            printf(
                "  LUT margin            = %.12e\n",
                fixed_margin);

            printf(
                "============================================================\n\n");
        }

        /*
         * ====================================================
         * First-image report
         * ====================================================
         */

        if (image == 0)
        {
            printf(
                "--------------------------------------------\n");

            printf(
                " First image verification\n");

            printf(
                "--------------------------------------------\n\n");

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "Hidden pre-activation %d:\n"
                    "  Floating-point = %.12f\n"
                    "  Fixed-point    = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    i,
                    golden.hidden_pre[i],
                    fixed_hidden_pre[i],
                    fabs(
                        fixed_hidden_pre[i] -
                        golden.hidden_pre[i]));
            }

            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                printf(
                    "Hidden sigmoid %d:\n"
                    "  Floating-point = %.12f\n"
                    "  LUT            = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    i,
                    golden.hidden_post[i],
                    fixed_hidden_post[i],
                    fabs(
                        fixed_hidden_post[i] -
                        golden.hidden_post[i]));
            }

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "Output score %d:\n"
                    "  Floating-point = %.12f\n"
                    "  LUT path       = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    i,
                    golden.output_score[i],
                    fixed_output[i],
                    fabs(
                        fixed_output[i] -
                        golden.output_score[i]));
            }

            for (int i = 0;
                 i < OUTPUT_NODES;
                 i++)
            {
                printf(
                    "Probability %d:\n"
                    "  Floating-point = %.12f\n"
                    "  LUT path       = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    i,
                    golden.probability[i],
                    fixed_probability[i],
                    fabs(
                        fixed_probability[i] -
                        golden.probability[i]));
            }

            printf(
                "Prediction:\n"
                "  Floating-point = %d\n"
                "  LUT path       = %d\n\n",
                golden_prediction,
                fixed_prediction);
        }

        /*
         * ====================================================
         * Progress
         * ====================================================
         */

        if ((image + 1) % 500 == 0)
        {
            printf(
                "Processed %d / %d images\n",
                image + 1,
                NUM_IMAGES);
        }
    }

    fclose(fp);

    /*
     * ========================================================
     * Average probability error
     * ========================================================
     */

    double average_probability_error
        [OUTPUT_NODES];

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        average_probability_error[i] =
            sum_probability_error[i] /
            NUM_IMAGES;
    }

    /*
     * ========================================================
     * Final report
     * ========================================================
     */

    printf("\n");
    printf(
        "============================================================\n");

    printf(
        " %d-Entry Sigmoid LUT Results\n",
        SIGMOID_LUT_SIZE);

    printf(
        "============================================================\n\n");

    printf(
        "Images tested = %d\n\n",
        NUM_IMAGES);

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {
        printf(
            "Maximum hidden error H%d   = %.12e\n",
            i,
            max_hidden_error[i]);
    }

    printf("\n");

    for (int i = 0;
         i < HIDDEN_NODES;
         i++)
    {
        printf(
            "Maximum sigmoid error H%d  = %.12e\n",
            i,
            max_sigmoid_error[i]);
    }

    printf("\n");

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        printf(
            "Maximum output error O%d   = %.12e\n",
            i,
            max_output_error[i]);
    }

    printf("\n");

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        printf(
            "Maximum probability error P%d = %.12e\n",
            i,
            max_probability_error[i]);
    }

    printf("\n");

    for (int i = 0;
         i < OUTPUT_NODES;
         i++)
    {
        printf(
            "Average probability error P%d = %.12e\n",
            i,
            average_probability_error[i]);
    }

    printf("\n");

    printf(
        "Prediction mismatches = %d\n",
        prediction_mismatches);

    printf(
        "Prediction matches    = %d\n\n",
        prediction_matches);

    /*
     * ========================================================
     * Overall result
     * ========================================================
     */

    printf(
        "--------------------------------------------\n");

    printf(
        " %d-entry sigmoid LUT inference: %s\n",
        SIGMOID_LUT_SIZE,
        prediction_mismatches == 0
            ? "PASS"
            : "FAIL");

    printf(
        "--------------------------------------------\n\n");

    return prediction_mismatches == 0
               ? 0
               : 1;
}