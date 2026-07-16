#include "../include/pe.h"

void pe_reset(PE *pe)
{
    pe->weight = 0;
    pe->input = 0;
    pe->partial_sum = 0;
}

void pe_compute(
    PE *pe,
    float input,
    float weight)
{
    pe->input = input;

    pe->weight = weight;

    pe->partial_sum +=
        pe->input * pe->weight;
}