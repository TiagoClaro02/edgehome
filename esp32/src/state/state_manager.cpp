#include "state/state_manager.h"

SemaphoreHandle_t g_stateMutex;

void initStateManager()
{
    g_stateMutex = xSemaphoreCreateMutex();
}