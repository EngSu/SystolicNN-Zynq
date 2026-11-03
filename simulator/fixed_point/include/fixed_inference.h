#ifndef FIXED_INFERENCE_H
#define FIXED_INFERENCE_H
#include <stdint.h>
#include "fixed_point.h"

#define FFNN_INPUTS 784
#define FFNN_HIDDEN 2
#define FFNN_OUTPUTS 2

/* All arrays are ALREADY quantized. Each hidden_weights[i][j] is the
 * weight from input i to hidden neuron j; output_weights[j][k] is
 * from hidden neuron j to output neuron k. This is a no-bias model. */
typedef struct {
    int64_t hidden_sum_q30[FFNN_HIDDEN];
    q4_12_t hidden_pre_q412[FFNN_HIDDEN];
    q15_t hidden_activation_q15[FFNN_HIDDEN];
    int64_t output_score_q30[FFNN_OUTPUTS];
    int prediction;
} fixed_result_t;

/* Integer-only, round-to-nearest-even: Q30 -> Q4.12 (18-bit drop). */
q4_12_t q30_to_q412_rne(int64_t acc_q30);

/* Frozen positive-half 32-entry sigmoid LUT. */
q15_t sigmoid_lut32_q15(q4_12_t x_q412);

/* No floating point in the arithmetic datapath. Return 0 for success. */
int fixed_inference_q15(
    const q15_t input_q15[FFNN_INPUTS],
    q15_t hidden_weights_q15[FFNN_INPUTS][FFNN_HIDDEN],
    q15_t output_weights_q15[FFNN_HIDDEN][FFNN_OUTPUTS],
    fixed_result_t *result);

#endif
