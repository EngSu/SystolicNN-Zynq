#ifndef PATHS_H
#define PATHS_H


/*
 * SystolicNN-Zynq
 *
 * Shared data file locations
 *
 * All simulator tests should use
 * these definitions.
 */


// MNIST input vectors

#define INPUT_FILE \
"../../data/mnist/images/inputs.txt"


// Expected neural network outputs

#define EXPECTED_OUTPUT_FILE \
"../../data/mnist/reference_outputs/expected_outputs.txt"


// Labels

#define LABEL_FILE \
"../../data/mnist/labels/labels.txt"


// Floating point weights

#define HIDDEN_WEIGHT_FILE \
"../../data/weights/float/hidden_weights_float.txt"


#define OUTPUT_WEIGHT_FILE \
"../../data/weights/float/layer2_weights_float.txt"


// Future fixed point files

#define HIDDEN_WEIGHT_FIXED_FILE \
"../../data/weights/fixed/hidden_weights_fixed.txt"


#define OUTPUT_WEIGHT_FIXED_FILE \
"../../data/weights/fixed/layer2_weights_fixed.txt"


#endif
