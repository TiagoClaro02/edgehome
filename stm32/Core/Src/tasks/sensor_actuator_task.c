#include "tasks/sensor_actuator_task.h"
#include "state/state_manager.h"
#include "main.h"
#include "bmp280.h"
#include <stdio.h>

#define BMP280_ADDR 0x76

void SensorActuatorTaskImpl(void *argument)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    BMP280_HandleTypedef bmp280;
    bmp280_params_t params;

    bmp280.i2c  = &hi2c1;
    bmp280.addr = BMP280_I2C_ADDRESS_0;

    bmp280_init_default_params(&params);
    bmp280_init(&bmp280, &params);

    float temperature = 0.0f;
    float pressure    = 0.0f;
    float humidity    = 0.0f;


    for(;;)
    {

        if (bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
        {
            if (osMutexAcquire(stateMutexHandle, 100) == osOK)
            {
                g_systemState.temperature = (int)(temperature * 100);
                g_systemState.pressure    = (int)pressure;
                osMutexRelease(stateMutexHandle);
            }
        }

        uint8_t fanSpeed = 50;
        uint32_t pulse = (fanSpeed * 3999) / 100;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);

        if (osMutexAcquire(stateMutexHandle, 100) == osOK)
        {
            g_systemState.fanSpeed = fanSpeed;
            osMutexRelease(stateMutexHandle);
        }

        osDelay(5000);
    }
}
