#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "tasks/network_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "network/wifi_manager.h"
#include "config/credentials.h"

static WiFiClient   wifiClient;
static PubSubClient mqttClient(wifiClient);

static void ensureMQTTConnected()
{
    while (!mqttClient.connected())
    {
        Serial.print("[MQTT] Connecting...");
        if (mqttClient.connect("ESP32_edgehome"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.printf("failed, rc=%d, retrying in 5s\n", mqttClient.state());
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

void NetworkTask(void *pvParameters)
{
    initWiFi();
    vTaskDelay(pdMS_TO_TICKS(500));
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

    char message[64];

    for (;;)
    {
        ensureMQTTConnected();
        mqttClient.loop();

        int temperature = 0;
        int pressure = 0;
        if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            temperature = g_systemState.temperature;
            pressure = g_systemState.pressure;
            xSemaphoreGive(g_stateMutex);
        }
        else
        {
            Serial.println("[WARNING] Mutex timeout in NetworkTask");
        }

        snprintf(message, sizeof(message),
                "{\"temp\":%d,\"pres\":%d}",
                temperature, pressure);
        mqttClient.publish("home/sensors", message);
        Serial.printf("[MQTT] Published: %s\n", message);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}