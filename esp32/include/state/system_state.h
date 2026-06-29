#pragma once
#include <stdint.h>

typedef struct
{
    bool buttonPressed = false;
} SystemState;

extern SystemState g_systemState;

