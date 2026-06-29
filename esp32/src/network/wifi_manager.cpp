#include <Arduino.h>
#include <WiFi.h>

#include "network/wifi_manager.h"
#include "config/credentials.h"



void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("[NET] Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[NET] WiFi connected");
  Serial.println(WiFi.localIP());
}