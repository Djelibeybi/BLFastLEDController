# Contributing to Bambu Lab FastLED Controller

Thank you for your interest in contributing to the Bambu Lab FastLED Controller project. This document provides guidelines and information to help you contribute effectively.

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior via GitHub issues or by contacting the maintainers directly.

## How to Contribute

### Reporting Bugs

Before submitting a bug report:

1. **Check existing issues** to see if the problem has already been reported
2. **Update to the latest version** to see if the issue persists
3. **Collect information** about your setup:
   - Firmware version
   - ESP32 board type (WiFi or Ethernet variant)
   - LED chipset and count
   - Bambu Lab printer model (X1, X1C, P1P, P1S)

When submitting a bug report, use the bug report issue template and include:

- A clear, descriptive title
- Steps to reproduce the issue
- Expected behavior vs actual behavior
- Serial monitor output if relevant
- Screenshots of the web interface if applicable

### Suggesting Features

Feature requests are welcome. Please use the feature request issue template and include:

- A clear description of the feature
- The problem it would solve or use case
- Any implementation ideas you have

### Pull Requests

1. **Fork the repository** and create a branch from `main`
2. **Follow the code style** of the existing codebase
3. **Test your changes** with actual hardware when possible
4. **Update documentation** if you're changing functionality
5. **Write clear commit messages** using conventional commit format

#### Development Setup

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/BLFastLEDController.git
cd BLFastLEDController

# Install uv (if not already installed)
curl -LsSf https://astral.sh/uv/install.sh | sh

# Create a virtual environment and install PlatformIO
uv venv
uv pip install platformio

# Build the firmware
uv run pio run -e esp32dev

# Upload to a connected ESP32
uv run pio run -e esp32dev -t upload

# Monitor serial output
uv run pio device monitor -b 115200
```

#### Testing

- Test with the printer simulator: `python bblp_sim.py`
- Test with actual Bambu Lab printers when possible
- Verify web interface functionality
- Check serial output for errors

#### Code Style

- Use consistent indentation (spaces, not tabs)
- Follow existing naming conventions
- Keep functions focused and reasonably sized
- Add comments for complex logic
- Use meaningful variable and function names

### Web Interface Changes

If modifying files in `src/www/`:

1. Edit the HTML/CSS/JS files directly
2. The `pre_build.py` script automatically compresses these into `www.h`
3. Test the web interface thoroughly after changes

## Project Structure

```
BLFastLEDController/
├── src/
│   ├── blflc/          # Core firmware modules
│   │   ├── types.h     # Global data structures
│   │   ├── leds.cpp    # LED control logic
│   │   ├── patterns.cpp # LED animation effects
│   │   ├── mqttmanager.cpp # MQTT client
│   │   └── ...
│   ├── www/            # Web interface assets
│   └── main.cpp        # Entry point
├── lib/                # External libraries
├── include/            # Header files
└── platformio.ini      # PlatformIO configuration
```

## Build Environments

| Environment | Description |
|-------------|-------------|
| `esp32dev` | Standard WiFi-enabled ESP32 boards |
| `esp32_eth_gledopto` | Gledopto Elite 2D/4D Ethernet boards |
| `esp32_eth_iotorero` | IoTorero ESP32 ETH boards |

## License

By contributing to this project, you agree that your contributions will be licensed under the [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)](LICENSE.md) license.

## Questions?

If you have questions about contributing, feel free to:

- Open a discussion on GitHub
- Ask in an existing related issue
- Contact the maintainers

Thank you for helping improve the Bambu Lab FastLED Controller!