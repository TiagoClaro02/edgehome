# edgehome

A multi-board home monitoring system built as a practical embedded systems learning project. The architecture separates concerns across dedicated microcontrollers — sensor/actuator nodes feed data to a network gateway, which publishes to a local MQTT broker and serves a web dashboard.

## Intended Architecture

```
Sensor Node (Arduino / STM32)
    → UART →
Network Gateway (ESP32)
    → MQTT →
Raspberry Pi
    ├── Mosquitto broker
    └── Web dashboard
```

The sensor node handles all sensor reads and actuator control. The ESP32 handles exclusively network communication, receiving data from the sensor node over UART and publishing it to the MQTT broker on the Raspberry Pi.

## Current State

The project is under active development. The ESP32 currently handles both sensor/actuator logic and network communication while the dedicated sensor node is being brought up. MQTT migration is pending — the current network transport is WebSocket.

### ESP32

Built on FreeRTOS with dedicated tasks for each concern:

- **InputTask** — reads a page-switch button
- **ControlTask** — drives a status LED reflecting WiFi connection state
- **NetworkTask** — manages WiFi and broadcasts telemetry
- **SystemTask** — drives an SSD1306 OLED display with two monitoring pages: system health (heap, WiFi, IP) and FreeRTOS stack usage per task

### Arduino

Placeholder for the dedicated sensor/actuator node. Will communicate with the ESP32 over UART and eventually be replaced by an STM32.

## Stack

- PlatformIO
- Arduino framework (ESP32 + Arduino)
- FreeRTOS
- U8g2 (OLED display)

## Roadmap

### Done
- [x] FreeRTOS multi-task architecture on ESP32
- [x] Shared state management with mutex protection
- [x] WebSocket telemetry broadcasting
- [x] Capacitive keypad input (TTP229)
- [x] SSD1306 OLED display over I2C
- [x] System monitoring display (heap, WiFi, stack HWM per task)
- [x] Status LED with WiFi state indication
- [x] Page switching via hardware button with debounce
- [x] Multi-board project structure (ESP32 + Arduino)

### Pending
- [ ] UART communication between Arduino and ESP32
- [ ] MQTT migration (replacing WebSocket)
- [ ] Mosquitto broker on Raspberry Pi
- [ ] Web dashboard on Raspberry Pi
- [ ] OTA firmware updates
- [ ] STM32 replacing Arduino sensor node
- [ ] Power management