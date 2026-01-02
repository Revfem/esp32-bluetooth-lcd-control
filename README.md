
# ESP32 Bluetooth LCD Control
Embedded system project that enables Bluetooth-based control of an I2C LCD display using an ESP32 microcontroller.

![Language](https://img.shields.io/github/languages/top/Revfem/esp32-bluetooth-lcd-control)
![Stars](https://img.shields.io/github/stars/Revfem/esp32-bluetooth-lcd-control)
![Repo Size](https://img.shields.io/github/repo-size/Revfem/esp32-bluetooth-lcd-control)
![ESP32](https://img.shields.io/badge/ESP32-Embedded-blue)
![Arduino](https://img.shields.io/badge/Arduino-Framework-green)


# Author:
Eva-Maria Florea
## Overview
This project implements a Bluetooth communication interface between a mobile device and an ESP32, allowing remote control of a 16x2 I2C LCD display. Commands are sent in JSON format and processed by the ESP32 to update text, icons, and scrolling behavior on the LCD.

## Features
- Bluetooth communication using ESP32 BluetoothSerial
- JSON-based command protocol
- Text display on 16x2 I2C LCD
- Custom icon rendering
- Left/right scrolling control using timers
- Bidirectional communication (command + response)

## Technologies Used
- ESP32
- Arduino Framework
- BluetoothSerial
- LiquidCrystal_I2C
- ArduinoJson
- Ticker library

## Hardware Setup
  - ESP32 Development Board
  - 16x2 I2C LCD (address 0x27)
  - Power via USB

## Supported Commands
Commands are sent via Bluetooth as JSON objects.
  - Get LCD Info
    {
      "action": "getLCDs"
    }
    
  - Set Text
    {
      "action": "setIcons",
      "id": 3,
      "icons": [
        { "data": [0,10,21,17,17,14,0,0] }
      ]
    }
    
  - Scroll Control
    {
      "action": "scroll",
      "id": 3,
      "direction": "Left"
    }

## Usage
1. Open `src/main.cpp` in Arduino IDE
2. Install required libraries: ArduinoJson, Ticker, BluetoothSerial
3. Upload to ESP32
4. Pair Bluetooth and send JSON commands

  
