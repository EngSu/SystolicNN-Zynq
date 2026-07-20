#include <stdio.h>

#include "systolic_array.h"
#include "clock.h"

int main()
{

    printf("\n");
    printf("==============================\n");
    printf(" SystolicNN-Zynq Simulator\n");
    printf("==============================\n\n");

    /*
     * Create hardware blocks
     */

    SystolicArray array;

    Clock clock;

    /*
     * Reset hardware
     */

    systolic_array_reset(&array);

    clock_reset(&clock);

    /*
     * Example computation:
     *
     * PE0:
     * input 2
     * weight 3
     *
     * PE1:
     * input 4
     * weight 5
     *
     */

    systolic_array_load(
        &array,
        0,
        2.0,
        3.0);

    systolic_array_load(
        &array,
        1,
        4.0,
        5.0);

    /*
     * One clock cycle
     */

    systolic_array_tick(&array);

    clock_tick(&clock);

    /*
     * Read outputs
     */

    printf(
        "Cycle = %llu\n",
        (unsigned long long)
            clock_get_cycle(&clock));

    printf(
        "PE0 output = %f\n",
        systolic_array_get_output(
            &array,
            0));

    printf(
        "PE1 output = %f\n",
        systolic_array_get_output(
            &array,
            1));

    printf("\nSimulation complete\n");

    return 0;
}