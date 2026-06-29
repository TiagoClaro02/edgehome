#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern TaskHandle_t inputTaskHandle;
extern TaskHandle_t controlTaskHandle;
extern TaskHandle_t networkTaskHandle;
extern TaskHandle_t systemTaskHandle;