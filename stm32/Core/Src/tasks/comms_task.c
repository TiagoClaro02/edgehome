/*
 * comms_task.c
 *
 *  Created on: 16/07/2026
 *      Author: tmcla
 */


#include "tasks/comms_task.h"
#include "state/state_manager.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

void CommsTaskImpl(void *argument)
{
    // PC13 is the onboard LED on Black Pill — active LOW
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // LED off

    for(;;)
    {
        int temperature = 0;
        int pressure    = 0;

        if (osMutexAcquire(stateMutexHandle, 100) == osOK)
        {
            temperature = g_systemState.temperature;
            pressure    = g_systemState.pressure;
            osMutexRelease(stateMutexHandle);
        }

        char message[64];
        snprintf(message, sizeof(message),
                 "{\"temp\":%d,\"pres\":%d}\n",
                 temperature, pressure);

        // blink LED when transmitting
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // LED on
        HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), 100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   // LED off

        osDelay(5000);
    }
}
