#include <Arduino.h>
#include "tasks/network_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "network/wifi_manager.h"
#include "network/websocket_server.h"

#define ARDUINO_RX 16
#define ARDUINO_TX 17

void NetworkTask(void *pvParameters)
{
    Serial2.begin(9600, SERIAL_8N1, ARDUINO_RX, ARDUINO_TX);

    initWiFi();
    initWebSocket();

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
                Serial.printf("[UART] Received: %s\n", buffer);
                idx = 0;
            }
            else if (idx < sizeof(buffer) - 1)
            {
                buffer[idx++] = c;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}