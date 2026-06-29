#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "tasks/sensor_actuator_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"

#define LDR_PIN A0

void SensorActuatorTask(void *pvParameters)
{
    for (;;)
    {
        int ldr = analogRead(LDR_PIN);

        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            g_systemState.ldrRaw = ldr;
            xSemaphoreGive(g_stateMutex);
        }
        else
        {
            Serial.println("[WARNING] Mutex timeout in SensorActuatorTask");
        }

        Serial.print("LDR: ");
        Serial.println(ldr);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}