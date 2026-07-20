#include "activation_buffer.h"

#include <string.h>

void activation_buffer_init(
    ActivationBuffer *buffer)
{

    memset(buffer,
           0,
           sizeof(ActivationBuffer));
}