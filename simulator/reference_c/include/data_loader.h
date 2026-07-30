#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "common.h"


/*
 * Load one MNIST image.
 *
 * File format:
 * 784 floating point values
 */

status_t load_input_image(

    const char *filename,

    nn_data_t input[INPUT_SIZE]

);



/*
 * Load hidden layer weights
 *
 * Dimension:
 *
 * 784 x 2
 *
 */

status_t load_hidden_weights(

    const char *filename,

    nn_data_t weights
    [HIDDEN_WEIGHT_ROWS]
    [HIDDEN_WEIGHT_COLS]

);



/*
 * Load output layer weights
 *
 * Dimension:
 *
 * 2 x 2
 *
 */

status_t load_output_weights(

    const char *filename,

    nn_data_t weights
    [OUTPUT_WEIGHT_ROWS]
    [OUTPUT_WEIGHT_COLS]

);



/*
 * Load expected prediction
 *
 * Used for verification.
 */

status_t load_expected_prediction(

    const char *filename,

    int *prediction

);


#endif
