# Gate Guardian


![GitHub Forks](https://img.shields.io/github/forks/laroo/GateGuardian?style=flat-square&color=blue)
![GitHub Stars](https://img.shields.io/github/stars/laroo/GateGuardian?style=flat-square&color=yellow)
![LibrePCB Powered](https://img.shields.io/badge/Powered%20By-LibrePCB-red?style=flat-square)  

**Automating the Sommer Twist 350 Swing Gate with ESP32 and MQTT**

This repository contains the design and code for Gate Guardian, an ESP32 swing gate controller designed to operate and monitor a Sommer Twist 350 swing gate. The system integrates remote control via MQTT over Ethernet, real-time status monitoring, and visual feedback. The design emphasizes reliability, safety, and ease of installation.

## Changelog

v1.1
- Changed pins for status LEDs as these were input only:
  - Red LED: GPIO 17 (from GPIO 39)
  - Green LED: GPIO 5 (from GPIO 36)
  - Photo eye: GPIO 36 (from GPIO 5)

v1.0
- Initial release

## MQTT Configuration

### MQTT Topics

```
gateguardian/state = CLOSED
gateguardian/sensor/lock = 0
gateguardian/sensor/lights = 0
gateguardian/sensor/photo_eye = 0
gateguardian/sensor/external_relay = 0
gateguardian/client_id = esp32_gate_FFAA00
gateguardian/uptime = 1510
```

### MQTT Commands

```
gateguardian/command = OPEN
gateguardian/command = CLOSE
gateguardian/command = STOP
```

## Home Assistant Configuration

The Home Assistant configuration can be found [here](home-assistant/configuration.yaml).

![GateGuardian_HomeAssistant](GateGuardian_HomeAssistant.png)

## Schematics and PCB

Used LibrePCB to design the schematics and PCB.

![Schematic](GateGuardian_Schematics_v1.1.png)

![PCB](GateGuardian_PCB_v1.1.png)

Gerber files for the PCB can be found [here](librepcb/output/v1.1/gerber).

# Input/Output

| Pin | Direction | Function | Note |
|----|----------|----------|------|
| GPIO 12 | Output | Gate Close Relay | Pulse to close the gate |
| GPIO 14 | Output | Gate Stop Relay | Pulse to stop the gate |
| GPIO 15 | Output | Gate Open Relay | Pulse to open the gate |
| GPIO 17 | Output | Red LED | Blinking during closing, solid during opening |
| GPIO 5 | Output | Green LED | Blinking during opening, solid during closing |
| GPIO 33 | Input | Gate Lights | Blinking warning light when opening or cloing |
| GPIO 32 | Input | Gate Lock | Enabled when gate is closed |
| GPIO 35 | Input | External Relay | Triggered when gate is closed |
| GPIO 36 | Input | Photo eye | When someone/omething goes through the gate |
| GPIO 4 | Input | Sensor 1 | Optional sensor (dht22 temperature sensor) |
| GPIO 2 | Input | Sensor 2 | Optional sensor (not used) |

## Code

The firmware is built on the Arduino framework using PlatformIO and implements a robust gate control system with the following key features:

- **MQTT Integration**: Remote control and status monitoring via MQTT over Ethernet (W5500)
- **Visual Feedback**: Dual LED system (red/green) indicating gate state with blinking patterns during movement
- **Position Sensing**: Real-time gate position detection using magnetic sensors
- **Safety Features**: Relay pulse timing, state machine logic, and operation timeouts
- **Diagnostics**: Serial output at 115200 baud for debugging and monitoring


### Platform IO Setup

```
pyenv local 3.13
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Build

```
pio run
```

### Upload (Serial)

```
pio run --target upload -v
```

### OTA (Over-the-Air) Update

The firmware includes [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA) for wireless firmware updates over the network.

1. Open a browser and navigate to `http://<device-ip>/update`
2. Authenticate with the configured `OTA_USERNAME` and `OTA_PASSWORD`
3. Select the firmware binary (`.pio/build/esp32/firmware.bin`) and upload

OTA credentials are set as compile-time build flags in `private_config.ini`:

```ini
'-D OTA_USERNAME=youruser'
'-D OTA_PASSWORD=yourpassword'
```

### Web Interface

The device runs an HTTP server on port 80 with the following endpoints:

| Endpoint | Description |
|----------|-------------|
| `/` | Device identification |
| `/gate/open` | Command gate to open |
| `/gate/close` | Command gate to close |
| `/gate/stop` | Command gate to stop |
| `/update` | ElegantOTA firmware update page |
