#include <stdio.h>

#include "systolic_array.h"

int main()
{

    SystolicArray array;

    systolic_array_reset(&array);

    /*
     * Enable multiple accumulation cycles
     *
     * PE0:
     *
     * 2*3
     * 4*5
     * 1*10
     *
     * Expected:
     *
     * 6 + 20 + 10 = 36
     *
     */

    systolic_array_load(
        &array,
        0,
        2,
        3);

    systolic_array_tick(&array);

    systolic_array_load(
        &array,
        0,
        4,
        5);

    systolic_array_tick(&array);

    systolic_array_load(
        &array,
        0,
        1,
        10);

    systolic_array_tick(&array);

    printf(
        "PE0 final accumulator = %f\n",
        systolic_array_get_output(
            &array,
            0));

    return 0;
}