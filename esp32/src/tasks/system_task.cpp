#include <Arduino.h>
#include <WiFi.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "tasks/system_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "state/task_handles.h"
#include "config/config.h"

// Screen configuration — swap these to switch which screen shows what
#define SYSTEM_SCREEN_ADDR  0x3D
#define STACKS_SCREEN_ADDR  0x3C
#define SYSTEM_SCREEN_ROTATION U8G2_R0  // flipped
#define STACKS_SCREEN_ROTATION U8G2_R2  // normal

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_system(SYSTEM_SCREEN_ROTATION, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_stacks(STACKS_SCREEN_ROTATION, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);


struct TaskStackInfo {
    const char*  name;
    uint32_t     hwm;
    uint32_t     allocated;
};

static void drawHeapBar(U8G2& u8g2, uint32_t value, uint32_t total, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    const uint32_t THRESHOLD = 20480;
    const uint8_t  THRESH_X  = x + (uint8_t)((float)THRESHOLD / total * w);
    uint8_t        fill      = (uint8_t)(((float)(total - value) / total) * w);

    u8g2.drawFrame(x, y, w, h);
    u8g2.drawBox(x, y, fill, h);
    u8g2.drawVLine(THRESH_X, y, h);
}

static void drawSystemScreen(uint32_t freeHeap, uint32_t minHeap, bool wifiOk, const char* ip)
{
    const uint32_t TOTAL_HEAP = ESP.getHeapSize();
    char line[32];

    u8g2_system.clearBuffer();
    u8g2_system.setFont(u8g2_font_7x13B_tf);
    u8g2_system.drawStr(0, 12, "SYSTEM");
    u8g2_system.drawHLine(0, 14, 128);

    u8g2_system.setFont(u8g2_font_6x10_tf);

    snprintf(line, sizeof(line), "Heap %u", freeHeap);
    u8g2_system.drawStr(0, 26, line);
    drawHeapBar(u8g2_system, freeHeap, TOTAL_HEAP, 90, 18, 38, 8);

    snprintf(line, sizeof(line), "Min  %u", minHeap);
    u8g2_system.drawStr(0, 38, line);
    drawHeapBar(u8g2_system, minHeap, TOTAL_HEAP, 90, 30, 38, 8);

    snprintf(line, sizeof(line), "Wifi: %s", wifiOk ? "Yes" : "No");
    u8g2_system.drawStr(0, 50, line);

    if (wifiOk)
    {
        snprintf(line, sizeof(line), "IP: %s", ip);
        u8g2_system.drawStr(0, 62, line);
    }

    u8g2_system.sendBuffer();
}

static void drawStacksScreen(TaskStackInfo tasks[], uint8_t count)
{
    u8g2_stacks.clearBuffer();
    u8g2_stacks.setFont(u8g2_font_7x13B_tf);
    u8g2_stacks.drawStr(0, 12, "STACKS");
    u8g2_stacks.drawHLine(0, 14, 128);

    u8g2_stacks.setFont(u8g2_font_6x10_tf);

    const uint8_t BAR_X   = 80;
    const uint8_t BAR_W   = 46;
    const uint8_t ROW_H   = 12;
    const uint8_t START_Y = 24;

    for (uint8_t i = 0; i < count; i++)
    {
        uint8_t y = START_Y + i * ROW_H;

        char line[20];
        snprintf(line, sizeof(line), "%-8s%4u", tasks[i].name, tasks[i].hwm);
        u8g2_stacks.drawStr(0, y, line);

        float used   = (float)(tasks[i].allocated - tasks[i].hwm);
        float ratio  = used / (float)tasks[i].allocated;
        uint8_t fill = (uint8_t)(ratio * BAR_W);

        u8g2_stacks.drawFrame(BAR_X, y - 8, BAR_W, 9);
        if (fill > 0)
            u8g2_stacks.drawBox(BAR_X, y - 8, fill, 9);
    }

    u8g2_stacks.sendBuffer();
}

void SystemTask(void *pvParameters)
{
    Wire.begin(21, 22);

    u8g2_system.setI2CAddress(SYSTEM_SCREEN_ADDR * 2);
    u8g2_stacks.setI2CAddress(STACKS_SCREEN_ADDR * 2);

    u8g2_system.begin();
    u8g2_stacks.begin();

    uint32_t freeHeap = 0;
    uint32_t minHeap  = 0;
    bool     wifiOk   = false;
    String   ipStr    = "";

    for (;;)
    {
        freeHeap = ESP.getFreeHeap();
        minHeap  = ESP.getMinFreeHeap();
        wifiOk   = WiFi.isConnected();
        ipStr    = wifiOk ? WiFi.localIP().toString() : "";

        uint32_t serialHWM   = serialTaskHandle   ? uxTaskGetStackHighWaterMark(serialTaskHandle)   : 0;
        uint32_t controlHWM = controlTaskHandle ? uxTaskGetStackHighWaterMark(controlTaskHandle) : 0;
        uint32_t networkHWM = networkTaskHandle ? uxTaskGetStackHighWaterMark(networkTaskHandle) : 0;
        uint32_t systemHWM  = systemTaskHandle  ? uxTaskGetStackHighWaterMark(systemTaskHandle)  : 0;

        TaskStackInfo tasks[] = {
            {"Serial",   serialHWM,   SERIAL_TASK_STACK_SIZE},
            {"Control", controlHWM, CONTROL_TASK_STACK_SIZE},
            {"Network", networkHWM, NETWORK_TASK_STACK_SIZE},
            {"System",  systemHWM,  SYSTEM_TASK_STACK_SIZE}
        };

        drawSystemScreen(freeHeap, minHeap, wifiOk, ipStr.c_str());
        drawStacksScreen(tasks, sizeof(tasks) / sizeof(tasks[0]));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}