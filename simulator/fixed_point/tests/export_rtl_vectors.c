
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <fenv.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "fixed_point.h"
#include "fixed_inference.h"

#define NUM_IMAGES 2115

static FILE *open_output(const char *dir, const char *name)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dir, name);

    FILE *f = fopen(path, "w");
    if (!f) {
        perror(path);
        exit(1);
    }
    return f;
}

static void write_rom(const char *dir, const char *name,
                      const q15_t *data, int count)
{
    FILE *f = open_output(dir, name);

    for (int i = 0; i < count; i++)
        fprintf(f, "%04x\n", (unsigned)(uint16_t)data[i]);

    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s OUTPUT_DIR\n", argv[0]);
        return 1;
    }

    if (fesetround(FE_TONEAREST) != 0) {
        fprintf(stderr, "Cannot set rounding mode\n");
        return 1;
    }

    nn_data_t hidden[HIDDEN_WEIGHT_ROWS][HIDDEN_WEIGHT_COLS];
    nn_data_t output[OUTPUT_WEIGHT_ROWS][OUTPUT_WEIGHT_COLS];
    nn_data_t input[INPUT_SIZE];

    q15_t hw[FFNN_INPUTS][FFNN_HIDDEN];
    q15_t ow[FFNN_HIDDEN][FFNN_OUTPUTS];
    q15_t input_q15[FFNN_INPUTS];

    if (load_hidden_weights(HIDDEN_WEIGHT_FILE, hidden) != STATUS_OK ||
        load_output_weights(OUTPUT_WEIGHT_FILE, output) != STATUS_OK) {
        fprintf(stderr, "Cannot load weights\n");
        return 1;
    }

    for (int i = 0; i < FFNN_INPUTS; i++)
        for (int j = 0; j < FFNN_HIDDEN; j++)
            hw[i][j] = float_to_q15(hidden[i][j]);

    for (int j = 0; j < FFNN_HIDDEN; j++)
        for (int k = 0; k < FFNN_OUTPUTS; k++)
            ow[j][k] = float_to_q15(output[j][k]);

    for (int j = 0; j < FFNN_HIDDEN; j++) {
        q15_t weights[FFNN_INPUTS];
        char name[64];

        for (int i = 0; i < FFNN_INPUTS; i++)
            weights[i] = hw[i][j];

        snprintf(name, sizeof(name), "hidden_w%d_q15.hex", j);
        write_rom(argv[1], name, weights, FFNN_INPUTS);
    }

    for (int k = 0; k < FFNN_OUTPUTS; k++) {
        q15_t weights[FFNN_HIDDEN];
        char name[64];

        for (int j = 0; j < FFNN_HIDDEN; j++)
            weights[j] = ow[j][k];

        snprintf(name, sizeof(name), "output_w%d_q15.hex", k);
        write_rom(argv[1], name, weights, FFNN_HIDDEN);
    }

    FILE *fin = fopen(INPUT_FILE, "r");
    FILE *csv = open_output(
        argv[1], "expected_inference_all_2115.csv");

    if (!fin) {
        perror(INPUT_FILE);
        fclose(csv);
        return 1;
    }

    fprintf(csv,
        "image,hidden_sum0_q30,hidden_sum1_q30,"
        "hidden_pre0_q412,hidden_pre1_q412,"
        "sigmoid0_q15,sigmoid1_q15,"
        "score0_q30,score1_q30,prediction\n");

    unsigned long saturation_count = 0;
    unsigned long exact_one = 0;

    for (int img = 1; img <= NUM_IMAGES; img++) {

        for (int i = 0; i < FFNN_INPUTS; i++) {

            if (fscanf(fin, "%lf", &input[i]) != 1) {
                fprintf(stderr, "Missing pixel at image %d\n", img);
                return 1;
            }

            if (input[i] > 32767.0 / 32768.0 ||
                input[i] < -1.0)
                saturation_count++;

            if (input[i] == 1.0)
                exact_one++;

            input_q15[i] = float_to_q15(input[i]);
        }

        fixed_result_t result;

        if (fixed_inference_q15(input_q15, hw, ow, &result)) {
            fprintf(stderr, "Inference failed: image %d\n", img);
            return 1;
        }

        fprintf(csv,
            "%d,%" PRId64 ",%" PRId64 ",%d,%d,%d,%d,"
            "%" PRId64 ",%" PRId64 ",%d\n",
            img,
            result.hidden_sum_q30[0],
            result.hidden_sum_q30[1],
            (int)result.hidden_pre_q412[0],
            (int)result.hidden_pre_q412[1],
            (int)result.hidden_activation_q15[0],
            (int)result.hidden_activation_q15[1],
            result.output_score_q30[0],
            result.output_score_q30[1],
            result.prediction);

        if (img <= 4 || img == 925) {
            char name[64];

            snprintf(name, sizeof(name),
                     "image_%04d_q15.hex", img);

            write_rom(argv[1], name, input_q15, FFNN_INPUTS);
        }
    }

    fclose(fin);
    fclose(csv);

    printf("Exported %d images\n", NUM_IMAGES);
    printf("Input Q1.15 saturations: %lu\n", saturation_count);
    printf("Inputs equal to 1.0: %lu\n", exact_one);
    printf("PASS: RTL reference vectors exported\n");

    return 0;
}

