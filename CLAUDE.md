# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32 firmware that connects to Bambu Lab 3D printers (X1, X1C, P1P, P1S) via MQTT and controls addressable RGB LED strips using FastLED. Supports WiFi and Ethernet connectivity with visual feedback for printing stages, errors, and HMS alerts.

## Build Commands

PlatformIO project. Use `uv run pio` for all commands:

```bash
# Build WiFi firmware
uv run pio run -e esp32dev

# Build Ethernet firmware (Gledopto Elite 2D/4D)
uv run pio run -e esp32_eth_gledopto

# Build Ethernet firmware (IoTorero ESP32 ETH)
uv run pio run -e esp32_eth_iotorero

# Upload firmware
uv run pio run -e esp32dev -t upload

# Upload filesystem (LittleFS)
uv run pio run -e esp32dev -t uploadfs

# Monitor serial output
uv run pio device monitor -b 115200

# Clean build
uv run pio run -e esp32dev -t clean
```

Build outputs in `.firmware/`:
- `BLFLC_V{version}.bin` - Merged firmware for initial flash
- `BLFLC_V{version}.bin.ota` - OTA update file

## Architecture

### Core Modules (src/blflc/)

| Module | Purpose |
|--------|---------|
| `types.h` | Global structs: `PrinterVariables`, `PrinterConfig`, `LedConfig` |
| `leds.cpp` | FastLED control, `updateleds()` state machine |
| `patterns.cpp` | LED effects: solid, breathing, chase, rainbow, progress |
| `mqttmanager.cpp` | MQTT client (TLS port 8883), FreeRTOS task |
| `mqttparsingutility.cpp` | JSON parsing: gcode_state, stage, HMS errors |
| `web-server.cpp` | AsyncWebServer, WebSocket status updates |
| `filesystem.cpp` | LittleFS persistence (`/blledconfig.json`) |
| `wifi-manager.cpp` | WiFi connection, AP mode fallback |
| `eth-manager.cpp` | Ethernet support for LAN8720A PHY boards |
| `improv-serial.cpp` | Improv WiFi provisioning protocol |

### Web Assets (src/www/)

HTML/CSS/JS compressed at build time by `pre_build.py` into `www.h` as gzip-compressed PROGMEM arrays.

### Key Data Flow

1. MQTT receives printer JSON → `ParseCallback()` extracts state
2. State changes trigger `updateleds()` priority-based handler chain:
   - Maintenance/Test/Disco/Progress Bar modes
   - Error states (HMS fatal/serious, temperature failures)
   - Pause states (stages 16, 30, 34, 35)
   - Stage-specific colors (1, 8, 9, 10, 12, 14)
   - Idle timeout → Running states → Finish indication → LED replication

### P1 Printer Compatibility

P1 printers don't send `gcode_state` in MQTT. The firmware infers state from `mc_percent`:
- 1-99%: Sets `gcodeState = "RUNNING"`
- 100%: Sets `gcodeState = "FINISH"`

### HMS Override System

`overridestage` (default 999 = inactive) can override `stage` for LED behavior. Set by `applyHMSOverride()` when HMS errors are parsed.

## Git Workflow

Always use `--no-gpg-sign` when committing:
```bash
git commit --no-gpg-sign -m "message"
```

## Testing

```bash
# Run printer simulator for testing
python bblp_sim.py
```
