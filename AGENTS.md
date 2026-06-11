# AGENTS.md

## Project Overview

This is an ESP32 PlatformIO project for an automated swing gate controller (Sommer Twist 350).
Only the ESP32 firmware is in scope. Ignore the `librepcb/`, `home-assistant/`, and `enclosure/` directories.

## Specs

Detailed requirements, design, and implementation tasks are in:

- `.kiro/specs/esp32-swing-gate-controller/requirements.md` — functional requirements
- `.kiro/specs/esp32-swing-gate-controller/design.md` — architecture, classes, state machine, wiring
- `.kiro/specs/esp32-swing-gate-controller/tasks.md` — implementation plan with checkboxes

## Source Code

All firmware source files are in `src/`:

- `esp32-swing-gate.ino.cpp` — main sketch (setup/loop)
- `gate.cpp` / `gate.h` — gate state machine and relay control
- `ledmanager.cpp` / `ledmanager.h` — LED status indicators
- `mqttmanager.cpp` / `mqttmanager.h` — MQTT over Ethernet (PubSubClient + W5500)
- `config.h` — compile-time configuration (MQTT, OTA) via build flags

## Build Configuration

- `platformio.ini` — PlatformIO project config with two environments: `esp32` and `esp32_wokwi`
- `private_config.ini` — local build flags (gitignored), copy from `private_config.template.ini`
- `private_config.template.ini` — template for `private_config.ini`

The `esp32` environment requires `private_config.ini` for build flags (`OTA_USERNAME`, `OTA_PASSWORD`, MQTT settings).
The `esp32_wokwi` environment has its own hardcoded build flags for simulation.

## How to Compile

1. Activate the Python virtual environment: `source .venv/bin/activate`
2. Build all environments: `pio run`
3. Build a specific environment: `pio run -e esp32` or `pio run -e esp32_wokwi`

## Key Technical Details

- **Platform**: ESP32 (espressif32 via pioarduino)
- **Framework**: Arduino
- **Python**: 3.13 (see `.python-version`), venv in `.venv/`
- **Board**: esp32dev
- **Serial baud rate**: 115200
