#include <stdio.h>
#include "../include/data_loader.h"

int load_inputs(
    const char *filename,
    float input[INPUT_SIZE])
{

    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        printf("Cannot open %s\n", filename);
        return -1;
    }

    for (int i = 0; i < INPUT_SIZE; i++)
    {
        fscanf(fp, "%f", &input[i]);
    }

    fclose(fp);

    return 0;
}

int load_hidden_weights(
    const char *filename,
    float weights[INPUT_SIZE][HIDDEN_SIZE])
{

    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        printf("Cannot open %s\n", filename);
        return -1;
    }

    for (int i = 0; i < INPUT_SIZE; i++)
    {

        for (int j = 0; j < HIDDEN_SIZE; j++)
        {
            fscanf(fp, "%f", &weights[i][j]);
        }
    }

    fclose(fp);

    return 0;
}