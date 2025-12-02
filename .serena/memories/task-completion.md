# Task Completion Checklist

## After Making Code Changes

### 1. Build the Firmware

Always verify the build succeeds after changes:

```bash
# Build WiFi firmware
uv run pio run -e esp32dev

# If changes affect Ethernet builds, also build:
uv run pio run -e esp32_eth_gledopto
uv run pio run -e esp32_eth_iotorero
```

### 2. Check for Compiler Warnings

Review build output for warnings that may indicate issues.

### 3. Web UI Changes

If you modified files in `src/www/`:
- The `pre_build.py` script runs automatically during build
- Verify `www.h` is regenerated with your changes
- Test the web UI in a browser

### 4. Git Commit Guidelines

When committing:

```bash
# Always use --no-gpg-sign
git commit --no-gpg-sign -m "type(scope): description"
```

Commit message format:
- `feat(scope):` - New feature
- `fix(scope):` - Bug fix
- `refactor(scope):` - Code restructure
- `docs(scope):` - Documentation
- `ci(scope):` - CI/CD changes
- `build(scope):` - Build system changes

Common scopes: `led`, `mqtt`, `web`, `wifi`, `eth`, `improv`, `config`

### 5. Testing

For functional testing:

```bash
# Run printer simulator
python bblp_sim.py
```

Then:
- Flash firmware to ESP32
- Connect to device via web UI or serial monitor
- Verify expected behavior

### 6. Version Bump (for releases)

Update version in `platformio.ini`:
```ini
custom_version = X.Y.Z
```

Update in ALL environment sections:
- `[env:esp32dev]`
- `[env:esp32_eth_gledopto]`
- `[env:esp32_eth_iotorero]`

## No Automated Tests

This project does not have automated unit tests or linting. Manual testing is required:
- Build succeeds without errors
- Web UI functions correctly
- LED behavior matches expected state machine
- MQTT connection works with simulator or real printer
