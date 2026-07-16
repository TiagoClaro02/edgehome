/*
 * state_manager.h
 *
 *  Created on: 16/07/2026
 *      Author: tmcla
 */

#pragma once
#include "cmsis_os.h"
#include "state/system_state.h"

extern osMutexId_t stateMutexHandle;

void initStateManager(void);

/* INC_STATE_STATE_MANAGER_H_ */
