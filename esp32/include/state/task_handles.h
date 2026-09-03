#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern TaskHandle_t serialTaskHandle;
extern TaskHandle_t controlTaskHandle;
extern TaskHandle_t networkTaskHandle;
extern TaskHandle_t systemTaskHandle;
extern TaskHandle_t otaTaskHandle;