/*
 * SystolicNN-Zynq
 *
 * Independent floating-point golden model
 *
 * Network:
 *
 *     784 inputs
 *        |
 *     784 x 2
 *        |
 *     2 hidden neurons
 *        |
 *     Sigmoid
 *        |
 *     2 output neurons
 *        |
 *     Softmax
 *        |
 *     Prediction
 *
 * This implementation intentionally remains independent from the
 * simulator implementation. It is used as the software reference
 * for Milestone 6: Golden-Model Verification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INPUT_FILE \
    "../data/mnist/images/inputs.txt"

#define EXPECTED_OUTPUT_FILE \
    "../data/mnist/reference_outputs/expected_outputs.txt"

#define HIDDEN_WEIGHT_FILE \
    "../data/weights/float/hidden_weights_float.txt"

#define OUTPUT_WEIGHT_FILE \
    "../data/weights/float/layer2_weights_float.txt"

#define INPUT_SIZE 784
#define HIDDEN_NODES 2
#define OUTPUT_NODES 2
#define TEST_IMAGES 2115

static int load_weights(
    const char *filename,
    double weights[INPUT_SIZE][HIDDEN_NODES])
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror(filename);
        return 0;
    }

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        for (int j = 0; j < HIDDEN_NODES; j++)
        {
            if (fscanf(file, "%lf", &weights[i][j]) != 1)
            {
                fprintf(stderr,
                        "Error reading hidden weights at [%d][%d]\n",
                        i, j);

                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}

static int load_output_weights(
    const char *filename,
    double weights[HIDDEN_NODES][OUTPUT_NODES])
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror(filename);
        return 0;
    }

    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        for (int j = 0; j < OUTPUT_NODES; j++)
        {
            if (fscanf(file, "%lf", &weights[i][j]) != 1)
            {
                fprintf(stderr,
                        "Error reading output weights at [%d][%d]\n",
                        i, j);

                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}

static int load_input(
    FILE *file,
    double input[INPUT_SIZE])
{
    for (int i = 0; i < INPUT_SIZE; i++)
    {
        if (fscanf(file, "%lf", &input[i]) != 1)
        {
            return 0;
        }
    }

    return 1;
}

static int load_expected_output(
    FILE *file,
    double probability[OUTPUT_NODES])
{
    for (int i = 0; i < OUTPUT_NODES; i++)
    {
        if (fscanf(file, "%lf", &probability[i]) != 1)
        {
            return 0;
        }
    }

    return 1;
}

static int predict(
    const double probability[OUTPUT_NODES])
{
    return probability[1] > probability[0] ? 1 : 0;
}

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Independent Floating-Point Golden Model\n");
    printf("============================================================\n\n");

    printf("Network: 784 -> 2 -> sigmoid -> 2 -> softmax\n");
    printf("Test images: %d\n\n", TEST_IMAGES);

    /*
     * ------------------------------------------------------------
     * Allocate model data
     * ------------------------------------------------------------
     */

    double hidden_weights
        [INPUT_SIZE][HIDDEN_NODES];

    double output_weights
        [HIDDEN_NODES][OUTPUT_NODES];

    double input[INPUT_SIZE];

    /*
     * ------------------------------------------------------------
     * Load weights
     * ------------------------------------------------------------
     */

    if (!load_weights(
            HIDDEN_WEIGHT_FILE,
            hidden_weights))
    {
        return 1;
    }

    if (!load_output_weights(
            OUTPUT_WEIGHT_FILE,
            output_weights))
    {
        return 1;
    }

    /*
     * ------------------------------------------------------------
     * Open input and expected-output files
     * ------------------------------------------------------------
     */

    FILE *input_file =
        fopen(INPUT_FILE, "r");

    if (input_file == NULL)
    {
        perror(INPUT_FILE);
        return 1;
    }

    FILE *expected_file =
        fopen(EXPECTED_OUTPUT_FILE, "r");

    if (expected_file == NULL)
    {
        perror(EXPECTED_OUTPUT_FILE);
        fclose(input_file);
        return 1;
    }

    /*
     * ------------------------------------------------------------
     * Process all images
     * ------------------------------------------------------------
     */

    clock_t start = clock();

    for (int image = 0;
         image < TEST_IMAGES;
         image++)
    {
        if (!load_input(input_file, input))
        {
            fprintf(stderr,
                    "Error reading image %d\n",
                    image);

            fclose(input_file);
            fclose(expected_file);

            return 1;
        }

        /*
         * --------------------------------------------------------
         * Hidden layer
         * --------------------------------------------------------
         */

        double hidden_pre[HIDDEN_NODES] = {0.0};

        for (int neuron = 0;
             neuron < HIDDEN_NODES;
             neuron++)
        {
            for (int i = 0;
                 i < INPUT_SIZE;
                 i++)
            {
                hidden_pre[neuron] +=
                    input[i] *
                    hidden_weights[i][neuron];
            }
        }

        /*
         * --------------------------------------------------------
         * Sigmoid
         * --------------------------------------------------------
         */

        double hidden_post[HIDDEN_NODES];

        for (int i = 0;
             i < HIDDEN_NODES;
             i++)
        {
            hidden_post[i] =
                1.0 /
                (1.0 + exp(-hidden_pre[i]));
        }

        /*
         * --------------------------------------------------------
         * Output layer
         * --------------------------------------------------------
         */

        double output_score[OUTPUT_NODES] = {0.0};

        for (int neuron = 0;
             neuron < OUTPUT_NODES;
             neuron++)
        {
            for (int i = 0;
                 i < HIDDEN_NODES;
                 i++)
            {
                output_score[neuron] +=
                    hidden_post[i] *
                    output_weights[i][neuron];
            }
        }

        /*
         * --------------------------------------------------------
         * Softmax
         * --------------------------------------------------------
         */

        double exp_output[OUTPUT_NODES];

        double sum_exp = 0.0;

        for (int i = 0;
             i < OUTPUT_NODES;
             i++)
        {
            exp_output[i] =
                exp(output_score[i]);

            sum_exp += exp_output[i];
        }

        double probability[OUTPUT_NODES];

        for (int i = 0;
             i < OUTPUT_NODES;
             i++)
        {
            probability[i] =
                exp_output[i] /
                sum_exp;
        }

        int prediction =
            predict(probability);

        /*
         * --------------------------------------------------------
         * Read expected output
         * --------------------------------------------------------
         */

        double expected[OUTPUT_NODES];

        if (!load_expected_output(
                expected_file,
                expected))
        {
            fprintf(stderr,
                    "Error reading expected output for image %d\n",
                    image);

            fclose(input_file);
            fclose(expected_file);

            return 1;
        }

        /*
         * --------------------------------------------------------
         * Display first image
         * --------------------------------------------------------
         */

        if (image == 0)
        {
            printf("Golden Model - Image 1\n");
            printf("----------------------------------------\n");

            printf(
                "Hidden pre activation:\n"
                "H0 = %.9f\n"
                "H1 = %.9f\n\n",
                hidden_pre[0],
                hidden_pre[1]);

            printf(
                "Hidden activation:\n"
                "H0 = %.9f\n"
                "H1 = %.9f\n\n",
                hidden_post[0],
                hidden_post[1]);

            printf(
                "Output score:\n"
                "Class0 = %.9f\n"
                "Class1 = %.9f\n\n",
                output_score[0],
                output_score[1]);

            printf(
                "Probability:\n"
                "P0 = %.9f\n"
                "P1 = %.9f\n\n",
                probability[0],
                probability[1]);

            printf(
                "Prediction = %d\n\n",
                prediction);
        }
    }

    clock_t end = clock();

    double elapsed =
        (double)(end - start) /
        CLOCKS_PER_SEC;

    fclose(input_file);
    fclose(expected_file);

    printf("============================================================\n");
    printf(
        "Golden-model execution time: %.6f sec\n",
        elapsed);
    printf("============================================================\n\n");

    return 0;
}