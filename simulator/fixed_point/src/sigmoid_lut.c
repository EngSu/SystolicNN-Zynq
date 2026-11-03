#include <math.h>

#include "sigmoid_lut.h"

/*
 * ------------------------------------------------------------
 * Positive-half sigmoid LUT
 * ------------------------------------------------------------
 *
 * LUT covers:
 *
 *     0 <= x <= 6
 *
 * The negative half is obtained using:
 *
 *     sigmoid(-x) = 1 - sigmoid(x)
 *
 * Output is Q1.15.
 * ------------------------------------------------------------
 */

static q15_t sigmoid_table[SIGMOID_LUT_SIZE];

static int table_initialized = 0;


/*
 * ------------------------------------------------------------
 * Initialize LUT
 * ------------------------------------------------------------
 */

static void sigmoid_lut_init(void)
{
    if (table_initialized)
        return;

    for (int i = 0;
         i < SIGMOID_LUT_SIZE;
         i++)
    {
        /*
         * Generate uniformly spaced points:
         *
         *     x = 0 ... 6
         */
        double x =
            6.0 *
            i /
            (SIGMOID_LUT_SIZE - 1);

        /*
         * Mathematical sigmoid.
         */
        double y =
            1.0 /
            (1.0 + exp(-x));

        /*
         * Store as Q1.15.
         */
        sigmoid_table[i] =
            float_to_q15(y);
    }

    table_initialized = 1;
}


/*
 * ------------------------------------------------------------
 * Q4.12 -> Q1.15 sigmoid
 * ------------------------------------------------------------
 */

q15_t sigmoid_lut(q4_12_t x)
{
    sigmoid_lut_init();

    /*
     * --------------------------------------------------------
     * Saturation
     * --------------------------------------------------------
     */

    if (x <= SIGMOID_MIN_Q412)
    {
        /*
         * sigmoid(-6)
         */
            return (q15_t)(32768 - sigmoid_table[SIGMOID_LUT_SIZE - 1]);
    }

    if (x >= SIGMOID_MAX_Q412)
    {
        /*
         * sigmoid(+6)
         */
        return sigmoid_table[SIGMOID_LUT_SIZE - 1];
    }


    /*
     * --------------------------------------------------------
     * Exact zero
     * --------------------------------------------------------
     *
     * sigmoid(0) = 0.5 exactly.
     *
     * Q1.15:
     *
     *     0.5 * 32768 = 16384
     * --------------------------------------------------------
     */

    if (x == SIGMOID_ZERO_Q412)
    {
        return SIGMOID_HALF_Q15;
    }


    /*
     * --------------------------------------------------------
     * Determine sign.
     * --------------------------------------------------------
     */

    int negative = 0;

    q4_12_t magnitude = x;

    if (x < 0)
    {
        negative = 1;

        /*
         * Convert negative value to positive magnitude.
         */
        magnitude = (q4_12_t)(-x);
    }


    /*
     * --------------------------------------------------------
     * Convert Q4.12 magnitude to LUT index.
     * --------------------------------------------------------
     *
     * magnitude:
     *
     *     0 ... 24576
     *
     * LUT:
     *
     *     index 0 ... 127
     *
     * Mapping:
     *
     *     index =
     *         magnitude / 24576
     *         * 127
     * --------------------------------------------------------
     */

    double position =
        (double)magnitude /
        (double)SIGMOID_MAX_Q412;

    int index =
        (int)lrint(
            position *
            (SIGMOID_LUT_SIZE - 1));


    /*
     * Safety limits.
     */

    if (index < 0)
        index = 0;

    if (index >= SIGMOID_LUT_SIZE)
        index = SIGMOID_LUT_SIZE - 1;


    /*
     * --------------------------------------------------------
     * Retrieve positive sigmoid value.
     * --------------------------------------------------------
     */

    q15_t positive_value =
        sigmoid_table[index];


    /*
     * --------------------------------------------------------
     * Apply sigmoid symmetry.
     * --------------------------------------------------------
     *
     * sigmoid(-x) = 1 - sigmoid(x)
     *
     * In Q1.15:
     *
     *     1.0 = 32768
     *
     * Therefore:
     *
     *     negative result =
     *         32768 - positive_value
     *
     * --------------------------------------------------------
     */

    if (negative)
    {
        int32_t result =
            32768 -
            (int32_t)positive_value;

        /*
         * Q1.15 maximum is 32767.
         */
        if (result > 32767)
            result = 32767;

        if (result < -32768)
            result = -32768;

        return (q15_t)result;
    }


    return positive_value;
}
