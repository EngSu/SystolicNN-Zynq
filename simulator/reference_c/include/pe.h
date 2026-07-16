#ifndef PE_H
#define PE_H

typedef struct
{
    float weight;
    float input;

    float partial_sum;

} PE;

void pe_reset(PE *pe);

void pe_compute(
    PE *pe,
    float input,
    float weight);

#endif