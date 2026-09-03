#include <Arduino.h>

#include "state/state_manager.h"
#include "tasks/serial_task.h"
#include "tasks/control_task.h"
#include "tasks/system_task.h"
#include "tasks/network_task.h"
#include "tasks/ota_task.h"
#include "state/task_handles.h"
#include "config/config.h"


void setup() {
  Serial.begin(115200);
  delay(1000);

  initStateManager();

  xTaskCreatePinnedToCore(
    SerialTask,
    "Serial",
    SERIAL_TASK_STACK_SIZE,
    NULL,
    3,
    &serialTaskHandle,
    1);

  xTaskCreatePinnedToCore(
    ControlTask,
    "Control",
    CONTROL_TASK_STACK_SIZE,
    NULL,
    2,
    &controlTaskHandle,
    1);

xTaskCreatePinnedToCore(
    NetworkTask,
    "Network",
    NETWORK_TASK_STACK_SIZE,
    NULL,
    2,
    &networkTaskHandle,
    0);

xTaskCreatePinnedToCore(
    SystemTask,
    "System",
    SYSTEM_TASK_STACK_SIZE,
    NULL,
    2,
    &systemTaskHandle,
    1);

xTaskCreatePinnedToCore(
    otaTask,
    "OTATask",
    8192,
    NULL,
    1,
    &otaTaskHandle,
    0
);
}

void loop() {
  // RTOS owns everything now
}