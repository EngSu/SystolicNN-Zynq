/* SystolicNN-Zynq: compare new integer inference with the existing LUT path
 * and previously validated floating-point nn_inference() path on 2115 images.
 * This test is not executable in isolation: link it with the user's repository.
 */
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>

#include "common.h"
#include "paths.h"
#include "data_loader.h"
#include "nn_accelerator.h"
#include "fixed_point.h"
#include "sigmoid_lut.h"
#include "fixed_inference.h"

#define NUM_IMAGES 2115

#if SIGMOID_LUT_SIZE != 32
#error "Set SIGMOID_LUT_SIZE to 32 before this comparison."
#endif
#if INPUT_SIZE != FFNN_INPUTS || HIDDEN_NODES != FFNN_HIDDEN || OUTPUT_NODES != FFNN_OUTPUTS
#error "The existing network dimensions differ from the 784-2-2 candidate."
#endif

static int saturated_q15(double v) {
    return v > (32767.0 / 32768.0) || v < -1.0;
}

/* The pre-existing test's fixed path, reproduced with raw integer scores so
 * that there is no ambiguity from a floating-point softmax. */
static void existing_lut_integer_path(
    const q15_t input_q15[FFNN_INPUTS],
    q15_t hidden_q15[FFNN_INPUTS][FFNN_HIDDEN],
    q15_t output_q15[FFNN_HIDDEN][FFNN_OUTPUTS],
    fixed_result_t *r) {
    for (int j = 0; j < FFNN_HIDDEN; ++j) {
        int64_t sum = 0;
        for (int i = 0; i < FFNN_INPUTS; ++i)
            sum += (int32_t)input_q15[i] * (int32_t)hidden_q15[i][j];
        r->hidden_sum_q30[j] = sum;
        r->hidden_pre_q412[j] = float_to_q412((double)sum / 1073741824.0);
        r->hidden_activation_q15[j] = sigmoid_lut(r->hidden_pre_q412[j]);
    }
    for (int k = 0; k < FFNN_OUTPUTS; ++k) {
        int64_t sum = 0;
        for (int j = 0; j < FFNN_HIDDEN; ++j)
            sum += (int32_t)r->hidden_activation_q15[j] * (int32_t)output_q15[j][k];
        r->output_score_q30[k] = sum;
    }
    r->prediction = (r->output_score_q30[0] >= r->output_score_q30[1]) ? 0 : 1;
}

int main(void) {
    if (fesetround(FE_TONEAREST) != 0) {
        fprintf(stderr, "Unable to set FE_TONEAREST rounding mode\n");
        return 1;
    }
    nn_data_t hidden_weights[HIDDEN_WEIGHT_ROWS][HIDDEN_WEIGHT_COLS];
    nn_data_t output_weights[OUTPUT_WEIGHT_ROWS][OUTPUT_WEIGHT_COLS];
    q15_t hw_q15[FFNN_INPUTS][FFNN_HIDDEN];
    q15_t ow_q15[FFNN_HIDDEN][FFNN_OUTPUTS];
    q15_t input_q15[FFNN_INPUTS];
    nn_data_t input[INPUT_SIZE];

    if (load_hidden_weights(HIDDEN_WEIGHT_FILE, hidden_weights) != STATUS_OK ||
        load_output_weights(OUTPUT_WEIGHT_FILE, output_weights) != STATUS_OK) {
        fprintf(stderr, "Unable to load original trained weights\n");
        return 1;
    }
    unsigned long weight_sat = 0;
    for (int i = 0; i < FFNN_INPUTS; ++i)
        for (int j = 0; j < FFNN_HIDDEN; ++j) {
            double v = hidden_weights[i][j];
            weight_sat += saturated_q15(v);
            hw_q15[i][j] = float_to_q15(v);
        }
    for (int j = 0; j < FFNN_HIDDEN; ++j)
        for (int k = 0; k < FFNN_OUTPUTS; ++k) {
            double v = output_weights[j][k];
            weight_sat += saturated_q15(v);
            ow_q15[j][k] = float_to_q15(v);
        }
    FILE *fp = fopen(INPUT_FILE, "r");
    if (!fp) { perror(INPUT_FILE); return 1; }

    unsigned long input_sat = 0;
    unsigned model_mismatches = 0, floating_mismatches = 0, ties = 0;
    int64_t out_min[2] = {INT64_MAX, INT64_MAX};
    int64_t out_max[2] = {INT64_MIN, INT64_MIN};

    for (int image = 0; image < NUM_IMAGES; ++image) {
        for (int i = 0; i < INPUT_SIZE; ++i) {
            if (fscanf(fp, "%lf", &input[i]) != 1) {
                fprintf(stderr, "Missing input: image %d, pixel %d\n", image + 1, i);
                fclose(fp);
                return 1;
            }
            input_sat += saturated_q15(input[i]);
            input_q15[i] = float_to_q15(input[i]);
        }
        nn_result_t golden;
        nn_inference(input, hidden_weights, output_weights, &golden);
        int floating_prediction = (golden.probability[1] > golden.probability[0]) ? 1 : 0;

        fixed_result_t candidate, existing;
        if (fixed_inference_q15(input_q15, hw_q15, ow_q15, &candidate) != 0) {
            fprintf(stderr, "Integer core rejected image %d\n", image + 1);
            fclose(fp);
            return 1;
        }
        existing_lut_integer_path(input_q15, hw_q15, ow_q15, &existing);
        int differ = 0;
        for (int j = 0; j < FFNN_HIDDEN; ++j) {
            if (candidate.hidden_sum_q30[j] != existing.hidden_sum_q30[j] ||
                candidate.hidden_pre_q412[j] != existing.hidden_pre_q412[j] ||
                candidate.hidden_activation_q15[j] != existing.hidden_activation_q15[j])
                differ = 1;
        }
        for (int k = 0; k < FFNN_OUTPUTS; ++k) {
            if (candidate.output_score_q30[k] != existing.output_score_q30[k])
                differ = 1;
            if (candidate.output_score_q30[k] < out_min[k]) out_min[k] = candidate.output_score_q30[k];
            if (candidate.output_score_q30[k] > out_max[k]) out_max[k] = candidate.output_score_q30[k];
        }
        if (candidate.prediction != existing.prediction) differ = 1;
        if (candidate.output_score_q30[0] == candidate.output_score_q30[1]) ++ties;
        if (differ) {
            if (model_mismatches == 0) {
                printf("First integer-vs-existing mismatch: image %d\n", image + 1);
                for (int j=0; j<FFNN_HIDDEN; ++j)
                    printf(" hidden %d: new(sum=%" PRId64 ",pre=%d,lut=%d) old(sum=%" PRId64 ",pre=%d,lut=%d)\n",
                          j, candidate.hidden_sum_q30[j], (int)candidate.hidden_pre_q412[j],
                          (int)candidate.hidden_activation_q15[j], existing.hidden_sum_q30[j],
                          (int)existing.hidden_pre_q412[j], (int)existing.hidden_activation_q15[j]);
            }
            ++model_mismatches;
        }
        if (candidate.prediction != floating_prediction) {
            if (floating_mismatches == 0)
                printf("First floating-model mismatch: image %d, integer=%d, golden=%d, scores=(%" PRId64 ",%" PRId64 ")\n",
                       image + 1, candidate.prediction, floating_prediction,
                       candidate.output_score_q30[0], candidate.output_score_q30[1]);
            ++floating_mismatches;
        }
    }
    fclose(fp);
    puts("=== 784-2-2 INTEGER INFERENCE DATASET REPORT ===");
    printf("Images: %d\n", NUM_IMAGES);
    printf("Integer-vs-existing mismatches: %u\n", model_mismatches);
    printf("Integer-vs-floating nn_inference prediction mismatches: %u\n", floating_mismatches);
    printf("Exact score ties: %u\n", ties);
    printf("Input Q1.15 saturations: %lu\n", input_sat);
    printf("Weight Q1.15 saturations: %lu\n", weight_sat);
    for (int k=0; k<FFNN_OUTPUTS; ++k)
        printf("Output score %d Q30 range: [%" PRId64 ", %" PRId64 "]\n",
               k, out_min[k], out_max[k]);
    if (model_mismatches || floating_mismatches) {
        puts("FAIL: investigate mismatches; keep both models unchanged");
        return 1;
    }
    puts("PASS: integer candidate agrees with existing LUT path and the existing floating nn_inference predictions");
    return 0;
}
