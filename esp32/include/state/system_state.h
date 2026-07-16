#pragma once
#include <stdint.h>

typedef struct
{
    int temperature = 0; // x100 e.g. 2350 = 23.50°C
    int pressure    = 0; // Pa
} SystemState;

extern SystemState g_systemState;