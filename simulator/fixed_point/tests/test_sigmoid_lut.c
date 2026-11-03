#include <stdio.h>
#include <math.h>

#include "fixed_point.h"
#include "sigmoid_lut.h"

static double sigmoid_reference(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf(" Q4.12 -> Q1.15 Sigmoid LUT Test\n");
    printf("============================================================\n\n");

    printf("LUT entries       = %d\n", SIGMOID_LUT_SIZE);
    printf("Input format      = Q4.12\n");
    printf("Output format     = Q1.15\n");
    printf("Input range       = [-6.0, +6.0]\n\n");

    double max_error = 0.0;
    double sum_error = 0.0;

    const int NUM_TESTS = 10001;

    for (int i = 0; i < NUM_TESTS; i++)
    {
        double x =
            -6.0 +
            12.0 * i /
            (NUM_TESTS - 1);

        q4_12_t x_fixed =
            float_to_q412(x);

        q15_t y_fixed =
            sigmoid_lut(x_fixed);

        double y_lut =
            q15_to_float(y_fixed);

        double y_reference =
            sigmoid_reference(x);

        double error =
            fabs(y_lut - y_reference);

        sum_error += error;

        if (error > max_error)
            max_error = error;
    }

    double average_error =
        sum_error / NUM_TESTS;

    printf("--------------------------------------------\n");
    printf(" Sigmoid LUT results\n");
    printf("--------------------------------------------\n\n");

    printf(
        "Maximum absolute error = %.12e\n",
        max_error);

    printf(
        "Average absolute error = %.12e\n",
        average_error);

    printf("\n");

    /*
     * Test representative points.
     */

    double test_values[] =
    {
        -6.0,
        -5.0,
        -2.0,
        -1.0,
         0.0,
         1.0,
         2.0,
         5.0,
         6.0
    };

    int num_values =
        sizeof(test_values) /
        sizeof(test_values[0]);

    printf("--------------------------------------------\n");
    printf(" Representative values\n");
    printf("--------------------------------------------\n\n");

    for (int i = 0; i < num_values; i++)
    {
        double x = test_values[i];

        q4_12_t x_fixed =
            float_to_q412(x);

        q15_t y_fixed =
            sigmoid_lut(x_fixed);

        double y_lut =
            q15_to_float(y_fixed);

        double y_reference =
            sigmoid_reference(x);

        double error =
            y_lut - y_reference;

        printf(
            "x = %+8.4f  "
            "reference = %.12f  "
            "LUT = %.12f  "
            "error = %+.12e\n",
            x,
            y_reference,
            y_lut,
            error);
    }

    printf("\n");
    printf("============================================================\n");
    printf(" Sigmoid LUT test complete\n");
    printf("============================================================\n");

    return 0;
}
