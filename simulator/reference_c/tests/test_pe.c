#include <stdio.h>

#include "pe.h"

int main()
{

    ProcessingElement pe;

    pe_reset(&pe);

    pe_load_input(
        &pe,
        2.0);

    pe_load_weight(
        &pe,
        3.0);

    pe.enable = true;

    pe_tick(&pe);

    printf(
        "Accumulator = %f\n",
        pe_get_output(&pe));

    return 0;
}