#include <stdio.h>

#include "fixed_point.h"

int main(void)
{
    double values[] =
    {
        -1.0,
        -0.786523160,
        -0.5,
        -0.133627060,
         0.0,
         0.151644510,
         0.5,
         0.816102600,
         0.992621099,
         1.0
    };

    int count =
        sizeof(values) / sizeof(values[0]);

    printf("\n");
    printf("============================================\n");
    printf(" Q1.15 Quantization Test\n");
    printf("============================================\n\n");

    for(int i = 0; i < count; i++)
    {
        q15_t fixed =
            float_to_q15(values[i]);

        double reconstructed =
            q15_to_float(fixed);

        double error =
            reconstructed - values[i];

        printf(
            "float = %+.12f  "
            "fixed = %6d  "
            "reconstructed = %+.12f  "
            "error = %+.12e\n",
            values[i],
            fixed,
            reconstructed,
            error
        );
    }

    return 0;
}
