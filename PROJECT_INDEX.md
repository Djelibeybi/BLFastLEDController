# Project Index: BLFastLEDController

Generated: 2025-12-02
Version: 3.3.1
Codename: Baldrick

## Overview

ESP32 firmware that connects to Bambu Lab 3D printers (X1, X1C, P1P, P1S) via MQTT and controls addressable RGB LED strips based on printer state using FastLED. Supports WiFi and Ethernet connectivity.

## Project Structure

```
BLLEDController-NG/
├── src/
│   ├── main.cpp              # Entry point: setup(), loop()
│   ├── blflc/                # Core firmware modules
│   │   ├── types.h           # Global data structures
│   │   ├── leds.h/cpp        # LED control (FastLED)
│   │   ├── patterns.h/cpp    # LED patterns (solid, breathing, chase, rainbow, progress)
│   │   ├── mqttmanager.h/cpp # MQTT client for Bambu printer
│   │   ├── mqttparsingutility.h/cpp # MQTT JSON parsing
│   │   ├── web-server.h/cpp  # AsyncWebServer + WebSocket
│   │   ├── filesystem.h/cpp  # LittleFS config persistence
│   │   ├── wifi-manager.h/cpp # WiFi connection + AP mode
│   │   ├── eth-manager.h/cpp # Ethernet support (LAN8720A)
│   │   ├── improv.h/cpp      # Improv WiFi provisioning
│   │   ├── improv-serial.h/cpp # Improv Serial protocol
│   │   ├── bblprinterdiscovery.h/cpp # SSDP printer discovery
│   │   ├── ssdp.h/cpp        # SSDP protocol
│   │   ├── logserial.h/cpp   # Debug logging (WebSerial)
│   │   └── autogrowbufferstream.h/cpp # Buffer utilities
│   └── www/                  # Web interface assets
│       ├── setupPage.html    # Main LED configuration → /submitConfig
│       ├── wifiSetup.html    # WiFi setup → /submitWiFi
│       ├── printerSetup.html # Printer setup → /submitPrinter
│       ├── authSetup.html    # Auth setup → /submitAuth
│       ├── debugSetup.html   # Debug setup → /submitDebug
│       ├── advancedSetup.html # Advanced setup → /submitHostname
│       ├── updatePage.html   # OTA firmware update
│       ├── backupRestore.html # Config backup/restore
│       ├── webSerialPage.html # Debug log viewer
│       ├── style.css         # Shared styles
│       ├── particleCanvas.js # UI effects
│       ├── blflc.svg         # Logo
│       ├── favicon.png       # Icon
│       └── www.h             # Compressed assets (generated)
├── .github/workflows/
│   └── build-and-release.yml # CI/CD workflow
├── platformio.ini            # PlatformIO configuration
├── pre_build.py              # HTML compression script
├── merge_firmware.py         # Firmware merger
└── bblp_sim.py               # Printer simulator (testing)
```

## Build Environments

| Environment | Board | Features |
|-------------|-------|----------|
| `esp32dev` | ESP32 DevKit | WiFi + Improv Serial |
| `esp32_eth_gledopto` | Gledopto Elite 2D/4D | Ethernet (LAN8720A) |
| `esp32_eth_iotorero` | IoTorero ESP32 ETH | Ethernet (LAN8720A) |

## Core Data Structures (`src/blflc/types.h`)

| Structure | Purpose |
|-----------|---------|
| `PrinterVariables` | Printer state: gcodeState, stage, overridestage, HMS errors, door status, ledReason |
| `PrinterConfig` | Settings: IP, colors, patterns, timeouts, relay config |
| `GlobalVariables` | WiFi credentials, firmware version |
| `LedConfig` | LED strip: chipType, colorOrder, ledCount, pins |
| `COLOR` | RGB color with hex representation |

### LED Chip Types
`WS2812B`, `SK6812`, `SK6812_RGBW`, `APA102`, `WS2811`, `NEOPIXEL`

### Color Orders
`GRB`, `RGB`, `BRG`, `RBG`, `BGR`, `GBR`, `GRBW`, `RGBW`

### LED Patterns
`SOLID`, `BREATHING`, `CHASE`, `RAINBOW`, `PROGRESS`

## Web Endpoints

| Page | Endpoint | Handler |
|------|----------|---------|
| LED Setup | `/submitConfig` | `handleSubmitConfig` |
| WiFi Setup | `/submitWiFi` | `handleSubmitWiFi` |
| Printer Setup | `/submitPrinter` | `handleSubmitPrinter` |
| Auth Setup | `/submitAuth` | `handleSubmitAuth` |
| Debug Setup | `/submitDebug` | `handleSubmitDebug` |
| Advanced | `/submitHostname` | `handleSubmitHostname` |

### Read-Only Endpoints

| Endpoint | Description |
|----------|-------------|
| `/` | Main LED setup page |
| `/wifi` | WiFi setup |
| `/printer` | Printer setup |
| `/auth` | Auth setup |
| `/debug` | Debug setup |
| `/advanced` | Advanced setup |
| `/fwupdate` | OTA firmware upload |
| `/backuprestore` | Config backup/restore |
| `/webserial` | Debug log viewer |
| `/getConfig` | GET current config JSON |
| `/config.json` | GET network type info |
| `/api/ledtest` | Trigger LED test |
| `/wifiscan` | Scan WiFi networks |
| `/printerlist` | List discovered printers |
| `/factoryreset` | Wipe all settings |
| `/reboot` | Restart device |

## Key Functions

### MQTT (`src/blflc/mqttmanager.cpp`)
- `parseHMS()` - Parse HMS errors and apply overrides
- `applyHMSOverride()` - Map HMS codes to stage overrides
- `ParseCallback()` - Parse printer JSON status

### LED Control (`src/blflc/leds.cpp`)
- `updateleds()` - Main state machine for LED behavior
- `handleStageColors()` - Stage-specific color handling
- `setLedState()` - Control LED color and pattern

## Printer Stages (MQTT `stg_cur`)

| Value | Stage |
|-------|-------|
| -1/255 | IDLE |
| 0 | Printing |
| 1 | Bed leveling |
| 2 | Preheating |
| 8 | Calibrating extrusion |
| 9 | Scanning bed |
| 10 | First layer inspection |
| 12 | Calibrating micro lidar |
| 14 | Cleaning nozzle |
| 16/30 | Paused |
| 34 | First layer error (paused) |
| 35 | Nozzle clog (paused) |

## HMS Stage Override

The `overridestage` variable can override `stage` for LED behavior:
- Value 999 = no override (inactive)
- Set by HMS error codes via `applyHMSOverride()`
- HMS code `0x0C0003000003000B` → stage 10 (First Layer Inspection)

## LED Priority (highest to lowest)

1. Maintenance/Test/Disco/Progress Bar modes
2. Error states (HMS fatal/serious, temperature failures)
3. Pause states (stage 16, 30, 34, 35)
4. Stage-specific colors (1, 8, 9, 10, 12, 14)
5. Idle timeout
6. Running/active states
7. Finish indication
8. LED replication (mirror chamber light)

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| FastLED | ^3.7.7 | Addressable LED control |
| ArduinoJson | 7.4.2 | JSON parsing |
| PubSubClient | 2.8.0 | MQTT client |
| ESP32SSDP | 1.2.1 | SSDP discovery |
| AsyncTCP | latest | Async TCP |
| ESPAsyncWebServer | latest | Web server |
| MycilaWebSerial | ^8.1.1 | Debug serial over web |
| improv-wifi/sdk-cpp | v1.2.5 | Improv WiFi provisioning |

## Build Commands

```bash
# Build WiFi firmware
uv run pio run -e esp32dev

# Build Ethernet firmware (Gledopto)
uv run pio run -e esp32_eth_gledopto

# Build Ethernet firmware (IoTorero)
uv run pio run -e esp32_eth_iotorero

# Upload firmware
uv run pio run -e esp32dev -t upload

# Upload filesystem
uv run pio run -e esp32dev -t uploadfs

# Monitor serial
uv run pio device monitor -b 115200

# Clean build
uv run pio run -e esp32dev -t clean
```

## Build Outputs

Location: `.firmware/`
- `BLFLC_V{version}.bin` - Merged firmware for initial flash
- `BLFLC_V{version}.bin.ota` - OTA update file

## Configuration

File: `/blledconfig.json` (LittleFS)

Key settings:
- WiFi: SSID, password, BSSID
- Printer: IP, serial number, access code
- LED: chip type, color order, count, data pin, clock pin
- Colors: per-state RGB values with patterns
- Behavior: finish timeout, inactivity timeout, relay config
- Debug: debugging, debugOnChange, mqttdebug

## Testing

```bash
# Run printer simulator
python bblp_sim.py
```

## Session Memories

Previous work sessions documented in `.serena/memories/`:
- `session-led-and-config-fixes-2025-12` - LED status and config save fixes
- `session-web-ui-overhaul-2025-12` - Major UI overhaul
- `session-ethernet-support-2025-12` - Ethernet support
- `session-improv-serial-2025-12` - Improv Serial implementation
- `session-progress-bar-fix-2025-12` - Progress bar fix
