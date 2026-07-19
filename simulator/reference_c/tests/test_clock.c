#include <stdio.h>

#include "clock.h"

int main()
{
    Clock clk;

    clock_reset(&clk);

    printf("Cycle = %llu\n",
           (unsigned long long)clock_get_cycle(&clk));

    for (int i = 0; i < 10; i++)
        clock_tick(&clk);

    printf("Cycle = %llu\n",
           (unsigned long long)clock_get_cycle(&clk));

    return 0;
}