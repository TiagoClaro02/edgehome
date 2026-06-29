#include <Arduino.h>
#include <WiFi.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "tasks/system_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "state/task_handles.h"
#include "config/config.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

struct TaskStackInfo {
    const char*  name;
    uint32_t     hwm;
    uint32_t     allocated;
};

static void drawHeapBar(uint32_t value, uint32_t total, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    const uint32_t THRESHOLD = 20480;
    const uint8_t  THRESH_X  = x + (uint8_t)((float)THRESHOLD / total * w);
    uint8_t        fill      = (uint8_t)(((float)(total - value) / total) * w);

    u8g2.drawFrame(x, y, w, h);
    u8g2.drawBox(x, y, fill, h);
    u8g2.drawVLine(THRESH_X, y, h);
}

static void drawPage1(uint32_t freeHeap, uint32_t minHeap, bool wifiOk, const char* ip)
{
    const uint32_t TOTAL_HEAP = ESP.getHeapSize();
    char line[32];

    u8g2.setFont(u8g2_font_7x13B_tf);
    u8g2.drawStr(0, 12, "SYSTEM");
    u8g2.drawHLine(0, 14, 128);

    u8g2.setFont(u8g2_font_6x10_tf);

    // Heap row
    snprintf(line, sizeof(line), "Heap %u", freeHeap);
    u8g2.drawStr(0, 26, line);
    drawHeapBar(freeHeap, TOTAL_HEAP, 90, 18, 38, 8);

    // Min heap row
    snprintf(line, sizeof(line), "Min  %u", minHeap);
    u8g2.drawStr(0, 38, line);
    drawHeapBar(minHeap, TOTAL_HEAP, 90, 30, 38, 8);

    // WiFi row
    snprintf(line, sizeof(line), "Wifi: %s", wifiOk ? "Yes" : "No");
    u8g2.drawStr(0, 50, line);

    // IP row
    if (wifiOk)
    {
        snprintf(line, sizeof(line), "IP: %s", ip);
        u8g2.drawStr(0, 62, line);
    }
}

static void drawPage2(TaskStackInfo tasks[], uint8_t count)
{
    u8g2.setFont(u8g2_font_7x13B_tf);
    u8g2.drawStr(0, 12, "STACKS");
    u8g2.drawHLine(0, 14, 128);

    u8g2.setFont(u8g2_font_6x10_tf);

    const uint8_t BAR_X   = 80;
    const uint8_t BAR_W   = 46;
    const uint8_t ROW_H   = 12;
    const uint8_t START_Y = 24;

    for (uint8_t i = 0; i < count; i++)
    {
        uint8_t y = START_Y + i * ROW_H;

        // label + number
        char line[20];
        snprintf(line, sizeof(line), "%-8s%4u", tasks[i].name, tasks[i].hwm);
        u8g2.drawStr(0, y, line);

        // bar
        float used  = (float)(tasks[i].allocated - tasks[i].hwm);
        float ratio = used / (float)tasks[i].allocated;
        uint8_t fill = (uint8_t)(ratio * BAR_W);

        u8g2.drawFrame(BAR_X, y - 8, BAR_W, 9);
        if (fill > 0)
            u8g2.drawBox(BAR_X, y - 8, fill, 9);
    }
}

void SystemTask(void *pvParameters)
{
    Wire.begin(21, 22);
    u8g2.begin();

    uint8_t page = 0;

    bool btnPressed = false;
    uint32_t freeHeap   = 0;
    uint32_t minHeap    = 0;
    bool     wifiOk     = false;
    String   ipStr      = "";

    for (;;)
    {
        
        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            btnPressed = g_systemState.buttonPressed;
            if (btnPressed)
                g_systemState.buttonPressed = false;
            xSemaphoreGive(g_stateMutex);
        }

        freeHeap = ESP.getFreeHeap();
        minHeap  = ESP.getMinFreeHeap();
        wifiOk   = WiFi.isConnected();
        ipStr    = wifiOk ? WiFi.localIP().toString() : "";

        uint32_t inputHWM   = inputTaskHandle   ? uxTaskGetStackHighWaterMark(inputTaskHandle)   : 0;
        uint32_t controlHWM = controlTaskHandle ? uxTaskGetStackHighWaterMark(controlTaskHandle) : 0;
        uint32_t networkHWM = networkTaskHandle ? uxTaskGetStackHighWaterMark(networkTaskHandle) : 0;
        uint32_t systemHWM  = systemTaskHandle  ? uxTaskGetStackHighWaterMark(systemTaskHandle)  : 0;

        TaskStackInfo tasks[] = {
            {"Input",   inputHWM,   INPUT_TASK_STACK_SIZE},
            {"Control", controlHWM, CONTROL_TASK_STACK_SIZE},
            {"Network", networkHWM, NETWORK_TASK_STACK_SIZE},
            {"System",  systemHWM,  SYSTEM_TASK_STACK_SIZE}
        };

        // draw current page
        u8g2.clearBuffer();

        if (page == 0)
            drawPage1(freeHeap, minHeap, wifiOk, ipStr.c_str());
        else
            drawPage2(tasks, sizeof(tasks)/sizeof(tasks[0]));


        u8g2.sendBuffer();

        // cycle page every N iterations
        if (btnPressed){
            page = (page + 1) % 2;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}