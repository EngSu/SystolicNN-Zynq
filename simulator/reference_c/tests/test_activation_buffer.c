#include <stdio.h>

#include "activation_buffer.h"

int main(void)
{
    ActivationBuffer act;

    activation_buffer_init(&act);

    act.hidden_pre[0] = 12.5;
    act.hidden_activation[0] = 0.91;

    act.output_score[0] = 8.3;

    act.probability[0] = 0.99;

    printf("%f\n", act.hidden_pre[0]);
    printf("%f\n", act.hidden_activation[0]);
    printf("%f\n", act.output_score[0]);
    printf("%f\n", act.probability[0]);

    return 0;
}