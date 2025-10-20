# Manufacturing and Assembly Guide - ESP32 Swing Gate Controller

## Overview

This document provides comprehensive manufacturing and assembly instructions for the ESP32 Swing Gate Controller PCB. It includes fabrication specifications, component sourcing, assembly procedures, and quality control guidelines.

## PCB Fabrication Specifications

### Board Parameters
- **Board Dimensions**: 100mm × 80mm ± 0.1mm
- **Board Thickness**: 1.6mm ± 0.1mm
- **Layer Count**: 4 layers
- **Material**: FR4, Tg ≥ 150°C
- **Copper Weight**: 1oz (35µm) inner/outer layers, 2oz (70µm) power layers

### Drill Specifications
- **Finished Hole Tolerance**: ±0.05mm
- **Via Drill Size**: 0.2mm ± 0.02mm
- **Via Pad Size**: 0.4mm ± 0.02mm
- **Component Holes**: Per component specifications
- **Mounting Holes**: 3.2mm ± 0.05mm

### Surface Finish Options
1. **HASL (Hot Air Solder Leveling)** - Standard, cost-effective
2. **ENIG (Electroless Nickel Immersion Gold)** - Premium, better for fine pitch
3. **OSP (Organic Solderability Preservative)** - Lead-free alternative

### Solder Mask and Silkscreen
- **Solder Mask**: Green, matte finish, 0.025mm thickness
- **Solder Mask Clearance**: 0.05mm from pads
- **Silkscreen**: White, component references and polarity marks
- **Minimum Text Size**: 0.8mm height, 0.12mm line width

### Quality Standards
- **IPC Class**: IPC-A-600 Class 2 (Industrial)
- **Testing**: 100% electrical test, visual inspection
- **Impedance Control**: 50Ω ± 10% for single-ended traces
- **Copper Thickness Variation**: ±20%

## Bill of Materials (BOM)

### Main Components

| Reference | Description | Manufacturer | Part Number | Package | Qty |
|-----------|-------------|--------------|-------------|---------|-----|
| U1 | ESP32 DevKit v1 | Espressif | ESP32-WROOM-32 | Module | 1 |
| U2 | W5500 Ethernet Module | WIZnet | W5500-EVB-Pico | Module | 1 |
| U3 | 2-Channel Relay Module | Generic | SRD-05VDC-SL-C | Module | 1 |
| J1 | Barrel Jack Connector | CUI | PJ-002AH | THT | 1 |
| J2,J3 | Screw Terminal 2-pin | Phoenix | 1935161 | THT | 2 |
| J4 | Screw Terminal 4-pin | Phoenix | 1935174 | THT | 1 |
| J5 | RJ45 Connector | Amphenol | 54602-908LF | THT | 1 |

### Passive Components

| Reference | Description | Value | Package | Tolerance | Qty |
|-----------|-------------|-------|---------|-----------|-----|
| R1, R2 | Resistor | 220Ω | 0805 | ±5% | 2 |
| R3 | Resistor | 10kΩ | 0805 | ±5% | 1 |
| R4 | Resistor | 22kΩ | 0805 | ±1% | 1 |
| R5 | Resistor | 3.3kΩ | 0805 | ±1% | 1 |
| C1 | Electrolytic Capacitor | 470µF/16V | Radial | ±20% | 1 |
| C2, C3 | Ceramic Capacitor | 100nF/50V | 0805 | ±10% | 2 |
| D1 | Rectifier Diode | 1N4007 | DO-41 | - | 1 |
| D2 | LED Red | 5mm | THT | - | 1 |
| D3 | LED Green | 5mm | THT | - | 1 |
| D4 | Zener Diode | 3.3V/1W | DO-41 | ±5% | 1 |
| F1 | Fuse | 2A Fast | 5×20mm | - | 1 |
| TVS1, TVS2 | TVS Diode | SMBJ5.0A | SMB | - | 2 |

### Hardware and Mechanical

| Description | Specification | Quantity | Notes |
|-------------|---------------|----------|-------|
| PCB Standoffs | M3 × 5mm | 4 | Nylon or metal |
| Mounting Screws | M3 × 8mm | 4 | Phillips head |
| Enclosure | IP65 rated | 1 | 120×100×40mm min |
| Cable Glands | PG7 | 4 | For external cables |
| Heat Shrink Tubing | 3mm, 5mm | 1m each | Wire protection |

## Component Sourcing Guidelines

### Preferred Suppliers
1. **Electronic Components**: Digi-Key, Mouser, Arrow
2. **ESP32 Modules**: Espressif direct, authorized distributors
3. **Mechanical Parts**: McMaster-Carr, Fastenal
4. **PCB Fabrication**: JLCPCB, PCBWay, Advanced Circuits

### Quality Requirements
- **Components**: Genuine parts only, no counterfeit
- **Lead-Free**: RoHS compliant components required
- **Temperature Rating**: -40°C to +85°C minimum
- **Moisture Sensitivity**: MSL 3 or better for SMD components

### Inventory Management
- **Minimum Order Quantities**: Consider MOQs for cost optimization
- **Lead Times**: Plan for 2-4 weeks for specialty components
- **Excess Inventory**: 10% overage recommended for production
- **Obsolescence**: Monitor component lifecycle status

## Assembly Procedures

### Pre-Assembly Preparation

#### PCB Inspection
1. **Visual Inspection**: Check for defects, scratches, contamination
2. **Dimensional Check**: Verify board dimensions and hole sizes
3. **Electrical Test**: Continuity and isolation testing
4. **Cleanliness**: Ensure PCB is clean and dry

#### Component Preparation
1. **Inventory Check**: Verify all components against BOM
2. **Component Testing**: Test critical components if required
3. **ESD Protection**: Use proper ESD handling procedures
4. **Component Orientation**: Pre-sort polarized components

### Assembly Sequence

#### Step 1: SMD Component Placement
1. **Solder Paste Application**: Use stencil for consistent application
2. **Component Placement**: Use pick-and-place machine or manual placement
3. **Reflow Soldering**: Follow recommended temperature profile
4. **Inspection**: Visual and AOI (Automated Optical Inspection)

#### Step 2: Through-Hole Component Assembly
1. **Component Insertion**: Insert components per assembly drawing
2. **Wave Soldering**: Or selective soldering for mixed assemblies
3. **Manual Soldering**: For components not suitable for wave soldering
4. **Cleaning**: Remove flux residue if required

#### Step 3: Module Integration
1. **ESP32 DevKit Installation**: Socket or direct solder
2. **W5500 Module**: Verify SPI connections
3. **Relay Module**: Check isolation and control signals
4. **Connector Installation**: Ensure proper alignment

### Soldering Guidelines

#### SMD Soldering (Reflow)
- **Peak Temperature**: 245°C ± 5°C
- **Time Above Liquidus**: 60-90 seconds
- **Ramp Rate**: 1-3°C/second
- **Cooling Rate**: <6°C/second

#### Through-Hole Soldering
- **Iron Temperature**: 350°C ± 25°C
- **Solder Type**: 63/37 or SAC305 lead-free
- **Joint Quality**: IPC-A-610 Class 2 standards
- **Flux**: No-clean or water-soluble

#### Manual Soldering Best Practices
1. **Clean Iron Tip**: Use damp sponge regularly
2. **Proper Flux**: Apply flux to joints as needed
3. **Heat Management**: Avoid overheating components
4. **Joint Inspection**: Check for cold joints, bridges

## Quality Control Procedures

### In-Process Testing

#### Electrical Testing
1. **Power-On Test**: Verify power supply voltages
2. **Continuity Test**: Check critical signal paths
3. **Isolation Test**: Verify relay isolation >1MΩ
4. **Functional Test**: Basic I/O operation

#### Visual Inspection Checklist
- [ ] Component placement and orientation
- [ ] Solder joint quality and completeness
- [ ] No solder bridges or cold joints
- [ ] Proper component values installed
- [ ] Connector alignment and seating
- [ ] PCB cleanliness and damage

### Final Testing Protocol

#### Automated Test Equipment (ATE)
1. **In-Circuit Test (ICT)**: Component values and connections
2. **Functional Test**: Full system operation
3. **Boundary Scan**: Digital circuit verification
4. **Programming**: ESP32 firmware upload

#### Manual Test Procedures
1. **Power Supply Test**: Measure all voltage rails
2. **Ethernet Test**: Network connectivity verification
3. **Relay Test**: Operation and isolation testing
4. **Sensor Interface**: Voltage divider verification
5. **LED Test**: Visual indicator functionality

### Test Documentation
- **Test Records**: Maintain for each assembled unit
- **Failure Analysis**: Document and track defects
- **Calibration**: Ensure test equipment calibration
- **Traceability**: Serial number tracking system

## Packaging and Shipping

### Protective Packaging
1. **Anti-Static Bags**: For individual PCBs
2. **Foam Padding**: Prevent mechanical damage
3. **Moisture Barrier**: Desiccant packs if required
4. **Labeling**: Clear identification and handling instructions

### Documentation Package
- Assembly drawings and schematics
- Test certificates and inspection reports
- User manual and installation guide
- Warranty and support information

### Shipping Considerations
- **ESD Protection**: Maintain throughout shipping
- **Temperature Control**: Avoid extreme temperatures
- **Handling Instructions**: Fragile electronics labeling
- **Insurance**: Appropriate coverage for value

## Troubleshooting Common Issues

### Assembly Problems

#### Solder Joint Issues
- **Cold Joints**: Increase temperature or time
- **Solder Bridges**: Improve paste application, use flux
- **Insufficient Solder**: Check paste volume, stencil condition
- **Component Tombstoning**: Balance thermal mass, adjust profile

#### Component Issues
- **Wrong Values**: Improve component verification process
- **Polarity Errors**: Enhance visual marking and training
- **Missing Components**: Implement pick-and-place verification
- **Damaged Components**: Improve handling procedures

### Testing Failures

#### Power Supply Issues
- **No 5V**: Check input fuse, reverse protection diode
- **No 3.3V**: Verify ESP32 module, connections
- **Voltage Ripple**: Check filter capacitors, grounding

#### Communication Problems
- **No Ethernet**: Verify W5500 connections, magnetics
- **SPI Issues**: Check clock, data lines, chip select
- **MQTT Failures**: Verify network configuration, broker settings

## Continuous Improvement

### Process Optimization
- **Yield Tracking**: Monitor and improve assembly yield
- **Cycle Time**: Optimize assembly sequence and tooling
- **Cost Reduction**: Evaluate component alternatives, processes
- **Automation**: Identify opportunities for automated assembly

### Quality Enhancement
- **Defect Analysis**: Root cause analysis of failures
- **Process Control**: Statistical process control implementation
- **Training**: Continuous operator skill development
- **Supplier Quality**: Vendor qualification and monitoring

### Documentation Updates
- **Revision Control**: Maintain current assembly procedures
- **Lessons Learned**: Incorporate feedback into procedures
- **Best Practices**: Share successful techniques across teams
- **Standards Compliance**: Keep current with industry standards

---

**Document Version**: 1.0  
**Last Updated**: October 4, 2024  
**Approved By**: Manufacturing Engineering  
**Next Review**: January 4, 2025