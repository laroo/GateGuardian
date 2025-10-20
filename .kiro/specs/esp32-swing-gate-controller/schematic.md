# Schematic Design - ESP32 Swing Gate Controller

## Circuit Schematic Overview

This document provides the detailed schematic design for the ESP32 Swing Gate Controller, showing all electrical connections, component values, and circuit topology.

## Main Schematic Diagram

```
                    ESP32 Swing Gate Controller Schematic
                    =====================================

Power Input Section:
                    
    +5V_IN ──●── F1(2A) ──●── D1 ──●── +5V_RAIL
             │             │        │
             │             │        ├── C1(470µF) ── GND
             │             │        │
             │             │        └── ESP32_VIN
             │             │
             │             └── TVS1(5V) ── GND
             │
             └── GND_IN ────────────────── GND_RAIL

ESP32 DevKit v1 Connections:

                        ┌─────────────────────┐
                        │    ESP32 DevKit     │
                        │                     │
                    VIN │●                   ●│ 3V3 ── +3V3_RAIL
                    GND │●                   ●│ EN
                    RST │●                   ●│ VP
                     VP │●                   ●│ VN
                     VN │●                   ●│ D34
                    D34 │●                   ●│ D35
                    D35 │●                   ●│ D32
                    D32 │●                   ●│ D33
                    D33 │●                   ●│ D25
                    D25 │●                   ●│ D26
                    D26 │●                   ●│ D27
                    D27 │●                   ●│ D14
                    D14 │●                   ●│ D12 ── LED1_RED
                    D12 │●                   ●│ D13 ── BUTTON_IN
                    D13 │●                   ●│ GND
                    GND │●                   ●│ VIN
                    VIN │●                   ●│ 3V3
                    3V3 │●                   ●│ EN
                     EN │●                   ●│ VP
                     VP │●                   ●│ VN
                     VN │●                   ●│ D34
                    D22 │●                   ●│ D35  
                    D21 │●                   ●│ D32
                    D19 │●                   ●│ D33
                    D18 │●                   ●│ D25
                     D5 │●                   ●│ D26
                    D17 │●                   ●│ D27
                    D16 │●                   ●│ D14 ── LED2_GREEN
                     D4 │●                   ●│ D12
                     D0 │●                   ●│ D13
                     D2 │●                   ●│ GND
                    D15 │●                   ●│ VIN
                     D8 │●                   ●│ 3V3
                     D7 │●                   ●│ EN
                    GND │●                   ●│ VP
                        └─────────────────────┘
                              │  │  │  │
                              │  │  │  └── D23 ── W5500_MOSI
                              │  │  └───── D22 ── SENSOR_IN
                              │  └──────── D19 ── W5500_MISO
                              └─────────── D18 ── W5500_SCK
                                          D5  ── W5500_CS
                                          D2  ── RELAY1_OPEN
                                          D0  ── RELAY2_CLOSE

W5500 Ethernet Module:

    +5V_RAIL ──●── +5V
               │
               ├── C2(100nF) ── GND
               │
    ESP32_D5  ──── CS
    ESP32_D18 ──── SCK
    ESP32_D19 ──── MISO
    ESP32_D23 ──── MOSI
               │
               └── GND ── GND_RAIL

                    ┌─────────────┐
                    │   RJ45      │
                    │  Connector  │
                    │             │
                    │ 1 ●─────────│── TX+
                    │ 2 ●─────────│── TX-
                    │ 3 ●─────────│── RX+
                    │ 4 ●         │
                    │ 5 ●         │
                    │ 6 ●─────────│── RX-
                    │ 7 ●         │
                    │ 8 ●         │
                    └─────────────┘

LED Indicator Circuits:

    +3V3_RAIL ──●── R1(220Ω) ──●── LED1_RED_ANODE
                │               │
                │               └── LED1_RED_CATHODE ── ESP32_D12
                │
                └── R2(220Ω) ──●── LED2_GREEN_ANODE
                                │
                                └── LED2_GREEN_CATHODE ── ESP32_D14

Button Input Circuit:

    +3V3_RAIL ──●── R3(10kΩ) ──●── ESP32_D13
                │               │
                │               └── SW1 ── GND_RAIL
                │
                └── C3(100nF) ── GND_RAIL

Sensor Input Circuit (24V to 3.3V):

    SENSOR_24V ──●── R4(22kΩ) ──●── R5(3.3kΩ) ── GND_RAIL
                 │               │
                 │               ├── D2(3.3V_ZENER) ── GND_RAIL
                 │               │
                 │               └── ESP32_D22
                 │
                 └── TVS2(30V) ── GND_RAIL

2-Channel Relay Module:

    +5V_RAIL ──●── RELAY_MODULE_VCC
               │
    ESP32_D2 ──●── RELAY1_IN (Open Gate)
               │
    ESP32_D0 ──●── RELAY2_IN (Close Gate)
               │
    GND_RAIL ──●── RELAY_MODULE_GND

                    Relay 1 (Open Gate):
                    ┌─────────────┐
                    │     NO      │── GATE_OPEN_OUT
                    │             │
                    │    COM      │── GATE_COMMON
                    │             │
                    │     NC      │── (Not Connected)
                    └─────────────┘

                    Relay 2 (Close Gate):
                    ┌─────────────┐
                    │     NO      │── GATE_CLOSE_OUT
                    │             │
                    │    COM      │── GATE_COMMON
                    │             │
                    │     NC      │── (Not Connected)
                    └─────────────┘

External Connectors:

    J1 - Power Input (Barrel Jack):
    ┌─────┐
    │  ●  │── +5V_IN
    │  ●  │── GND_IN
    └─────┘

    J2 - Button Input (Screw Terminal):
    ┌─────┐
    │  1  │── BUTTON_SIGNAL
    │  2  │── GND
    └─────┘

    J3 - Sensor Input (Screw Terminal):
    ┌─────┐
    │  1  │── SENSOR_24V
    │  2  │── SENSOR_GND
    └─────┘

    J4 - Gate Control Output (Screw Terminal):
    ┌─────┐
    │  1  │── GATE_OPEN_OUT (Relay1 NO)
    │  2  │── GATE_COMMON (Relay1&2 COM)
    │  3  │── GATE_CLOSE_OUT (Relay2 NO)
    │  4  │── GATE_COMMON (Relay1&2 COM)
    └─────┘

    J5 - Ethernet (RJ45):
    Standard RJ45 connector with integrated magnetics
```

## Component Specifications

### Power Components
- **F1**: 2A Fast-blow fuse, 5x20mm
- **D1**: 1N4007 Rectifier diode (Reverse protection)
- **TVS1**: SMBJ5.0A TVS diode (5V protection)
- **C1**: 470µF/16V Electrolytic capacitor
- **C2**: 100nF/50V Ceramic capacitor

### LED Circuit Components
- **LED1**: 5mm Red LED, 2V forward voltage
- **LED2**: 5mm Green LED, 2.2V forward voltage
- **R1, R2**: 220Ω ±5% 1/4W resistors (LED current limiting)

### Input Circuit Components
- **R3**: 10kΩ ±5% 1/4W resistor (Button pull-up)
- **C3**: 100nF/50V Ceramic capacitor (Button debounce)
- **SW1**: Momentary push button, SPST NO

### Sensor Interface Components
- **R4**: 22kΩ ±1% 1/4W resistor (Voltage divider high side)
- **R5**: 3.3kΩ ±1% 1/4W resistor (Voltage divider low side)
- **D2**: 3.3V Zener diode, 1W (Overvoltage protection)
- **TVS2**: SMBJ30A TVS diode (30V protection)

### Relay Module Specifications
- **Type**: 2-Channel 5V Relay Module
- **Coil Voltage**: 5V DC
- **Coil Current**: 70mA per relay
- **Contact Rating**: 10A @ 250VAC, 10A @ 30VDC
- **Isolation**: Optocoupler isolation (>2500V)
- **Control Logic**: Active HIGH (3.3V/5V compatible)

## Circuit Analysis

### Power Budget
| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 DevKit | 5V | 250mA | 1.25W |
| W5500 Module | 5V | 150mA | 0.75W |
| Relay Module | 5V | 140mA | 0.70W |
| LED Indicators | 3.3V | 30mA | 0.10W |
| **Total** | **5V** | **570mA** | **2.80W** |

### Voltage Divider Calculation
For 24V to 3.3V conversion:
- **Input**: 24V (Sommer sensor output)
- **Output**: 3.3V max (ESP32 GPIO tolerance)
- **R4**: 22kΩ (high side)
- **R5**: 3.3kΩ (low side)
- **Ratio**: 3.3kΩ / (22kΩ + 3.3kΩ) = 0.13
- **Output Voltage**: 24V × 0.13 = 3.12V ✓

### LED Current Calculation
For 3.3V supply with 220Ω resistor:
- **Red LED**: (3.3V - 2.0V) / 220Ω = 5.9mA ✓
- **Green LED**: (3.3V - 2.2V) / 220Ω = 5.0mA ✓

## Safety Features

### Electrical Isolation
- **Relay Contacts**: Galvanic isolation between control and power circuits
- **Optocouplers**: Built into relay module for 2500V isolation
- **Ground Separation**: Separate grounds for control and power circuits

### Protection Circuits
- **Overvoltage Protection**: TVS diodes on all external connections
- **Reverse Polarity**: Diode protection on power input
- **Overcurrent Protection**: 2A fuse on main power input
- **ESD Protection**: TVS diodes on GPIO connections

### Fail-Safe Operation
- **Relay Default State**: Normally open contacts (fail-safe)
- **Watchdog Timer**: ESP32 built-in watchdog for system recovery
- **Power-On Reset**: Proper reset circuitry for clean startup

## Testing Points

### Debug Access Points
- **TP1**: +5V Rail
- **TP2**: +3.3V Rail
- **TP3**: ESP32 Reset
- **TP4**: SPI Clock (D18)
- **TP5**: MQTT Status LED
- **TP6**: Sensor Input (after voltage divider)
- **TP7**: Button Input
- **TP8**: Ground Reference

### Signal Monitoring
- **Oscilloscope Probes**: Accessible test points for SPI signals
- **Multimeter Access**: Voltage measurement points
- **Logic Analyzer**: GPIO header for debugging

This schematic provides a complete electrical design for the ESP32 Swing Gate Controller with proper protection, isolation, and interface circuits as specified in the requirements.