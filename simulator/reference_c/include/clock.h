#ifndef CLOCK_H
#define CLOCK_H

#include "common.h"

typedef struct
{
    uint64_t cycle;

} Clock;

void clock_reset(Clock *clock);

void clock_tick(Clock *clock);

uint64_t clock_get_cycle(const Clock *clock);

#endif