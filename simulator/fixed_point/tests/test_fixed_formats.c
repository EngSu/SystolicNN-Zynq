#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"

#define NUM_IMAGES 2115

/*
 * ------------------------------------------------------------
 * Fixed-point format definitions
 * ------------------------------------------------------------
 */

/*
 * Q4.12
 *
 * 16-bit signed value
 * 12 fractional bits
 *
 * Range:
 *   -8.0 <= x <= 7.999755859375
 *
 * Resolution:
 *   2^-12 = 0.000244140625
 */

#define Q412_FRAC_BITS 12
#define Q412_SCALE     (1 << Q412_FRAC_BITS)

#define Q412_MAX       32767
#define Q412_MIN      -32768

/*
 * ------------------------------------------------------------
 * Float -> Q4.12
 * ------------------------------------------------------------
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

/*
 * ------------------------------------------------------------
 * Q4.12 -> float
 * ------------------------------------------------------------
 */

static double q412_to_float(int16_t x)
{
    return (double)x / Q412_SCALE;
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
    printf(" Q4.12 Hidden Pre-Activation Format Analysis\n");
    printf("============================================================\n\n");

    printf("Images analyzed       : %d\n", NUM_IMAGES);
    printf("Format                : Q4.12\n");
    printf("Fractional bits       : %d\n", Q412_FRAC_BITS);
    printf("Resolution            : %.12f\n",
           1.0 / Q412_SCALE);
    printf("Representable range   : [-8.0, %.12f]\n\n",
           (double)Q412_MAX / Q412_SCALE);

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
     * Open dataset
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

    double min_pre[HIDDEN_NODES] = {
        INFINITY,
        INFINITY
    };

    double max_pre[HIDDEN_NODES] = {
        -INFINITY,
        -INFINITY
    };

    double max_quant_error[HIDDEN_NODES] = {
        0.0,
        0.0
    };

    double sum_quant_error[HIDDEN_NODES] = {
        0.0,
        0.0
    };

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
         * Read one image.
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
         * Calculate floating-point hidden pre-activation
         * ----------------------------------------------------
         */

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            double pre = 0.0;

            for (int i = 0;
                 i < INPUT_SIZE;
                 i++)
            {
                pre +=
                    input[i] *
                    hidden_weights[i][neuron];
            }

            /*
             * Update observed range.
             */

            if (pre < min_pre[neuron])
                min_pre[neuron] = pre;

            if (pre > max_pre[neuron])
                max_pre[neuron] = pre;

            /*
             * ------------------------------------------------
             * Quantize to Q4.12
             * ------------------------------------------------
             */

            int16_t fixed =
                float_to_q412(pre);

            double reconstructed =
                q412_to_float(fixed);

            /*
             * Quantization error.
             */

            double error =
                fabs(reconstructed - pre);

            sum_quant_error[neuron] += error;

            if (error > max_quant_error[neuron])
                max_quant_error[neuron] = error;
        }

        /*
         * Progress.
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
     * Final results
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================================\n");
    printf(" Q4.12 Format Results\n");
    printf("============================================================\n\n");

    for (int neuron = 0;
         neuron < HIDDEN_NODES;
         neuron++)
    {
        double average_error =
            sum_quant_error[neuron] /
            NUM_IMAGES;

        printf(
            "Hidden neuron %d:\n",
            neuron);

        printf(
            "  Observed minimum       = %.12f\n",
            min_pre[neuron]);

        printf(
            "  Observed maximum       = %.12f\n",
            max_pre[neuron]);

        printf(
            "  Maximum quant. error   = %.12e\n",
            max_quant_error[neuron]);

        printf(
            "  Average quant. error   = %.12e\n\n",
            average_error);
    }

    /*
     * --------------------------------------------------------
     * Example values
     * --------------------------------------------------------
     */

    printf("--------------------------------------------\n");
    printf(" Example Q4.12 conversions\n");
    printf("--------------------------------------------\n\n");

    double examples[] = {
        -5.636656175129,
        -1.586152687213,
         0.0,
         1.774077674874,
         4.901724334025
    };

    int num_examples =
        sizeof(examples) / sizeof(examples[0]);

    for (int i = 0;
         i < num_examples;
         i++)
    {
        int16_t fixed =
            float_to_q412(examples[i]);

        double reconstructed =
            q412_to_float(fixed);

        double error =
            reconstructed - examples[i];

        printf(
            "float = %+.12f  "
            "fixed = %6d  "
            "reconstructed = %+.12f  "
            "error = %+.12e\n",
            examples[i],
            fixed,
            reconstructed,
            error);
    }

    printf("\n");
    printf("============================================================\n");
    printf(" Q4.12 analysis complete\n");
    printf("============================================================\n");

    return 0;
}
