/*
 * system_state.h
 *
 *  Created on: 16/07/2026
 *      Author: tmcla
 */

#pragma once
#include <stdint.h>

typedef struct {
    int temperature;  // BME280 temperature x100 (e.g. 2350 = 23.50°C)
    int pressure;     // BME280 pressure in Pa
    uint8_t fanSpeed; // PWM duty cycle 0-100%
} SystemState;

extern SystemState g_systemState;

 /* SYSTEM_STATE_H_ */
