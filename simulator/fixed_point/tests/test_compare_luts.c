/* Check the frozen integer LUT against the existing exp()-initialized LUT.
 * Run only when SIGMOID_LUT_SIZE is set to 32 in sigmoid_lut.h. */
#include <stdint.h>
#include <stdio.h>
#include "fixed_inference.h"
#include "sigmoid_lut.h"

#if SIGMOID_LUT_SIZE != 32
#error "Set SIGMOID_LUT_SIZE to 32 in your existing include/sigmoid_lut.h first."
#endif

int main(void) {
    unsigned mismatches = 0;
    for (int raw = -32768; raw <= 32767; ++raw) {
        q4_12_t x = (q4_12_t)raw;
        q15_t old_y = sigmoid_lut(x);
        q15_t new_y = sigmoid_lut32_q15(x);
        if (old_y != new_y) {
            if (mismatches < 12U)
                printf("MISMATCH x=%d original=%d frozen=%d\n",
                       raw, (int)old_y, (int)new_y);
            ++mismatches;
        }
    }
    printf("Compared %u Q4.12 encodings; mismatches=%u\n", 65536U, mismatches);
    if (mismatches == 0) {
        puts("PASS: original C sigmoid LUT and frozen integer LUT match");
        return 0;
    }
    puts("FAIL: preserve both versions and investigate first mismatch");
    return 1;
}
