#include <Arduino.h>

#include "state/state_manager.h"
#include "tasks/input_task.h"
#include "tasks/control_task.h"
#include "tasks/system_task.h"
#include "tasks/network_task.h"
#include "state/task_handles.h"
#include "config/config.h"


void setup() {
  Serial.begin(115200);
  delay(1000);

  initStateManager();

  xTaskCreatePinnedToCore(
    InputTask,
    "Input",
    INPUT_TASK_STACK_SIZE,
    NULL,
    2,
    &inputTaskHandle,
    1);

  xTaskCreatePinnedToCore(
    ControlTask,
    "Control",
    CONTROL_TASK_STACK_SIZE,
    NULL,
    1,
    &controlTaskHandle,
    1);

xTaskCreatePinnedToCore(
    NetworkTask,
    "Network",
    NETWORK_TASK_STACK_SIZE,
    NULL,
    1,
    &networkTaskHandle,
    0);

xTaskCreatePinnedToCore(
    SystemTask,
    "System",
    SYSTEM_TASK_STACK_SIZE,
    NULL,
    1,
    &systemTaskHandle,
    1);
}

void loop() {
  // RTOS owns everything now
}