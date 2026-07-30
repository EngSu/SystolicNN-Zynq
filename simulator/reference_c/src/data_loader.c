#include <stdio.h>

#include "data_loader.h"



status_t load_input_image(

    const char *filename,

    nn_data_t input[INPUT_SIZE]

)
{

    FILE *file = fopen(filename,"r");


    if(file == NULL)
    {
        printf("Cannot open input file: %s\n",
                filename);

        return STATUS_FILE_ERROR;
    }



    for(int i=0;i<INPUT_SIZE;i++)
    {

        if(fscanf(file,"%lf",&input[i]) != 1)
        {
            fclose(file);

            return STATUS_ERROR;
        }

    }


    fclose(file);


    return STATUS_OK;
}





status_t load_hidden_weights(

    const char *filename,

    nn_data_t weights
    [HIDDEN_WEIGHT_ROWS]
    [HIDDEN_WEIGHT_COLS]

)
{

    FILE *file=fopen(filename,"r");


    if(file==NULL)
    {
        printf("Cannot open hidden weights\n");

        return STATUS_FILE_ERROR;
    }



    for(int i=0;i<HIDDEN_WEIGHT_ROWS;i++)
    {

        for(int j=0;j<HIDDEN_WEIGHT_COLS;j++)
        {

            fscanf(
                file,
                "%lf",
                &weights[i][j]
            );

        }

    }


    fclose(file);


    return STATUS_OK;

}





status_t load_output_weights(

    const char *filename,

    nn_data_t weights
    [OUTPUT_WEIGHT_ROWS]
    [OUTPUT_WEIGHT_COLS]

)
{

    FILE *file=fopen(filename,"r");


    if(file==NULL)
    {
        printf("Cannot open output weights\n");

        return STATUS_FILE_ERROR;
    }



    for(int i=0;i<OUTPUT_WEIGHT_ROWS;i++)
    {

        for(int j=0;j<OUTPUT_WEIGHT_COLS;j++)
        {

            fscanf(
                file,
                "%lf",
                &weights[i][j]
            );

        }

    }


    fclose(file);


    return STATUS_OK;

}





status_t load_expected_prediction(

    const char *filename,

    int *prediction

)
{

    FILE *file=fopen(filename,"r");


    if(file==NULL)
    {
        return STATUS_FILE_ERROR;
    }



    fscanf(
        file,
        "%d",
        prediction
    );



    fclose(file);


    return STATUS_OK;

}
