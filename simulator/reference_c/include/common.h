#ifndef COMMON_H
#define COMMON_H

/*==============================================================
 *
 * SystolicNN-Zynq
 *
 * Global Project Definitions
 *
 *==============================================================*/

#include <stdint.h>
#include <stdbool.h>

/***************************************************************
 * Neural Network Topology
 ***************************************************************/

#define INPUT_SIZE 784
#define HIDDEN_NODES 2
#define OUTPUT_NODES 2

#define DATASET_SIZE 2115

/***************************************************************
 * Weight Dimensions
 ***************************************************************/

#define HIDDEN_WEIGHT_ROWS INPUT_SIZE
#define HIDDEN_WEIGHT_COLS HIDDEN_NODES

#define OUTPUT_WEIGHT_ROWS HIDDEN_NODES
#define OUTPUT_WEIGHT_COLS OUTPUT_NODES

/***************************************************************
 * File Locations
 ***************************************************************/

#define INPUT_FILE \
    "../../../data/inputs/inputs.txt"

#define HIDDEN_WEIGHT_FILE \
    "../../../data/weights/hidden_weights_float.txt"

#define OUTPUT_WEIGHT_FILE \
    "../../../data/weights/layer2_weights_float.txt"

#define EXPECTED_OUTPUT_FILE \
    "../../../data/expected/outputs.txt"

/***************************************************************
 * Floating Point Type
 *
 * Later this becomes fixed-point.
 ***************************************************************/

typedef double nn_data_t;

/***************************************************************
 * Return Status
 ***************************************************************/

typedef enum
{
    STATUS_OK = 0,

    STATUS_ERROR,

    STATUS_FILE_ERROR,

    STATUS_MEMORY_ERROR

} status_t;

/***************************************************************
 * Controller States
 ***************************************************************/

typedef enum
{
    RESET_STATE = 0,

    LOAD_INPUT_STATE,

    LOAD_WEIGHTS_STATE,

    HIDDEN_MAC_STATE,

    SIGMOID_STATE,

    OUTPUT_MAC_STATE,

    SOFTMAX_STATE,

    PREDICTION_STATE,

    DONE_STATE

} accelerator_state_t;

#endif