#ifndef SIGMOID_LUT_H
#define SIGMOID_LUT_H

#include "fixed_point.h"

/*
 * ------------------------------------------------------------
 * Sigmoid LUT configuration
 * ------------------------------------------------------------
 *
 * Input:
 *     Q4.12
 *
 * Output:
 *     Q1.15
 *
 * Sigmoid range:
 *     [-6, +6]
 *
 * We exploit:
 *
 *     sigmoid(-x) = 1 - sigmoid(x)
 *
 * Therefore only the positive half of the sigmoid
 * needs to be stored.
 *
 * 128 entries cover:
 *
 *     [0, +6]
 *
 * Negative inputs use the symmetry relationship.
 * ------------------------------------------------------------
 */

#define SIGMOID_LUT_SIZE 32

#define SIGMOID_MIN_Q412 (-24576)
#define SIGMOID_MAX_Q412 ( 24576)

/*
 * Q4.12 representation of zero.
 */
#define SIGMOID_ZERO_Q412 0

/*
 * Q1.15 representation of 0.5.
 */
#define SIGMOID_HALF_Q15 16384

/*
 * Q4.12 -> Q1.15 sigmoid
 */
q15_t sigmoid_lut(q4_12_t x);

#endif
