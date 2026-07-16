/*
 * state_manager.c
 *
 *  Created on: 16/07/2026
 *      Author: tmcla
 */

#include "state/state_manager.h"

void initStateManager(void) {
    g_systemState.temperature = 0;
    g_systemState.pressure    = 0;
    g_systemState.fanSpeed    = 0;
}
