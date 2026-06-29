#include "state/state_manager.h"

SemaphoreHandle_t g_stateMutex;
SystemState g_systemState;

void initStateManager()
{
    g_stateMutex = xSemaphoreCreateMutex();
    g_systemState.ldrRaw = 0;
}