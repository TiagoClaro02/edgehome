#include <Arduino.h>
#include "tasks/serial_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "protocol/protocol.h"
#include "protocol/crc8.h"
#include <string.h>

#define STM32_RX 16
#define STM32_TX 17

static void sendAck(uint8_t status)
{
    uint8_t frame[6];
    frame[0] = FRAME_START;
    frame[1] = MSG_ACK;
    frame[2] = 0x01;
    frame[3] = status;
    frame[4] = crc8(&frame[1], 3);
    frame[5] = FRAME_END;
    Serial2.write(frame, 6);
}

void SerialTask(void *pvParameters)
{
    Serial2.begin(9600, SERIAL_8N1, STM32_RX, STM32_TX);

    // parser state machine
    enum ParseState { WAIT_START, READ_TYPE, READ_LENGTH, READ_PAYLOAD, READ_CRC, READ_END };
    ParseState state = WAIT_START;

    uint8_t msgType    = 0;
    uint8_t msgLength  = 0;
    uint8_t payload[16];
    uint8_t payloadIdx = 0;
    uint8_t receivedCrc = 0;

    for (;;)
    {
        while (Serial2.available())
        {
            uint8_t c = Serial2.read();
            //Serial.printf("[UART] byte: 0x%02X state:%d\n", c, state);
            
            switch (state)
            {
                case WAIT_START:
                    if (c == FRAME_START)
                        state = READ_TYPE;
                    break;

                case READ_TYPE:
                    msgType = c;
                    state = READ_LENGTH;
                    break;

                case READ_LENGTH:
                    msgLength = c;
                    payloadIdx = 0;
                    if (msgLength > sizeof(payload))
                    {
                        // payload too big → abort
                        state = WAIT_START;
                        sendAck(ACK_ERROR);
                    }
                    else if (msgLength == 0)
                    {
                        state = READ_CRC;
                    }
                    else
                    {
                        state = READ_PAYLOAD;
                    }
                    break;

                case READ_PAYLOAD:
                    payload[payloadIdx++] = c;
                    if (payloadIdx >= msgLength)
                        state = READ_CRC;
                    break;

                case READ_CRC:
                    receivedCrc = c;
                    state = READ_END;
                    break;

                case READ_END:
                    if (c != FRAME_END)
                    {
                        sendAck(ACK_ERROR);
                        state = WAIT_START;
                        break;
                    }

                    // verify CRC over TYPE + LENGTH + PAYLOAD
                    uint8_t crcData[18];
                    crcData[0] = msgType;
                    crcData[1] = msgLength;
                    memcpy(&crcData[2], payload, msgLength);

                    if (crc8(crcData, 2 + msgLength) != receivedCrc)
                    {
                        sendAck(ACK_CRC_ERROR);
                        state = WAIT_START;
                        break;
                    }

                    // valid frame — process by type
                    if (msgType == MSG_SENSOR_DATA && msgLength == 8)
                    {
                        int32_t temp, pres;
                        memcpy(&temp, &payload[0], 4);
                        memcpy(&pres, &payload[4], 4);

                        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
                        {
                            g_systemState.temperature = temp;
                            g_systemState.pressure    = pres;
                            xSemaphoreGive(g_stateMutex);
                        }

                        sendAck(ACK_OK);
                    }
                    else
                    {
                        sendAck(ACK_ERROR);
                    }

                    state = WAIT_START;
                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}