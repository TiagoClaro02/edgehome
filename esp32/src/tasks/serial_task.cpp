#include <Arduino.h>
#include "tasks/serial_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"

#define ARDUINO_RX 16
#define ARDUINO_TX 17

void SerialTask(void *pvParameters)
{
    Serial2.begin(9600, SERIAL_8N1, ARDUINO_RX, ARDUINO_TX);

    char buffer[64];
    uint8_t idx = 0;

    for (;;)
    {
        while (Serial2.available())
        {
            char c = Serial2.read();

            if (c == '\n')
            {
                buffer[idx] = '\0';
                idx = 0;

                // parse {"ldr":512}
                int ldr = 0;
                if (sscanf(buffer, "{\"ldr\":%d}", &ldr) == 1)
                {
                    if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
                    {
                        g_systemState.ldrRaw = ldr;
                        xSemaphoreGive(g_stateMutex);
                    }
                    else
                    {
                        Serial.println("[WARNING] Mutex timeout in SerialTask");
                    }
                }
                else
                {
                    Serial.printf("[SERIAL] Parse error: %s\n", buffer);
                }
            }
            else if (idx < sizeof(buffer) - 1)
            {
                buffer[idx++] = c;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}