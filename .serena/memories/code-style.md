# Code Style and Conventions

## Language: C++ (Arduino Framework)

### Naming Conventions

- **Variables**: camelCase (e.g., `printerConfig`, `gcodeState`, `printProgress`)
- **Structs**: PascalCase with `Struct` suffix (e.g., `PrinterVariablesStruct`, `LedConfigStruct`)
- **Type aliases**: PascalCase (e.g., `PrinterVariables`, `LedConfig`)
- **Enums**: PascalCase, values UPPER_SNAKE_CASE (e.g., `LedChipType::CHIP_WS2812B`)
- **Functions**: camelCase (e.g., `setupLeds()`, `updateleds()`, `parseCallback()`)
- **Constants/Macros**: UPPER_SNAKE_CASE (e.g., `CONFIG_ASYNC_TCP_STACK_SIZE`)
- **Global instances**: camelCase (e.g., `printerVariables`, `printerConfig`, `globalVariables`)

### File Organization

- **Header/Source Split**: Each module has separate `.h` and `.cpp` files
- **Headers include guards**: Standard `#ifndef`/`#define`/`#endif` pattern
- **Module location**: Core modules in `src/blflc/`, web assets in `src/www/`

### Type Hints

- Use Arduino types where appropriate (`uint8_t`, `uint16_t`, `uint32_t`)
- Use `String` for Arduino string handling
- Use explicit struct typedefs for configuration objects

### Documentation

- Minimal inline comments; code is expected to be self-explanatory
- Important logic documented with brief comments
- No formal docstring format (not a Doxygen project)

### Patterns

- **Global singletons**: Configuration structs declared `extern` in headers, defined in types.cpp
- **FreeRTOS tasks**: Used for MQTT client (non-blocking)
- **State machine**: LED control uses priority-based handler chain in `updateleds()`
- **WebSocket**: Real-time status updates to web UI

### Build Flags

Version defined via build flags in platformio.ini:
- `-DVERSION=${this.custom_version}` (numeric)
- `-DSTRVERSION=\""${this.custom_version}"\"` (string)
- `-DUSE_ETHERNET` (for Ethernet variants)

### Web Assets

- HTML/CSS/JS are compressed at build time by `pre_build.py`
- Stored as gzip-compressed PROGMEM arrays in `www.h`
- Served with appropriate MIME types

## Python Scripts

- Used for build tooling only (`pre_build.py`, `merge_firmware.py`, `bblp_sim.py`)
- Standard Python 3.11+ style
- Minimal dependencies
