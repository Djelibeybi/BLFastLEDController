# Suggested Commands

## Build Commands

**IMPORTANT**: Always use `uv run pio` for PlatformIO commands.

### Build Firmware

```bash
# Build WiFi firmware (default)
uv run pio run -e esp32dev

# Build Ethernet firmware (Gledopto Elite 2D/4D)
uv run pio run -e esp32_eth_gledopto

# Build Ethernet firmware (IoTorero ESP32 ETH)
uv run pio run -e esp32_eth_iotorero

# Build all environments
uv run pio run
```

### Upload Firmware

```bash
# Upload WiFi firmware
uv run pio run -e esp32dev -t upload

# Upload filesystem (LittleFS)
uv run pio run -e esp32dev -t uploadfs
```

### Monitor Serial Output

```bash
uv run pio device monitor -b 115200
```

### Clean Build

```bash
uv run pio run -e esp32dev -t clean
```

## Build Outputs

Build artifacts are placed in `.firmware/`:
- `BLFLC_V{version}.bin` - Merged firmware for initial flash
- `BLFLC_V{version}.bin.ota` - OTA update file
- `BLFLC-ETH-Gledopto_V{version}.bin` - Ethernet variant (Gledopto)
- `BLFLC-ETH-IoTorero_V{version}.bin` - Ethernet variant (IoTorero)

## Testing

```bash
# Run printer simulator for testing
python bblp_sim.py
```

## Git Commands

**IMPORTANT**: Always use `--no-gpg-sign` when committing.

```bash
# Commit changes
git commit --no-gpg-sign -m "message"

# Check status
git status

# View recent commits
git log --oneline -10
```

## System Utilities (Linux)

```bash
ls          # List directory
cd          # Change directory
grep        # Search in files
find        # Find files
cat         # View file contents
```
