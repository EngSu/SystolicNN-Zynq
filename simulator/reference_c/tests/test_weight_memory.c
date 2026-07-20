#include <stdio.h>

#include "weight_memory.h"

int main(void)
{
    WeightMemory mem;

    weight_memory_init(&mem);

    mem.hidden_weights[0][0] = 1.5;
    mem.hidden_weights[10][1] = 2.7;

    printf("%f\n",
           get_hidden_weight(&mem, 0, 0));

    printf("%f\n",
           get_hidden_weight(&mem, 10, 1));

    return 0;
}