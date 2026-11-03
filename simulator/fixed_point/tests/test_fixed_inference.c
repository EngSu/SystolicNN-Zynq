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

/*
 * Q1.15 fixed-point hidden neuron.
 *
 * Q1.15 x Q1.15 -> Q2.30
 * Accumulation is performed using int64_t.
 *
 * The final accumulator is converted back to floating point
 * only for comparison with the floating-point reference.
 */
static double fixed_hidden_neuron(
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
     * Q2.30 -> floating point.
     */
    return (double)accumulator / (double)(1LL << 30);
}


/*
 * Q1.15 fixed-point output neuron.
 *
 * Hidden activations are quantized to Q1.15 before
 * multiplication with the output weights.
 */
static double fixed_output_neuron(
    const double hidden[],
    const double weights[][OUTPUT_WEIGHT_COLS],
    int neuron)
{
    int64_t accumulator = 0;

    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        q15_t hidden_fixed =
            float_to_q15(hidden[i]);

        q15_t weight_fixed =
            float_to_q15(weights[i][neuron]);

        int32_t product =
            (int32_t)hidden_fixed *
            (int32_t)weight_fixed;

        accumulator += product;
    }

    /*
     * Q2.30 -> floating point.
     */
    return (double)accumulator / (double)(1LL << 30);
}


/*
 * Return the index of the largest probability.
 */
static int argmax(
    const double values[],
    int count)
{
    int index = 0;

    for (int i = 1; i < count; i++)
    {
        if (values[i] > values[index])
            index = i;
    }

    return index;
}


int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Complete Q1.15 Fixed-Point Inference Verification\n");
    printf("============================================================\n\n");

    printf("Network: 784 -> 2 -> 2\n");
    printf("Format: Q1.15\n");
    printf("Accumulator: int64_t\n");
    printf("Images tested: %d\n\n", NUM_IMAGES);


    /*
     * --------------------------------------------------------
     * Load weights
     * --------------------------------------------------------
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
        printf("ERROR: Could not load hidden weights.\n");
        return 1;
    }


    if (load_output_weights(
            OUTPUT_WEIGHT_FILE,
            output_weights) != STATUS_OK)
    {
        printf("ERROR: Could not load output weights.\n");
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
     * Statistics
     * --------------------------------------------------------
     */

    int prediction_mismatches = 0;

    double max_hidden_error_h0 = 0.0;
    double max_hidden_error_h1 = 0.0;

    double max_output_error_0 = 0.0;
    double max_output_error_1 = 0.0;

    double max_probability_error_0 = 0.0;
    double max_probability_error_1 = 0.0;

    double sum_probability_error_0 = 0.0;
    double sum_probability_error_1 = 0.0;


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
         * Load one complete image.
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
         * FLOATING-POINT REFERENCE
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
         * FIXED-POINT HIDDEN LAYER
         * ====================================================
         */

        double fixed_hidden[HIDDEN_NODES];

        for (int i = 0;
             i < HIDDEN_NODES;
             i++)
        {
            fixed_hidden[i] =
                fixed_hidden_neuron(
                    input,
                    hidden_weights,
                    i);
        }


        /*
         * ----------------------------------------------------
         * Hidden-layer error
         * ----------------------------------------------------
         */

        double hidden_error_0 =
            fabs(
                fixed_hidden[0] -
                golden.hidden_pre[0]);

        double hidden_error_1 =
            fabs(
                fixed_hidden[1] -
                golden.hidden_pre[1]);


        if (hidden_error_0 > max_hidden_error_h0)
            max_hidden_error_h0 = hidden_error_0;

        if (hidden_error_1 > max_hidden_error_h1)
            max_hidden_error_h1 = hidden_error_1;


        /*
         * ====================================================
         * FLOATING-POINT SIGMOID
         * ====================================================
         *
         * We intentionally use the floating-point sigmoid
         * here. This experiment isolates MAC quantization
         * error from activation-function approximation error.
         */

        double fixed_hidden_activation[HIDDEN_NODES];

        for (int i = 0;
             i < HIDDEN_NODES;
             i++)
        {
            fixed_hidden_activation[i] =
                1.0 /
                (1.0 +
                 exp(-fixed_hidden[i]));
        }


        /*
         * ====================================================
         * FIXED-POINT OUTPUT LAYER
         * ====================================================
         */

        double fixed_output[OUTPUT_NODES];

        for (int i = 0;
             i < OUTPUT_NODES;
             i++)
        {
            fixed_output[i] =
                fixed_output_neuron(
                    fixed_hidden_activation,
                    output_weights,
                    i);
        }


        /*
         * ----------------------------------------------------
         * Output-score error
         * ----------------------------------------------------
         */

        double output_error_0 =
            fabs(
                fixed_output[0] -
                golden.output_score[0]);

        double output_error_1 =
            fabs(
                fixed_output[1] -
                golden.output_score[1]);


        if (output_error_0 > max_output_error_0)
            max_output_error_0 = output_error_0;

        if (output_error_1 > max_output_error_1)
            max_output_error_1 = output_error_1;


        /*
         * ====================================================
         * SOFTMAX
         * ====================================================
         */

        double max_score = fixed_output[0];

        if (fixed_output[1] > max_score)
            max_score = fixed_output[1];


        double exp0 =
            exp(fixed_output[0] - max_score);

        double exp1 =
            exp(fixed_output[1] - max_score);

        double exp_sum =
            exp0 + exp1;


        double fixed_probability[OUTPUT_NODES];

        fixed_probability[0] =
            exp0 / exp_sum;

        fixed_probability[1] =
            exp1 / exp_sum;


        /*
         * ----------------------------------------------------
         * Probability error
         * ----------------------------------------------------
         */

        double probability_error_0 =
            fabs(
                fixed_probability[0] -
                golden.probability[0]);

        double probability_error_1 =
            fabs(
                fixed_probability[1] -
                golden.probability[1]);


        if (probability_error_0 >
            max_probability_error_0)
        {
            max_probability_error_0 =
                probability_error_0;
        }


        if (probability_error_1 >
            max_probability_error_1)
        {
            max_probability_error_1 =
                probability_error_1;
        }


        sum_probability_error_0 +=
            probability_error_0;

        sum_probability_error_1 +=
            probability_error_1;


        /*
         * ====================================================
         * PREDICTION COMPARISON
         * ====================================================
         */

        int golden_prediction =
            argmax(
                golden.probability,
                OUTPUT_NODES);

        int fixed_prediction =
            argmax(
                fixed_probability,
                OUTPUT_NODES);


        if (golden_prediction !=
            fixed_prediction)
        {
            prediction_mismatches++;
        }


        /*
         * ====================================================
         * FIRST IMAGE REPORT
         * ====================================================
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
                "  Absolute error = %.12e\n\n",
                golden.hidden_pre[0],
                fixed_hidden[0],
                hidden_error_0);


            printf(
                "Hidden neuron 1:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n\n",
                golden.hidden_pre[1],
                fixed_hidden[1],
                hidden_error_1);


            printf(
                "Output score 0:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n\n",
                golden.output_score[0],
                fixed_output[0],
                output_error_0);


            printf(
                "Output score 1:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n\n",
                golden.output_score[1],
                fixed_output[1],
                output_error_1);


            printf(
                "Probability 0:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n\n",
                golden.probability[0],
                fixed_probability[0],
                probability_error_0);


            printf(
                "Probability 1:\n"
                "  Floating-point = %.12f\n"
                "  Fixed-point    = %.12f\n"
                "  Absolute error = %.12e\n\n",
                golden.probability[1],
                fixed_probability[1],
                probability_error_1);


            printf(
                "Prediction:\n"
                "  Floating-point = %d\n"
                "  Fixed-point    = %d\n\n",
                golden_prediction,
                fixed_prediction);
        }


        /*
         * Progress indicator.
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
     * --------------------------------------------------------
     * Average probability errors
     * --------------------------------------------------------
     */

    double average_probability_error_0 =
        sum_probability_error_0 /
        NUM_IMAGES;

    double average_probability_error_1 =
        sum_probability_error_1 /
        NUM_IMAGES;


    /*
     * --------------------------------------------------------
     * Final results
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================================\n");
    printf(" Complete Q1.15 Fixed-Point Inference Results\n");
    printf("============================================================\n\n");


    printf(
        "Images tested                = %d\n\n",
        NUM_IMAGES);


    printf(
        "Maximum hidden error H0     = %.12e\n",
        max_hidden_error_h0);

    printf(
        "Maximum hidden error H1     = %.12e\n\n",
        max_hidden_error_h1);


    printf(
        "Maximum output error O0     = %.12e\n",
        max_output_error_0);

    printf(
        "Maximum output error O1     = %.12e\n\n",
        max_output_error_1);


    printf(
        "Maximum probability error P0 = %.12e\n",
        max_probability_error_0);

    printf(
        "Maximum probability error P1 = %.12e\n\n",
        max_probability_error_1);


    printf(
        "Average probability error P0 = %.12e\n",
        average_probability_error_0);

    printf(
        "Average probability error P1 = %.12e\n\n",
        average_probability_error_1);


    printf(
        "Prediction mismatches        = %d\n",
        prediction_mismatches);

    printf(
        "Prediction matches           = %d\n\n",
        NUM_IMAGES -
        prediction_mismatches);


    /*
     * --------------------------------------------------------
     * Overall interpretation
     * --------------------------------------------------------
     */

    printf("--------------------------------------------\n");

    if (prediction_mismatches == 0)
    {
        printf(
            " Q1.15 prediction verification: PASS\n");
    }
    else
    {
        printf(
            " Q1.15 prediction verification: FAIL\n");
    }

    printf("--------------------------------------------\n\n");


    return 0;
}
