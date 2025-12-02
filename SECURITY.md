# Security Policy

## Supported Versions

Security updates are provided for the following versions:

| Version | Supported          |
| ------- | ------------------ |
| 3.x     | :white_check_mark: |
| < 3.0   | :x:                |

## Security Considerations

The Bambu Lab FastLED Controller firmware handles sensitive information:

- **Printer Access Code**: Used to authenticate MQTT connections to Bambu Lab printers
- **WiFi Credentials**: Stored for network connectivity
- **Network Configuration**: IP addresses and connection details

### Known Limitations

- The web interface does not use HTTPS (hardware constraints)
- Credentials are stored in plaintext on the device filesystem
- The device should only be used on trusted local networks

## Reporting a Vulnerability

If you discover a security vulnerability in this project, please report it responsibly:

### How to Report

1. **Do NOT open a public GitHub issue** for security vulnerabilities
2. **Email the maintainers directly** or use GitHub's private vulnerability reporting feature
3. Include:
   - A description of the vulnerability
   - Steps to reproduce the issue
   - Potential impact assessment
   - Any suggested fixes (optional)

### What to Expect

- **Acknowledgment**: Within 48 hours of your report
- **Initial Assessment**: Within 7 days
- **Resolution Timeline**: Depends on severity and complexity
- **Credit**: You will be credited in the release notes (unless you prefer anonymity)

### Severity Levels

| Severity | Description | Expected Response |
|----------|-------------|-------------------|
| Critical | Remote code execution, credential theft | Immediate patch |
| High | Authentication bypass, data exposure | Patch within 7 days |
| Medium | Information disclosure, DoS | Patch in next release |
| Low | Minor issues, hardening suggestions | Evaluated for future releases |

## Security Best Practices for Users

1. **Use on trusted networks only** - The device communicates over your local network
2. **Keep firmware updated** - Install updates promptly
3. **Use strong WiFi passwords** - Protect your network
4. **Isolate IoT devices** - Consider using a separate VLAN for IoT devices
5. **Monitor for unusual activity** - Check your network logs periodically

## Scope

This security policy applies to:

- The BLFastLEDController firmware
- The web interface served by the device
- Configuration storage and handling

This policy does NOT cover:

- Vulnerabilities in third-party libraries (report these to the library maintainers)
- Issues with Bambu Lab's MQTT protocol or printers
- Network security outside of this project's control

## Acknowledgments

We appreciate the security research community's efforts in identifying and responsibly disclosing vulnerabilities. Contributors who report valid security issues will be acknowledged here (with their permission).