/*
 * system_state.h
 *
 *  Created on: 16/07/2026
 *      Author: tmcla
 */

#pragma once
#include <stdint.h>

typedef enum {
    COMMS_OK,
    COMMS_CRC_ERROR,
    COMMS_ERROR,
    COMMS_TIMEOUT,
    COMMS_FAILED
} CommsStatus;

typedef struct {
    int         temperature;
    int         pressure;
    uint8_t     fanSpeed;
    CommsStatus lastAck;
    uint8_t     retryCount;
} SystemState;

extern SystemState g_systemState;

 /* SYSTEM_STATE_H_ */
