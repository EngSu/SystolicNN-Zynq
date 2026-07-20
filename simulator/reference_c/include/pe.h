#ifndef PE_H
#define PE_H

#include "common.h"

/*
 * Processing Element
 *
 * Hardware equivalent:
 *
 * pe.sv
 *
 */

typedef struct
{

    /*
     * Registers
     */

    nn_data_t input_reg;

    nn_data_t weight_reg;

    /*
     * Accumulator register
     */

    nn_data_t accumulator;

    /*
     * Output register
     */

    nn_data_t output;

    /*
     * Control signals
     */

    bool enable;

    bool valid;

} ProcessingElement;

/*
 * Initialize PE
 */

void pe_reset(
    ProcessingElement *pe);

/*
 * Load input register
 */

void pe_load_input(
    ProcessingElement *pe,
    nn_data_t input);

/*
 * Load weight register
 */

void pe_load_weight(
    ProcessingElement *pe,
    nn_data_t weight);

/*
 * Execute one clock cycle
 */

void pe_tick(
    ProcessingElement *pe);

/*
 * Read accumulated result
 */

nn_data_t pe_get_output(
    ProcessingElement *pe);

#endif