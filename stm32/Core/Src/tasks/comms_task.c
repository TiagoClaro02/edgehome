#include "tasks/comms_task.h"
#include "state/state_manager.h"
#include "protocol/protocol.h"
#include "protocol/crc8.h"
#include "main.h"
#include <string.h>

static uint8_t buildSensorFrame(uint8_t *frame, int32_t temp, int32_t pres)
{
    frame[0] = FRAME_START;
    frame[1] = MSG_SENSOR_DATA;
    frame[2] = 0x08; // 8 bytes payload

    memcpy(&frame[3], &temp, 4);
    memcpy(&frame[7], &pres, 4);

    // CRC over TYPE + LENGTH + PAYLOAD (bytes 1-10)
    frame[11] = crc8(&frame[1], 10);
    frame[12] = FRAME_END;

    return 13;
}

static CommsStatus waitForAck(void)
{
    uint8_t rxBuf[6];
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, rxBuf, 6, ACK_TIMEOUT_MS);

    if (status == HAL_TIMEOUT)
        return COMMS_TIMEOUT;

    if (status != HAL_OK)
        return COMMS_ERROR;

    // validate frame
    if (rxBuf[0] != FRAME_START || rxBuf[5] != FRAME_END || rxBuf[1] != MSG_ACK)
        return COMMS_ERROR;

    // verify CRC on TYPE + LENGTH + STATUS (bytes 1-3)
    if (crc8(&rxBuf[1], 3) != rxBuf[4])
        return COMMS_CRC_ERROR;

    switch (rxBuf[3])
    {
        case ACK_OK:        return COMMS_OK;
        case ACK_CRC_ERROR: return COMMS_CRC_ERROR;
        default:            return COMMS_ERROR;
    }
}

void CommsTaskImpl(void *argument)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

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

        uint8_t frame[13];
        uint8_t frameLen = buildSensorFrame(frame, temperature, pressure);

        CommsStatus finalStatus = COMMS_FAILED;
        uint8_t retryCount = 0;

        for (uint8_t attempt = 0; attempt < MAX_RETRIES; attempt++)
        {
            retryCount = attempt + 1;

            // update state with current retry count
            if (osMutexAcquire(stateMutexHandle, 100) == osOK)
            {
                g_systemState.retryCount = retryCount;
                osMutexRelease(stateMutexHandle);
            }

            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_UART_Transmit(&huart2, frame, frameLen, 100);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

            CommsStatus ack = waitForAck();

            if (ack == COMMS_OK)
            {
                finalStatus = COMMS_OK;
                break;
            }

            // update last ACK status for display during retries
            if (osMutexAcquire(stateMutexHandle, 100) == osOK)
            {
                g_systemState.lastAck = ack;
                osMutexRelease(stateMutexHandle);
            }

            finalStatus = ack;
        }

        // final status update after all attempts
        if (osMutexAcquire(stateMutexHandle, 100) == osOK)
        {
            g_systemState.lastAck    = finalStatus;
            g_systemState.retryCount = (finalStatus == COMMS_OK) ? 0 : retryCount;
            osMutexRelease(stateMutexHandle);
        }

        osDelay(1000);
    }
}
