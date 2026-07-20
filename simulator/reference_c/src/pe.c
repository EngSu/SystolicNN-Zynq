#include "pe.h"

void pe_reset(
    ProcessingElement *pe)
{

    pe->input_reg = 0;

    pe->weight_reg = 0;

    pe->accumulator = 0;

    pe->output = 0;

    pe->enable = false;

    pe->valid = false;
}

void pe_load_input(
    ProcessingElement *pe,
    nn_data_t input)
{

    pe->input_reg = input;
}

void pe_load_weight(
    ProcessingElement *pe,
    nn_data_t weight)
{

    pe->weight_reg = weight;
}

void pe_tick(
    ProcessingElement *pe)
{

    if (pe->enable)
    {

        nn_data_t product;

        product =
            pe->input_reg *
            pe->weight_reg;

        pe->accumulator =
            pe->accumulator +
            product;

        pe->valid = true;
    }
}

nn_data_t pe_get_output(
    ProcessingElement *pe)
{

    return pe->accumulator;
}