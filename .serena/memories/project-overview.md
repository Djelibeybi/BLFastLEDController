# BLFastLEDController - Project Overview

## Purpose
ESP32 firmware that connects to Bambu Lab 3D printers (X1, X1C, P1P, P1S) via MQTT and controls addressable RGB LED strips using FastLED. Provides visual feedback for printing stages, errors, and HMS alerts.

## Version
Current version: 3.3.1 (codename: Baldrick)

## Key Features
- MQTT connection to Bambu Lab printers (TLS port 8883)
- Support for multiple LED chipsets: WS2812B, SK6812, SK6812-RGBW, WS2811, WS2814-RGBW, APA102, NEOPIXEL
- LED pattern effects: Solid, Breathing, Chase, Rainbow, Progress Bar
- WiFi and Ethernet connectivity (LAN8720A PHY boards)
- Web-based configuration interface
- Improv WiFi provisioning protocol
- Optional relay control for LED strip power
- HMS (Health Management System) error indication

## Connectivity Options
1. **WiFi** (esp32dev) - Standard WiFi connection with AP fallback mode
2. **Ethernet Gledopto** (esp32_eth_gledopto) - For Gledopto Elite 2D/4D-EXMU boards
3. **Ethernet IoTorero** (esp32_eth_iotorero) - For IoTorero ESP32 ETH boards

## Tech Stack
- **Platform**: ESP32 (Arduino framework)
- **Build System**: PlatformIO
- **Languages**: C++ (firmware), Python (build scripts), HTML/CSS/JS (web UI)
- **Key Libraries**:
  - FastLED v3.7.7+ (LED control)
  - ArduinoJson v7.4.2 (JSON parsing)
  - PubSubClient v2.8.0 (MQTT client)
  - AsyncTCP + ESPAsyncWebServer (async web server)
  - ESP32SSDP (SSDP discovery)
  - MycilaWebSerial (web-based serial console)
  - improv-wifi SDK v1.2.5 (WiFi provisioning)

## Filesystem
- Uses LittleFS for persistent storage
- Configuration stored in `/blflcconfig.json`
- Partition scheme: min_spiffs.csv

## License
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
