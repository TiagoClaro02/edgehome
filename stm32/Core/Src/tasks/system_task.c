#include "tasks/system_task.h"
#include "state/state_manager.h"
#include "main.h"
#include "u8g2.h"
#include <stdio.h>

#define SYSTEM_SCREEN_ADDR  0x3C
#define STACKS_SCREEN_ADDR  0x3D
#define SYSTEM_SCREEN_ROT   U8G2_R0
#define STACKS_SCREEN_ROT   U8G2_R0

#define STACK_WARNING_THRESHOLD 20

static u8g2_t u8g2_system;
static u8g2_t u8g2_stacks;

uint8_t u8g2_hw_i2c_byte(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while (arg_int--)
                buffer[buf_idx++] = *data++;
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8),
                                    buffer, buf_idx, 100);
            break;
    }
    return 1;
}

uint8_t u8g2_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
        case U8X8_MSG_DELAY_NANO:
        case U8X8_MSG_DELAY_100NANO:
            break;
    }
    return 1;
}

static void drawHeapBar(u8g2_t *u8g2, uint32_t value, uint32_t total,
                        uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t fill = (uint8_t)(((float)(total - value) / total) * w);
    u8g2_DrawFrame(u8g2, x, y, w, h);
    if (fill > 0)
        u8g2_DrawBox(u8g2, x, y, fill, h);
}

static void drawStackBar(u8g2_t *u8g2, uint32_t hwm, uint32_t allocated,
                         uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint32_t used = allocated - hwm;
    uint8_t fill  = (uint8_t)(((float)used / allocated) * w);
    u8g2_DrawFrame(u8g2, x, y, w, h);
    if (fill > 0)
        u8g2_DrawBox(u8g2, x, y, fill, h);
}

static void initScreens(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2_system,
        SYSTEM_SCREEN_ROT, u8g2_hw_i2c_byte, u8g2_gpio_and_delay);
    u8x8_SetI2CAddress(&u8g2_system.u8x8, SYSTEM_SCREEN_ADDR << 1);
    u8g2_InitDisplay(&u8g2_system);
    u8g2_SetPowerSave(&u8g2_system, 0);

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2_stacks,
        STACKS_SCREEN_ROT, u8g2_hw_i2c_byte, u8g2_gpio_and_delay);
    u8x8_SetI2CAddress(&u8g2_stacks.u8x8, STACKS_SCREEN_ADDR << 1);
    u8g2_InitDisplay(&u8g2_stacks);
    u8g2_SetPowerSave(&u8g2_stacks, 0);
}

static void drawSystemScreen(void)
{
    char line[32];
    uint8_t fanSpeed = 0;

    if (osMutexAcquire(stateMutexHandle, 100) == osOK)
    {
        fanSpeed = g_systemState.fanSpeed;
        osMutexRelease(stateMutexHandle);
    }

    uint32_t freeHeap  = xPortGetFreeHeapSize();
    uint32_t minHeap   = xPortGetMinimumEverFreeHeapSize();
    uint32_t totalHeap = configTOTAL_HEAP_SIZE;

    // uptime
    uint32_t ticks   = osKernelGetTickCount();
    uint32_t seconds = ticks / osKernelGetTickFreq();
    uint32_t minutes = (seconds / 60) % 60;
    uint32_t hours   = (seconds / 3600) % 24;
    uint32_t days    = seconds / 86400;

    u8g2_ClearBuffer(&u8g2_system);

    u8g2_SetFont(&u8g2_system, u8g2_font_7x13B_tf);
    u8g2_DrawStr(&u8g2_system, 0, 12, "SYSTEM");
    u8g2_DrawHLine(&u8g2_system, 0, 14, 128);

    u8g2_SetFont(&u8g2_system, u8g2_font_6x10_tf);

    // heap row
    snprintf(line, sizeof(line), "Heap %lu", freeHeap);
    u8g2_DrawStr(&u8g2_system, 0, 26, line);
    drawHeapBar(&u8g2_system, freeHeap, totalHeap, 90, 18, 38, 8);

    // min heap row
    snprintf(line, sizeof(line), "Min  %lu", minHeap);
    u8g2_DrawStr(&u8g2_system, 0, 38, line);
    drawHeapBar(&u8g2_system, minHeap, totalHeap, 90, 30, 38, 8);

    // fan row
    snprintf(line, sizeof(line), "Fan  %u%%", fanSpeed);
    u8g2_DrawStr(&u8g2_system, 0, 50, line);

    // uptime row
    snprintf(line, sizeof(line), "Up %lud %02luh%02lum", days, hours, minutes);
    u8g2_DrawStr(&u8g2_system, 0, 62, line);

    u8g2_SendBuffer(&u8g2_system);
}

static void drawStacksScreen(void)
{
    char line[32];

    uint32_t sensorHWM = SensorActuatorHandle ?
        uxTaskGetStackHighWaterMark(SensorActuatorHandle) : 0;
    uint32_t systemHWM = SystemHandle ?
        uxTaskGetStackHighWaterMark(SystemHandle) : 0;
    uint32_t commsHWM  = CommsHandle ?
        uxTaskGetStackHighWaterMark(CommsHandle) : 0;

    u8g2_ClearBuffer(&u8g2_stacks);

    u8g2_SetFont(&u8g2_stacks, u8g2_font_7x13B_tf);
    u8g2_DrawStr(&u8g2_stacks, 0, 12, "STACKS");
    u8g2_DrawHLine(&u8g2_stacks, 0, 14, 128);

    u8g2_SetFont(&u8g2_stacks, u8g2_font_6x10_tf);

    const uint8_t BAR_X  = 80;
    const uint8_t BAR_W  = 38;
    const uint8_t ROW_H  = 12;
    const uint8_t START_Y = 24;

    uint32_t hwms[]       = {sensorHWM, systemHWM, commsHWM};
    uint32_t allocated[]  = {
    		SensorActuator_attributes.stack_size / sizeof(StackType_t),
			System_attributes.stack_size / sizeof(StackType_t),
			Comms_attributes.stack_size / sizeof(StackType_t)
    };
    const char *names[]   = {"SensAct", "System ", "Comms  "};

    for (uint8_t i = 0; i < 3; i++)
    {
        uint8_t y = START_Y + i * ROW_H;

        snprintf(line, sizeof(line), "%-7s %3lu%s",
                 names[i], hwms[i],
                 hwms[i] < STACK_WARNING_THRESHOLD ? " !" : "  ");
        u8g2_DrawStr(&u8g2_stacks, 0, y, line);

        drawStackBar(&u8g2_stacks, hwms[i], allocated[i],
                     BAR_X, y - 8, BAR_W, 9);
    }

    u8g2_SendBuffer(&u8g2_stacks);
}

void SystemTaskImpl(void *argument)
{
    initScreens();

    for(;;)
    {
        drawSystemScreen();
        drawStacksScreen();
        osDelay(1000);
    }
}
