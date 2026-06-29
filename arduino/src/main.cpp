#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "state/task_handles.h"
#include "state/state_manager.h"

#include "tasks/system_task.h"
#include "tasks/sensor_actuator_task.h"
#include "tasks/comms_task.h"

void setup() {
    Serial.begin(9600);

    initStateManager();
    xTaskCreate(SensorActuatorTask, "SensorActuator", 128, NULL, 1, &sensorActuatorHandle);
    xTaskCreate(SystemTask,         "System",         128, NULL, 1, &systemHandle);
    xTaskCreate(CommsTask, "Comms", 128, NULL, 1, &commsHandle);
    vTaskStartScheduler();
}

void loop() {}