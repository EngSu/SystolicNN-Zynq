#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"
#include "fixed_point.h"

#define NUM_IMAGES 2115

#define Q412_FRAC_BITS 12
#define Q412_SCALE     (1 << Q412_FRAC_BITS)

#define Q412_MAX       32767
#define Q412_MIN      -32768


/*
 * ============================================================
 * Q4.12 conversion
 * ============================================================
 */

static int16_t float_to_q412(double x)
{
    double scaled = x * Q412_SCALE;

    if (scaled > Q412_MAX)
        scaled = Q412_MAX;

    if (scaled < Q412_MIN)
        scaled = Q412_MIN;

    return (int16_t)lrint(scaled);
}


static double q412_to_float(int16_t x)
{
    return (double)x / Q412_SCALE;
}


/*
 * ============================================================
 * Fixed-point hidden pre-activation
 *
 * Input:
 *     Q1.15
 *
 * Weight:
 *     Q1.15
 *
 * Product:
 *     Q2.30
 *
 * Accumulator:
 *     int64_t
 *
 * Result:
 *     Q4.12
 * ============================================================
 */

static double fixed_hidden_q412(
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
    double pre_float =
        (double)accumulator / 1073741824.0;

    /*
     * Quantize hidden pre-activation to Q4.12.
     */
    int16_t pre_fixed =
        float_to_q412(pre_float);

    /*
     * Reconstruct Q4.12 value.
     */
    return q412_to_float(pre_fixed);
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
    printf(" Q4.12 End-to-End Fixed-Point Inference\n");
    printf("============================================================\n\n");

    printf("Network                  : 784 -> 2 -> 2\n");
    printf("Input / weights          : Q1.15\n");
    printf("Hidden pre-activation    : Q4.12\n");
    printf("Accumulator              : int64_t\n");
    printf("Images tested            : %d\n\n", NUM_IMAGES);


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

    double max_hidden_error[HIDDEN_NODES] = {
        0.0,
        0.0
    };

    double max_sigmoid_error[HIDDEN_NODES] = {
        0.0,
        0.0
    };

    double max_output_error[OUTPUT_NODES] = {
        0.0,
        0.0
    };

    double max_probability_error[OUTPUT_NODES] = {
        0.0,
        0.0
    };


    double sum_probability_error[OUTPUT_NODES] = {
        0.0,
        0.0
    };


    int prediction_mismatches = 0;


    /*
     * --------------------------------------------------------
     * Input buffer
     * --------------------------------------------------------
     */

    nn_data_t input[INPUT_SIZE];

    nn_result_t golden;


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
         * Read one image
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
         * Floating-point golden model
         * ----------------------------------------------------
         */

        nn_inference(
            input,
            hidden_weights,
            output_weights,
            &golden);


        /*
         * ----------------------------------------------------
         * Q4.12 hidden pre-activation
         * ----------------------------------------------------
         */

        double fixed_hidden[HIDDEN_NODES];

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            fixed_hidden[neuron] =
                fixed_hidden_q412(
                    input,
                    hidden_weights,
                    neuron);
        }


        /*
         * ----------------------------------------------------
         * Sigmoid
         *
         * For this experiment we use the same floating-point
         * sigmoid function as the golden model.
         *
         * This isolates the effect of Q4.12 hidden
         * pre-activation quantization.
         * ----------------------------------------------------
         */

        double fixed_hidden_post[HIDDEN_NODES];

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            fixed_hidden_post[neuron] =
                1.0 /
                (1.0 + exp(-fixed_hidden[neuron]));
        }


        /*
         * ----------------------------------------------------
         * Compare hidden pre-activation
         * ----------------------------------------------------
         */

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            double error =
                fabs(
                    fixed_hidden[neuron] -
                    golden.hidden_pre[neuron]);

            if (error > max_hidden_error[neuron])
                max_hidden_error[neuron] = error;


            /*
             * Compare sigmoid output.
             */

            double sigmoid_error =
                fabs(
                    fixed_hidden_post[neuron] -
                    golden.hidden_post[neuron]);

            if (sigmoid_error >
                max_sigmoid_error[neuron])
            {
                max_sigmoid_error[neuron] =
                    sigmoid_error;
            }
        }


        /*
         * ----------------------------------------------------
         * Fixed-point output layer
         *
         * Quantize hidden sigmoid output to Q1.15.
         * ----------------------------------------------------
         */

        q15_t hidden_post_fixed[HIDDEN_NODES];

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            hidden_post_fixed[neuron] =
                float_to_q15(
                    fixed_hidden_post[neuron]);
        }


        /*
         * ----------------------------------------------------
         * Output scores
         *
         * Q1.15 hidden activation
         * ×
         * Q1.15 output weights
         *
         * Product = Q2.30
         *
         * Accumulator = int64_t
         * ----------------------------------------------------
         */

        double fixed_output[OUTPUT_NODES];

        for (int output = 0;
             output < OUTPUT_NODES;
             output++)
        {
            int64_t accumulator = 0;

            for (int neuron = 0;
                 neuron < HIDDEN_NODES;
                 neuron++)
            {
                q15_t weight_fixed =
                    float_to_q15(
                        output_weights[neuron][output]);

                int32_t product =
                    (int32_t)hidden_post_fixed[neuron] *
                    (int32_t)weight_fixed;

                accumulator += product;
            }

            fixed_output[output] =
                (double)accumulator /
                1073741824.0;
        }


        /*
         * ----------------------------------------------------
         * Softmax
         * ----------------------------------------------------
         */

        double max_score =
            fixed_output[0];

        for (int output = 1;
             output < OUTPUT_NODES;
             output++)
        {
            if (fixed_output[output] >
                max_score)
            {
                max_score =
                    fixed_output[output];
            }
        }


        double exp_sum = 0.0;

        double fixed_probability[OUTPUT_NODES];

        for (int output = 0;
             output < OUTPUT_NODES;
             output++)
        {
            fixed_probability[output] =
                exp(
                    fixed_output[output] -
                    max_score);

            exp_sum +=
                fixed_probability[output];
        }


        for (int output = 0;
             output < OUTPUT_NODES;
             output++)
        {
            fixed_probability[output] /=
                exp_sum;
        }


        /*
         * ----------------------------------------------------
         * Compare output scores
         * ----------------------------------------------------
         */

        for (int output = 0;
             output < OUTPUT_NODES;
             output++)
        {
            double error =
                fabs(
                    fixed_output[output] -
                    golden.output_score[output]);

            if (error >
                max_output_error[output])
            {
                max_output_error[output] =
                    error;
            }


            /*
             * Probability error.
             */

            double probability_error =
                fabs(
                    fixed_probability[output] -
                    golden.probability[output]);

            if (probability_error >
                max_probability_error[output])
            {
                max_probability_error[output] =
                    probability_error;
            }

            sum_probability_error[output] +=
                probability_error;
        }


        /*
         * ----------------------------------------------------
         * Prediction
         * ----------------------------------------------------
         */

        int fixed_prediction = 0;

        if (fixed_probability[1] >
            fixed_probability[0])
        {
            fixed_prediction = 1;
        }


        if (fixed_prediction !=
            golden.prediction)
        {
            prediction_mismatches++;
        }


        /*
         * ----------------------------------------------------
         * First image report
         * ----------------------------------------------------
         */

        if (image == 0)
        {
            printf("--------------------------------------------\n");
            printf(" First image verification\n");
            printf("--------------------------------------------\n\n");


            for (int neuron = 0;
                 neuron < HIDDEN_NODES;
                 neuron++)
            {
                printf(
                    "Hidden pre-activation %d:\n"
                    "  Floating-point = %.12f\n"
                    "  Q4.12          = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    neuron,
                    golden.hidden_pre[neuron],
                    fixed_hidden[neuron],
                    fabs(
                        fixed_hidden[neuron] -
                        golden.hidden_pre[neuron]));
            }


            for (int neuron = 0;
                 neuron < HIDDEN_NODES;
                 neuron++)
            {
                printf(
                    "Hidden sigmoid %d:\n"
                    "  Floating-point = %.12f\n"
                    "  Q4.12 path     = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    neuron,
                    golden.hidden_post[neuron],
                    fixed_hidden_post[neuron],
                    fabs(
                        fixed_hidden_post[neuron] -
                        golden.hidden_post[neuron]));
            }


            for (int output = 0;
                 output < OUTPUT_NODES;
                 output++)
            {
                printf(
                    "Output score %d:\n"
                    "  Floating-point = %.12f\n"
                    "  Q4.12 path     = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    output,
                    golden.output_score[output],
                    fixed_output[output],
                    fabs(
                        fixed_output[output] -
                        golden.output_score[output]));
            }


            for (int output = 0;
                 output < OUTPUT_NODES;
                 output++)
            {
                printf(
                    "Probability %d:\n"
                    "  Floating-point = %.12f\n"
                    "  Q4.12 path     = %.12f\n"
                    "  Absolute error = %.12e\n\n",
                    output,
                    golden.probability[output],
                    fixed_probability[output],
                    fabs(
                        fixed_probability[output] -
                        golden.probability[output]));
            }


            printf(
                "Prediction:\n"
                "  Floating-point = %d\n"
                "  Q4.12 path     = %d\n\n",
                golden.prediction,
                fixed_prediction);
        }


        /*
         * ----------------------------------------------------
         * Progress
         * ----------------------------------------------------
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
     * Final statistics
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================================\n");
    printf(" Q4.12 End-to-End Results\n");
    printf("============================================================\n\n");

    printf(
        "Images tested = %d\n\n",
        NUM_IMAGES);


    printf(
        "Maximum hidden error H0 = %.12e\n",
        max_hidden_error[0]);

    printf(
        "Maximum hidden error H1 = %.12e\n\n",
        max_hidden_error[1]);


    printf(
        "Maximum sigmoid error H0 = %.12e\n",
        max_sigmoid_error[0]);

    printf(
        "Maximum sigmoid error H1 = %.12e\n\n",
        max_sigmoid_error[1]);


    printf(
        "Maximum output error O0 = %.12e\n",
        max_output_error[0]);

    printf(
        "Maximum output error O1 = %.12e\n\n",
        max_output_error[1]);


    printf(
        "Maximum probability error P0 = %.12e\n",
        max_probability_error[0]);

    printf(
        "Maximum probability error P1 = %.12e\n\n",
        max_probability_error[1]);


    printf(
        "Average probability error P0 = %.12e\n",
        sum_probability_error[0] /
        NUM_IMAGES);

    printf(
        "Average probability error P1 = %.12e\n\n",
        sum_probability_error[1] /
        NUM_IMAGES);


    printf(
        "Prediction mismatches = %d\n",
        prediction_mismatches);

    printf(
        "Prediction matches    = %d\n",
        NUM_IMAGES - prediction_mismatches);


    printf("\n");

    if (prediction_mismatches == 0)
    {
        printf(
            "--------------------------------------------\n");

        printf(
            " Q4.12 end-to-end inference: PASS\n");

        printf(
            "--------------------------------------------\n");
    }
    else
    {
        printf(
            "--------------------------------------------\n");

        printf(
            " Q4.12 end-to-end inference: REVIEW\n");

        printf(
            "--------------------------------------------\n");
    }

    printf("\n");

    return 0;
}
