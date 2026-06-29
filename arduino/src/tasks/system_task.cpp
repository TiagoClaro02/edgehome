#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>
#include "tasks/system_task.h"
#include "state/task_handles.h"

// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

#define SENSOR_ACTUATOR_STACK 128
#define SYSTEM_STACK          128
#define COMMS_STACK           128

static void drawTaskPage(const char* name, UBaseType_t hwm, uint16_t allocated)
{
    // row 0 — name and free words
    lcd.setCursor(0, 0);
    lcd.print(name);
    lcd.print(" ");
    lcd.print(hwm);
    lcd.print("w   ");

    // row 1 — usage bar across 16 characters
    uint8_t used  = allocated - hwm;
    uint8_t fill  = (uint8_t)((float)used / allocated * 16);

    lcd.setCursor(0, 1);
    for (uint8_t i = 0; i < 16; i++)
        lcd.print(i < fill ? (char)0xFF : '-');
}

void SystemTask(void *pvParameters)
{
    lcd.begin(16, 2);

    uint8_t page      = 0;
    uint8_t pageCount = 3;

    for (;;)
    {
        UBaseType_t sensActHWM = sensorActuatorHandle ? uxTaskGetStackHighWaterMark(sensorActuatorHandle) : 0;
        UBaseType_t systemHWM  = systemHandle         ? uxTaskGetStackHighWaterMark(systemHandle)         : 0;
        UBaseType_t commsHWM   = commsHandle          ? uxTaskGetStackHighWaterMark(commsHandle)          : 0;

        lcd.clear();

        switch (page)
        {
            case 0:
                drawTaskPage("SensAct", sensActHWM, SENSOR_ACTUATOR_STACK);
                break;
            case 1:
                drawTaskPage("System ", systemHWM,  SYSTEM_STACK);
                break;
            case 2:
                drawTaskPage("Comms  ", commsHWM,   COMMS_STACK);
                break;
        }

        page = (page + 1) % pageCount;

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}