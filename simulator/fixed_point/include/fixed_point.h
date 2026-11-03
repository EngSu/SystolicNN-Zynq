#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include <math.h>

/*
 * Q1.15
 *
 * signed 16-bit
 * 1 sign/integer bit
 * 15 fractional bits
 */

#define Q15_FRAC_BITS 15
#define Q15_SCALE     (1 << Q15_FRAC_BITS)
#define Q412_FRAC_BITS 12
#define Q412_SCALE     (1 << Q412_FRAC_BITS)

typedef int16_t q15_t;
typedef int16_t q4_12_t;

/*
 * Floating-point -> Q1.15
 */
static inline q15_t float_to_q15(double x)
{
    double scaled;

    scaled = x * Q15_SCALE;

    if(scaled > 32767.0)
        scaled = 32767.0;

    if(scaled < -32768.0)
        scaled = -32768.0;

    return (q15_t)lrint(scaled);
}

static inline q4_12_t float_to_q412(double x)
{
    double scaled = x * Q412_SCALE;

    if (scaled > 32767.0)
        scaled = 32767.0;

    if (scaled < -32768.0)
        scaled = -32768.0;

    return (q4_12_t)lrint(scaled);
}

static inline double q412_to_float(q4_12_t x)
{
    return (double)x / Q412_SCALE;
}

/*
 * Q1.15 -> floating point
 */
static inline double q15_to_float(q15_t x)
{
    return (double)x / Q15_SCALE;
}

#endif
