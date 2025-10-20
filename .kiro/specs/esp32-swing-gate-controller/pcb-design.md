# PCB Design - ESP32 Swing Gate Controller

## Overview
This document describes the PCB layout design for the ESP32 Swing Gate Controller, featuring the ESP32 DevKit v1 at the center with integrated W5500 Ethernet module, relay controls, LED indicators, and sensor interfaces.

## Board Specifications
- **Board Size**: 100mm x 80mm
- **Layer Count**: 4-layer PCB (recommended for proper power distribution)
- **Thickness**: 1.6mm standard PCB thickness
- **Copper Weight**: 1oz (35µm) for signal layers, 2oz (70µm) for power layers
- **Via Size**: 0.2mm drill, 0.4mm pad
- **Minimum Trace Width**: 0.2mm for signals, 0.5mm for power

## Component Placement Layout

### Central Processing Unit
```
                    ESP32 DevKit v1 (30-pin)
                    ┌─────────────────────┐
                    │  ┌─────────────┐    │
                    │  │   ESP32     │    │
                    │  │  WROOM-32   │    │
                    │  └─────────────┘    │
                    │                     │
                    │ 3V3  GND  D15  D2   │
                    │ EN   D23  D0   D4   │
                    │ VP   D22  D16  D16  │
                    │ VN   D21  D17  D17  │
                    │ D34  D19  D5   D18  │
                    │ D35  D18  D18  D19  │
                    │ D32  D5   D19  D21  │
                    │ D33  D17  D21  D22  │
                    │ D25  D16  D22  D23  │
                    │ D26  D4   D23  GND  │
                    │ D27  D0   GND  3V3  │
                    │ D14  D2   3V3  EN   │
                    │ D12  D15  CMD  VP   │
                    │ D13  D8   SD0  VN   │
                    │ GND  D7   SD1  D34  │
                    └─────────────────────┘
```

### Component Placement Map
```
    ┌─────────────────────────────────────────────────────────────┐
    │  PWR_IN                                            ETH_RJ45 │
    │  ○ ○                                               ┌──────┐ │
    │                                                    │      │ │
    │  LED1(R)  LED2(G)                                  │ RJ45 │ │
    │    ○        ○                                      │      │ │
    │                                                    └──────┘ │
    │                                                             │
    │  ┌─────────────────────┐    ┌──────────────────┐          │
    │  │                     │    │                  │          │
    │  │    ESP32 DevKit     │    │   W5500 Module   │          │
    │  │       v1            │    │                  │          │
    │  │                     │    │                  │          │
    │  └─────────────────────┘    └──────────────────┘          │
    │                                                             │
    │  ┌──────────────────────────────────────────────────────┐  │
    │  │              2-Channel Relay Module                  │  │
    │  │  ┌─────────┐                    ┌─────────┐         │  │
    │  │  │ RELAY1  │                    │ RELAY2  │         │  │
    │  │  │  OPEN   │                    │ CLOSE   │         │  │
    │  │  └─────────┘                    └─────────┘         │  │
    │  └──────────────────────────────────────────────────────┘  │
    │                                                             │
    │  BTN_IN   SENSOR_IN                           GATE_OUT     │
    │   ○ ○       ○ ○                               ○ ○ ○ ○     │
    └─────────────────────────────────────────────────────────────┘
```

## Layer Stack-up (4-Layer PCB)

### Layer 1 - Top Signal Layer
- Component placement and routing
- High-speed digital signals (SPI, GPIO)
- Ethernet differential pairs
- Component silkscreen and solder mask

### Layer 2 - Ground Plane
- Solid ground plane for signal return paths
- Ground pour with thermal reliefs for vias
- Shielding for high-speed signals

### Layer 3 - Power Plane
- 5V power distribution
- 3.3V power distribution
- Power plane splits for different voltage domains

### Layer 4 - Bottom Signal Layer
- Additional routing for complex connections
- Power and ground connections
- Component placement on bottom (if needed)

## Power Distribution Design

### Power Input Section
```
5V_IN ──┬── FUSE(2A) ──┬── 5V_RAIL
        │              │
        └── TVS_DIODE  └── ESP32_VIN
                       │
                       ├── RELAY_MODULE_VCC
                       │
                       └── W5500_VCC
```

### 3.3V Distribution
```
ESP32_3V3 ──┬── 3V3_RAIL
            │
            ├── W5500_3V3 (if needed)
            │
            ├── LED_CIRCUITS
            │
            └── SENSOR_PULLUP
```

## GPIO Pin Assignments and Routing

### ESP32 Pin Mapping
| GPIO | Function | Connection | Notes |
|------|----------|------------|-------|
| GPIO2 | Open Relay | Relay Module IN1 | Output, relay control |
| GPIO0 | Close Relay | Relay Module IN2 | Output, relay control |
| GPIO5 | Ethernet CS | W5500 CS | SPI Chip Select |
| GPIO18 | SPI SCK | W5500 SCK | SPI Clock |
| GPIO19 | SPI MISO | W5500 MISO | SPI Data In |
| GPIO23 | SPI MOSI | W5500 MOSI | SPI Data Out |
| GPIO12 | Red LED | LED1 Cathode | Output, gate closed |
| GPIO14 | Green LED | LED2 Cathode | Output, gate open |
| GPIO13 | Button Input | Button Switch | Input, pull-up |
| GPIO22 | Sensor Input | Voltage Divider | Input, 24V→3.3V |

### SPI Bus Routing (High-Speed)
- **Trace Impedance**: 50Ω single-ended
- **Trace Width**: 0.2mm for 50Ω on 4-layer PCB
- **Via Stitching**: Ground vias every 5mm along traces
- **Length Matching**: ±0.5mm between SPI signals
- **Keep Away**: 3x trace width from other signals

## Component Footprints and Specifications

### ESP32 DevKit v1
- **Footprint**: 30-pin DIP socket or direct solder
- **Pitch**: 2.54mm (0.1")
- **Mounting**: Through-hole pins
- **Clearance**: 5mm around module for components

### W5500 Ethernet Module
- **Footprint**: Custom module footprint
- **Interface**: 2.54mm pin header
- **Placement**: Adjacent to ESP32 for short SPI traces
- **RJ45 Connector**: Integrated on module or separate

### Relay Module
- **Type**: 2-channel relay board
- **Footprint**: Standard relay module footprint
- **Control**: 3.3V/5V compatible inputs
- **Isolation**: Optocoupler isolation built-in
- **Contact Rating**: 10A @ 250VAC

### LED Indicators
- **Type**: 5mm through-hole LEDs
- **Footprint**: Standard LED footprint
- **Current Limiting**: 220Ω series resistors
- **Placement**: Front edge for visibility

## Connector Specifications

### Power Input Connector
```
J1 - Power Input (Barrel Jack)
Pin 1: +5V DC Input
Pin 2: Ground
Center Positive, 2.1mm x 5.5mm
```

### Button Input Connector
```
J2 - Button Input (2-pin screw terminal)
Pin 1: Button Signal (to GPIO13)
Pin 2: Ground
```

### Sensor Input Connector
```
J3 - Sensor Input (2-pin screw terminal)
Pin 1: Sensor Signal (24V from Sommer)
Pin 2: Sensor Ground
```

### Gate Output Connector
```
J4 - Gate Control Output (4-pin screw terminal)
Pin 1: Open Relay NO (Normally Open)
Pin 2: Open Relay COM (Common)
Pin 3: Close Relay NO (Normally Open)
Pin 4: Close Relay COM (Common)
```

### Ethernet Connector
```
J5 - RJ45 Ethernet Connector
Standard 8P8C connector with integrated magnetics
Pins 1,2: TX+ TX- (Transmit pair)
Pins 3,6: RX+ RX- (Receive pair)
Pins 4,5: Not used (10/100 Ethernet)
Pins 7,8: Not used (10/100 Ethernet)
```

## Protection Circuits

### ESD Protection
```
GPIO_PIN ──┬── TVS_DIODE ── GND
           │
           └── SERIES_RESISTOR ── EXTERNAL_CONNECTION
```

### Voltage Divider for 24V Sensor
```
24V_SENSOR ── R1(22kΩ) ──┬── R2(3.3kΩ) ── GND
                          │
                          ├── ZENER(3.3V) ── GND
                          │
                          └── GPIO22
```

### Power Protection
```
5V_IN ── FUSE(2A) ── REVERSE_PROTECTION_DIODE ── 5V_RAIL
```

## Thermal Considerations

### Heat Dissipation
- **ESP32**: Thermal pad connection to ground plane
- **Relay Module**: Adequate spacing for heat dissipation
- **W5500**: Thermal vias under IC if applicable
- **Power Components**: Copper pour for heat spreading

### Component Spacing
- **Minimum Spacing**: 2mm between high-power components
- **Airflow**: Orient components for natural convection
- **Thermal Relief**: Use thermal relief pads for ground connections

## Manufacturing Specifications

### PCB Fabrication
- **Material**: FR4, Tg ≥ 150°C
- **Copper Thickness**: 1oz signal, 2oz power
- **Surface Finish**: HASL or ENIG
- **Solder Mask**: Green, matte finish
- **Silkscreen**: White, component references

### Assembly Notes
- **Component Orientation**: All polarized components clearly marked
- **Test Points**: Accessible test points for key signals
- **Fiducials**: 3 fiducial markers for automated assembly
- **Panelization**: Single board or 2x2 panel for cost efficiency

## Design Rule Check (DRC) Parameters

### Electrical Rules
- **Minimum Trace Width**: 0.15mm (6 mil)
- **Minimum Via Size**: 0.2mm drill, 0.4mm pad
- **Minimum Spacing**: 0.15mm trace-to-trace
- **Annular Ring**: Minimum 0.05mm

### Mechanical Rules
- **Board Edge Clearance**: 0.5mm minimum
- **Component Courtyard**: 0.25mm minimum
- **Drill Sizes**: Standard drill sizes only
- **Slot Tolerances**: ±0.1mm for slots

This PCB design provides a robust platform for the ESP32 Swing Gate Controller with proper power distribution, signal integrity, and protection circuits as required by the specifications.