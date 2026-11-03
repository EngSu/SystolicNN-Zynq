#include "fixed_inference.h"
#include <stddef.h>

#define SIGMOID_LIMIT_Q412 24576
#define LUT_SIZE 32

static const q15_t sigmoid_table[LUT_SIZE] = {
    16384, 17965, 19516, 21011, 22427, 23746, 24955, 26048,
    27023, 27883, 28634, 29285, 29843, 30319, 30723, 31064,
    31351, 31591, 31792, 31960, 32099, 32215, 32311, 32390,
    32456, 32511, 32556, 32593, 32623, 32649, 32670, 32687
};

/* Signed rounding to nearest, ties-to-even without floating-point.
 * Division in C99+ truncates toward zero, so negative rounding is symmetric. */
q4_12_t q30_to_q412_rne(int64_t acc_q30) {
    const int64_t denom = INT64_C(1) << 18;
    int64_t q = acc_q30 / denom;
    int64_t r = acc_q30 % denom;
    int64_t abs_r = (r < 0) ? -r : r; /* |r| <= denom - 1 */
    if ((abs_r * 2 > denom) ||
        ((abs_r * 2 == denom) && (q % 2 != 0))) {
        q += (acc_q30 < 0) ? -1 : 1;
    }
    if (q > 32767) q = 32767;
    if (q < -32768) q = -32768;
    return (q4_12_t)q;
}

/* Match the user-supplied original C LUT: saturate to sampled sigma(+-6). */
q15_t sigmoid_lut32_q15(q4_12_t x_q412) {
    int32_t x = (int32_t)x_q412;
    if (x <= -SIGMOID_LIMIT_Q412) return (q15_t)(32768 - sigmoid_table[LUT_SIZE - 1]);
    if (x >= SIGMOID_LIMIT_Q412) return sigmoid_table[LUT_SIZE - 1];
    if (x == 0) return (q15_t)16384;

    uint32_t mag = (uint32_t)((x < 0) ? -x : x);
    uint32_t numerator = mag * (LUT_SIZE - 1U);
    uint32_t idx = numerator / SIGMOID_LIMIT_Q412;
    uint32_t remainder = numerator % SIGMOID_LIMIT_Q412;
    if ((2U * remainder > SIGMOID_LIMIT_Q412) ||
        ((2U * remainder == SIGMOID_LIMIT_Q412) && (idx % 2U != 0))) {
        ++idx;
    }
    if (idx >= LUT_SIZE) idx = LUT_SIZE - 1U;
    int32_t y = (int32_t)sigmoid_table[idx];
    if (x < 0) y = 32768 - y;
    if (y > 32767) y = 32767;
    if (y < 0) y = 0;
    return (q15_t)y;
}

int fixed_inference_q15(
    const q15_t input_q15[FFNN_INPUTS],
    q15_t hidden_weights_q15[FFNN_INPUTS][FFNN_HIDDEN],
    q15_t output_weights_q15[FFNN_HIDDEN][FFNN_OUTPUTS],
    fixed_result_t *result) {

    if (!input_q15 || !hidden_weights_q15 || !output_weights_q15 || !result)
        return -1;

    for (int j = 0; j < FFNN_HIDDEN; ++j) {
        int64_t acc = 0;
        for (int i = 0; i < FFNN_INPUTS; ++i) {
            int32_t prod = (int32_t)input_q15[i] *
                           (int32_t)hidden_weights_q15[i][j];
            acc += (int64_t)prod;
        }
        result->hidden_sum_q30[j] = acc;
        result->hidden_pre_q412[j] = q30_to_q412_rne(acc);
        result->hidden_activation_q15[j] =
            sigmoid_lut32_q15(result->hidden_pre_q412[j]);
    }
    for (int k = 0; k < FFNN_OUTPUTS; ++k) {
        int64_t acc = 0;
        for (int j = 0; j < FFNN_HIDDEN; ++j) {
            int32_t prod = (int32_t)result->hidden_activation_q15[j] *
                           (int32_t)output_weights_q15[j][k];
            acc += (int64_t)prod;
        }
        result->output_score_q30[k] = acc;
    }
    /* Provisional tie rule: class 0 wins exact ties. Verify against golden. */
    result->prediction = (result->output_score_q30[0] >=
                          result->output_score_q30[1]) ? 0 : 1;
    return 0;
}
