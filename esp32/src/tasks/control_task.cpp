#include <Arduino.h>
#include <WiFi.h>
#include "tasks/control_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"

#define LED_PIN 2

void ControlTask(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);

    bool ledState = false;

    for (;;)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            // fast blink — 100ms on/off
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            // solid ON when connected
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}