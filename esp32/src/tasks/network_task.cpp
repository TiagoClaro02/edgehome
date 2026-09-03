#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "tasks/network_task.h"
#include "tasks/ota_task.h"
#include "state/system_state.h"
#include "state/state_manager.h"
#include "network/wifi_manager.h"
#include "config/credentials.h"

#define LED_PIN 2
#define OTA_TOPIC "home/ota/update"

static WiFiClient   wifiClient;
static PubSubClient mqttClient(wifiClient);

static void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    if (strcmp(topic, OTA_TOPIC) == 0)
    {
        char url[256];
        unsigned int copyLen = min(length, sizeof(url) - 1);
        memcpy(url, payload, copyLen);
        url[copyLen] = '\0';
        Serial.printf("[MQTT] OTA trigger received: %s\n", url);
        triggerOTA(url);
    }
}

static void ensureMQTTConnected()
{
    while (!mqttClient.connected())
    {
        Serial.print("[MQTT] Connecting...");
        if (mqttClient.connect("ESP32_edgehome"))
        {
            Serial.println("connected");
            mqttClient.subscribe(OTA_TOPIC);
            Serial.printf("[MQTT] Subscribed to %s\n", OTA_TOPIC);
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
    mqttClient.setCallback(mqttCallback);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

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
        digitalWrite(LED_PIN, HIGH);
        mqttClient.publish("home/sensors", message);
        //Serial.printf("[MQTT] Published: %s\n", message);
        digitalWrite(LED_PIN, LOW);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}