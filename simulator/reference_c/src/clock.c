#include "clock.h"

void clock_reset(Clock *clock)
{
    clock->cycle = 0;
}

void clock_tick(Clock *clock)
{
    clock->cycle++;
}

uint64_t clock_get_cycle(const Clock *clock)
{
    return clock->cycle;
}