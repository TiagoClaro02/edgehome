#pragma once
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "state/system_state.h"

extern SemaphoreHandle_t g_stateMutex;

void initStateManager();