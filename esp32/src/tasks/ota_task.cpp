// ota_task.cpp
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

#include "tasks/ota_task.h"
#include "state/task_handles.h"

static char pendingUrl[256];
static volatile bool otaInProgress = false;

void triggerOTA(const char* url) {
    if (otaInProgress) return;
    strncpy(pendingUrl, url, sizeof(pendingUrl) - 1);
    pendingUrl[sizeof(pendingUrl) - 1] = '\0';
    xTaskNotifyGive(otaTaskHandle);
}

static void performOTA(const char* url) {
    otaInProgress = true;
    Serial.printf("[OTA] Connecting to %s\n", url);   // add this

    HTTPClient http;
    http.begin(url);
    http.addHeader("Connection", "close");
    int code = http.GET();
    Serial.printf("[OTA] HTTP GET returned code: %d\n", code);   // add this
    if (code != 200) {
        http.end();
        otaInProgress = false;
        return;
    }

    int len = http.getSize();
    Serial.printf("[OTA] Content length: %d\n", len);
    if (!Update.begin(len)) {
        Serial.println("[OTA] Update.begin failed");
        http.end();
        otaInProgress = false;
        return;
    }

    WiFiClient* stream = http.getStreamPtr();

    uint8_t buf[512];
    size_t totalWritten = 0;
    bool firstChunk = true;

    while (http.connected() && (totalWritten < (size_t)len)) {
        size_t available = stream->available();
        if (available) {
            size_t toRead = min(available, sizeof(buf));
            size_t readBytes = stream->readBytes(buf, toRead);

            if (firstChunk) {
                Serial.printf("[OTA] First %d bytes: ", (int)min((size_t)16, readBytes));
                for (size_t i = 0; i < min((size_t)16, readBytes); i++) {
                    Serial.printf("%02x ", buf[i]);
                }
                Serial.println();
                firstChunk = false;
            }

            size_t written = Update.write(buf, readBytes);
            if (written != readBytes) {
                Serial.printf("[OTA] Write mismatch: wrote %d of %d\n", written, readBytes);
                break;
            }
            totalWritten += written;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    Serial.printf("[OTA] Total written: %d / %d\n", totalWritten, len);

    if (totalWritten == (size_t)len && Update.end() && Update.isFinished()) {
        http.end();
        esp_restart();
    } else {
        Update.printError(Serial);
        http.end();
        otaInProgress = false;
    }
}

void otaTask(void* pvParameters) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Serial.println("[OTA] Task woken, starting performOTA");
        performOTA(pendingUrl);
        Serial.println("[OTA] performOTA returned");
    }
}