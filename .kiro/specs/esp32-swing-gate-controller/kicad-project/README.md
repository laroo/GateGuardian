# ESP32 Swing Gate Controller - PCB Design

## Project Overview

This KiCad project contains the complete PCB design for the ESP32 Swing Gate Controller, a comprehensive embedded system for controlling and monitoring automated swing gates, specifically designed for integration with the Sommer Twist 350 gate system.

## Project Files

### KiCad Design Files
- **esp32-gate-controller.pro** - KiCad project file
- **esp32-gate-controller.sch** - Schematic design
- **esp32-gate-controller.kicad_pcb** - PCB layout design
- **README.md** - This documentation file

### Supporting Documentation
- **../pcb-design.md** - Detailed PCB design specifications
- **../schematic.md** - Complete schematic documentation
- **../design.md** - System design document
- **../requirements.md** - Project requirements

## Board Specifications

### Physical Characteristics
- **Board Size**: 100mm × 80mm
- **Layer Count**: 4-layer PCB
- **Thickness**: 1.6mm standard
- **Material**: FR4, Tg ≥ 150°C
- **Surface Finish**: HASL or ENIG

### Electrical Specifications
- **Supply Voltage**: 5V DC (2.1mm barrel jack)
- **Power Consumption**: ~2.8W maximum
- **Operating Temperature**: -10°C to +60°C
- **Humidity**: 5% to 95% non-condensing

## Component Layout

### Main Components
1. **ESP32 DevKit v1** (U1) - Central processing unit at board center
2. **W5500 Ethernet Module** (U2) - Network connectivity
3. **2-Channel Relay Module** (U3) - Gate control relays
4. **Power Input** (J1) - 5V DC barrel jack connector
5. **Ethernet Port** (J5) - RJ45 connector for network
6. **I/O Connectors** (J2-J4) - Screw terminals for field connections

### Indicators and Controls
- **LED1** (D2) - Red LED for gate closed status
- **LED2** (D3) - Green LED for gate open status
- **Status LEDs** - Power and network activity indicators

### Protection Circuits
- **Fuse** (F1) - 2A overcurrent protection
- **TVS Diodes** - ESD protection on all I/O
- **Voltage Divider** - 24V to 3.3V sensor interface
- **Zener Diode** - Overvoltage protection

## Pin Assignments

### ESP32 GPIO Mapping
| GPIO | Function | Connection | Direction |
|------|----------|------------|-----------|
| GPIO2 | Open Relay | Relay Module IN1 | Output |
| GPIO0 | Close Relay | Relay Module IN2 | Output |
| GPIO5 | Ethernet CS | W5500 CS | Output |
| GPIO18 | SPI SCK | W5500 SCK | Output |
| GPIO19 | SPI MISO | W5500 MISO | Input |
| GPIO23 | SPI MOSI | W5500 MOSI | Output |
| GPIO12 | Red LED | LED1 Control | Output |
| GPIO14 | Green LED | LED2 Control | Output |
| GPIO13 | Button Input | Button Switch | Input |
| GPIO22 | Sensor Input | Gate Position | Input |

### External Connectors

#### J1 - Power Input (Barrel Jack)
- Pin 1: +5V DC Input
- Pin 2: Ground
- Specification: 2.1mm × 5.5mm, center positive

#### J2 - Button Input (2-pin Screw Terminal)
- Pin 1: Button Signal
- Pin 2: Ground
- Wire Gauge: 18-24 AWG

#### J3 - Sensor Input (2-pin Screw Terminal)
- Pin 1: 24V Sensor Signal (from Sommer)
- Pin 2: Sensor Ground
- Wire Gauge: 18-24 AWG

#### J4 - Gate Control Output (4-pin Screw Terminal)
- Pin 1: Open Relay NO (Normally Open)
- Pin 2: Open Relay COM (Common)
- Pin 3: Close Relay NO (Normally Open)
- Pin 4: Close Relay COM (Common)
- Wire Gauge: 14-18 AWG (gate control circuits)

#### J5 - Ethernet (RJ45)
- Standard 8P8C connector
- Integrated magnetics
- 10/100 Mbps Ethernet

## Layer Stack-up

### 4-Layer Configuration
1. **Layer 1 (F.Cu)** - Top signal layer
   - Component placement and routing
   - High-speed digital signals
   - Silkscreen and solder mask

2. **Layer 2 (In1.Cu)** - Ground plane
   - Solid ground pour
   - Signal return paths
   - EMI shielding

3. **Layer 3 (In2.Cu)** - Power plane
   - +5V power distribution
   - +3.3V power distribution
   - Power plane splits

4. **Layer 4 (B.Cu)** - Bottom signal layer
   - Additional routing
   - Power connections
   - Component placement (if needed)

## Design Rules

### Electrical Rules
- **Minimum Trace Width**: 0.2mm (8 mil)
- **Minimum Via Size**: 0.4mm pad, 0.2mm drill
- **Minimum Spacing**: 0.2mm trace-to-trace
- **Power Trace Width**: 0.5mm minimum
- **High-Speed Traces**: 0.2mm for 50Ω impedance

### Mechanical Rules
- **Board Edge Clearance**: 0.5mm minimum
- **Component Courtyard**: 0.25mm minimum
- **Mounting Holes**: 3.2mm for M3 screws
- **Drill Tolerances**: ±0.05mm

## Manufacturing Notes

### PCB Fabrication
- **Copper Weight**: 1oz signal layers, 2oz power layers
- **Via Fill**: Tented vias preferred
- **Solder Mask**: Green, matte finish
- **Silkscreen**: White, component references
- **Panelization**: Single board or 2×2 panel

### Assembly Requirements
- **Component Orientation**: All polarized components marked
- **Test Points**: Accessible for debugging
- **Fiducials**: 3 markers for automated assembly
- **Lead-Free**: RoHS compliant components

## Testing and Validation

### Electrical Testing
- **Power-On Test**: Verify 5V and 3.3V rails
- **Continuity Test**: Check all connections
- **Isolation Test**: Verify relay isolation
- **Signal Integrity**: Test SPI communication

### Functional Testing
- **ESP32 Programming**: Upload test firmware
- **Ethernet Connectivity**: Verify network connection
- **Relay Operation**: Test gate control functions
- **Sensor Interface**: Verify 24V to 3.3V conversion

### Safety Testing
- **Insulation Resistance**: >1MΩ between isolated circuits
- **Dielectric Strength**: 1500V AC for 1 minute
- **Ground Continuity**: <0.1Ω resistance
- **Overcurrent Protection**: Verify fuse operation

## Installation Guidelines

### Mounting
- Use M3 screws with 5mm standoffs
- Mount in IP65 rated enclosure
- Ensure adequate ventilation
- Protect from direct moisture

### Wiring
- Use appropriate wire gauges per connector specifications
- Implement proper strain relief
- Follow local electrical codes
- Test all connections before power-on

### Configuration
- Program ESP32 with gate controller firmware
- Configure network settings (DHCP/Static IP)
- Set MQTT broker parameters
- Test gate operation cycles

## Troubleshooting

### Common Issues
1. **No Power**: Check fuse, power supply, connections
2. **No Network**: Verify Ethernet cable, W5500 module
3. **Relay Not Operating**: Check GPIO connections, relay module power
4. **Sensor Issues**: Verify voltage divider, 24V input

### Debug Points
- **TP1**: +5V rail voltage
- **TP2**: +3.3V rail voltage
- **TP3**: ESP32 reset signal
- **TP4**: SPI clock signal
- **TP5**: Sensor input (after voltage divider)

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2024-10-04 | Initial PCB design |
| | | - ESP32 DevKit v1 integration |
| | | - W5500 Ethernet module |
| | | - 2-channel relay control |
| | | - Protection circuits |
| | | - 4-layer PCB layout |

## Support and Documentation

For additional information, refer to:
- System requirements document
- Software implementation guide
- Sommer Twist 350 integration manual
- Component datasheets and specifications

## License and Compliance

This design is provided for educational and development purposes. Ensure compliance with local electrical codes and safety regulations before deployment in production environments.

---

**Project**: ESP32 Swing Gate Controller  
**Designer**: ESP32 Gate Controller Project  
**Date**: October 4, 2024  
**Version**: 1.0