# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Water level monitoring system for Seeed XIAO ESP32-C3. Measures distance using an ultrasonic sensor and sends data to SORACOM via LTE modem using AT commands.

## Build Commands

This is a PlatformIO project. Use the following commands:

```bash
# Build
pio run

# Upload to device
pio run -t upload

# Serial monitor (57600 baud)
pio device monitor -b 57600

# Clean build
pio run -t clean
```

## Architecture

### Hardware
- **MCU**: Seeed XIAO ESP32-C3
- **Sensor**: Ultrasonic distance sensor (connected via MySerial1 on GPIO 9/10 at 9600 baud)
- **LTE Modem**: SIM7080-series (communicates via MySerial0 at 57600 baud using AT commands)
- **Power control**: GPIO2 controls sensor power via switch

### Data Flow
1. Device wakes from deep sleep
2. Reads distance from ultrasonic sensor (4-byte protocol: 0xFF header, 2 bytes distance, 1 byte checksum)
3. Sends JSON payload `{"distance": X, "fieldId": "Yokosuka-A1"}` to `http://uni.soracom.io` via HTTP POST
4. Powers off sensors and enters deep sleep for 15 minutes (SLEEPTIME_SECONDS = 900)

### Key Functions in `src/main.cpp`
- `esp32c3_deepsleep()`: Handles WiFi/BT shutdown and enters deep sleep
- `sendATCommand()`: Sends AT commands to LTE modem with timeout and error handling
- `sendBody()`: Sends HTTP body data via AT+SHBOD command
- `serial_send()`: Orchestrates the full HTTP POST sequence to SORACOM

### Libraries
- `EspSoftwareSerial`: Software serial communication
- `PCF8563`: RTC library (included in lib/, currently unused in main code)
