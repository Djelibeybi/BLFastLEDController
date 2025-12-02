# Architecture

## Directory Structure

```
BLFastLEDController/
├── src/
│   ├── main.cpp              # Entry point (setup/loop)
│   ├── blflc/                # Core firmware modules
│   │   ├── types.h/cpp       # Global data structures
│   │   ├── leds.h/cpp        # LED control, state machine
│   │   ├── patterns.h/cpp    # LED effects implementation
│   │   ├── mqttmanager.h/cpp # MQTT client (FreeRTOS task)
│   │   ├── mqttparsingutility.h/cpp # JSON parsing
│   │   ├── web-server.h/cpp  # AsyncWebServer, WebSocket
│   │   ├── filesystem.h/cpp  # LittleFS persistence
│   │   ├── wifi-manager.h/cpp # WiFi connection/AP mode
│   │   ├── eth-manager.h/cpp # Ethernet support
│   │   ├── improv.h/cpp      # Improv WiFi types
│   │   ├── improv-serial.h/cpp # Improv serial protocol
│   │   ├── ssdp.h/cpp        # SSDP discovery
│   │   ├── bblprinterdiscovery.h/cpp # Printer mDNS discovery
│   │   ├── logserial.h/cpp   # Serial logging
│   │   └── autogrowbufferstream.h/cpp # Buffer utilities
│   └── www/                  # Web UI assets
│       ├── *.html            # Configuration pages
│       ├── style.css         # Stylesheet
│       ├── particleCanvas.js # Background animation
│       ├── blflc.svg         # Logo
│       ├── favicon.png       # Favicon
│       └── www.h             # Auto-generated (gzip compressed)
├── platformio.ini            # Build configuration
├── pre_build.py              # Compresses www/ into www.h
├── merge_firmware.py         # Creates merged .bin files
├── bblp_sim.py               # Printer MQTT simulator
└── .firmware/                # Build output directory
```

## Core Data Structures (types.h)

### PrinterVariables
Runtime printer state: gcodeState, stage, printProgress, HMS errors, online status

### PrinterConfig
User configuration: printer IP/credentials, LED colors, patterns, brightness, error detection settings

### LedConfig
Hardware config: chipType, colorOrder, ledCount, dataPin, clockPin, RGBW placement

### GlobalVariables
System state: WiFi SSID/password, hostname, firmware version

## Key Data Flow

1. **MQTT → State**: `mqttmanager.cpp` connects to printer, receives JSON via FreeRTOS task
2. **JSON → Variables**: `mqttparsingutility.cpp` parses gcode_state, stage, HMS codes
3. **State → LEDs**: `leds.cpp:updateleds()` priority-based handler chain determines LED output
4. **Patterns → Display**: `patterns.cpp` implements visual effects (solid, breathing, chase, rainbow, progress)
5. **Web → Config**: `web-server.cpp` serves UI and handles configuration changes via WebSocket

## LED State Machine Priority (leds.cpp:updateleds)

1. Maintenance/Test/Disco/Progress Bar modes (special modes)
2. Error states (HMS fatal/serious, temperature failures)
3. Pause states (stages 16, 30, 34, 35)
4. Stage-specific colors (1, 8, 9, 10, 12, 14)
5. Idle timeout handling
6. Running states
7. Finish indication
8. LED replication (mirror printer chamber light)

## P1 Printer Compatibility

P1 printers don't send `gcode_state` in MQTT. Firmware infers state from `mc_percent`:
- 1-99%: Sets `gcodeState = "RUNNING"`
- 100%: Sets `gcodeState = "FINISH"`

## HMS Override System

`overridestage` (default 999 = inactive) overrides `stage` for LED behavior.
Set by `applyHMSOverride()` when HMS errors are parsed.

## Build Process

1. `pre_build.py` runs first: compresses `src/www/*.html|css|js|svg|png` → `src/www/www.h`
2. PlatformIO compiles C++ sources
3. `merge_firmware.py` creates combined firmware binary for initial flash
