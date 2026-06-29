#include <Arduino.h>
#include "tasks/input_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"

#define BUTTON_PIN 32

void InputTask(void *pvParameters)
{
    pinMode(BUTTON_PIN, INPUT); // HIGH at rest, LOW when pressed

    vTaskDelay(pdMS_TO_TICKS(200));

    bool prevButton  = (digitalRead(BUTTON_PIN) == LOW);

    for (;;)
    {
        // button — detect press transition only
        bool buttonNow = digitalRead(BUTTON_PIN) == LOW;
        bool buttonPressed = buttonNow && !prevButton;
        prevButton = buttonNow;

        if (buttonPressed)
            vTaskDelay(pdMS_TO_TICKS(50));

        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            if (buttonPressed)
            {
                g_systemState.buttonPressed = true;
            }
            xSemaphoreGive(g_stateMutex);
        }
        else
        {
            Serial.println("[WARNING] Mutex timeout in InputTask");
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}