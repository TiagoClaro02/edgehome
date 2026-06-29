#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include "tasks/comms_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"

#define UART_RX 10
#define UART_TX 11

SoftwareSerial espSerial(UART_RX, UART_TX);

void CommsTask(void *pvParameters)
{
    espSerial.begin(9600);

    for (;;)
    {
        int ldr = 0;

        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ldr = g_systemState.ldrRaw;
            xSemaphoreGive(g_stateMutex);
        }
        else
        {
            Serial.println("[WARNING] Mutex timeout in CommsTask");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        char message[32];
        snprintf(message, sizeof(message), "{\"ldr\":%d}\n", ldr);
        espSerial.print(message);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}