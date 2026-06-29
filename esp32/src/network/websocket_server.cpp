#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "network/websocket_server.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void *arg,
             uint8_t *data,
             size_t len) {

  if (type == WS_EVT_CONNECT) {
    Serial.println("[WS] Client connected");
  }
}

void initWebSocket() {

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "ESP32 WebSocket OK");
  });

  server.begin();

  Serial.println("[WS] Server started");
}

void sendTelemetry(const char* message) {
  ws.textAll(message);
}