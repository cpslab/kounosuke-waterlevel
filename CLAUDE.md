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
2. `loop()` monitors serial input for commands to dynamically change sleep duration
3. Reads distance from ultrasonic sensor (4-byte protocol: 0xFF header, 2 bytes distance, 1 byte checksum)
4. Sends JSON payload `{"distance": X, "fieldId": "Yokosuka-A1"}` to `http://uni.soracom.io` via HTTP POST
5. Powers off sensors and enters deep sleep (configurable via serial, default 15 minutes)

### Key Functions in `src/main.cpp`
- `setup()`: Initializes hardware serial, GPIO, and global variables
- `loop()`: Main loop that monitors serial commands and handles sensor data
- `handleSerialCommand()`: Processes serial input to dynamically change sleep duration (3-3600 seconds)
- `readSensorData()`: Reads 4-byte data from ultrasonic sensor
- `processSensorData()`: Validates sensor data and parses distance
- `sendAndSleep()`: Sends data via `serial_send()` and enters deep sleep
- `esp32c3_deepsleep()`: Handles WiFi/BT shutdown and enters deep sleep with configurable duration

### Libraries
- `EspSoftwareSerial`: Software serial communication
- `PCF8563`: RTC library (included in lib/, currently unused in main code)
