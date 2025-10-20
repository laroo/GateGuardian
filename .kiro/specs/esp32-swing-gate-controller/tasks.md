# Implementation Plan

- [x] 1. Set up project structure and core interfaces





  - Create main ESP32 sketch file with proper includes and setup
  - Define GPIO pin constants and configuration structure
  - Initialize serial communication at 115200 baud for diagnostics
  - _Requirements: 4.1, 5.1_

- [x] 2. Implement Gate class with state machine





  - [x] 2.1 Create Gate class header with state enumeration


    - Define GateState enum (UNKNOWN, CLOSED, OPENING, OPEN, CLOSING)
    - Declare private member variables for state tracking and timing
    - Define public interface methods for gate control and status
    - _Requirements: 2.1, 2.2, 2.3, 2.4_

  - [x] 2.2 Implement gate state machine logic








    - Code state transition logic with 20-second operation timing
    - Implement sensor reading and state determination
    - Add state change logging via serial communication
    - Handle boot-up state detection when sensor reads LOW
    - _Requirements: 2.3, 2.5, 4.2_

  - [x] 2.3 Implement gate control methods


    - Code openGate() and closeGate() methods with relay activation
    - Implement toggle() method for button press handling
    - Add 500ms relay pulse timing with automatic deactivation
    - Prevent multiple activations during gate movement
    - _Requirements: 1.1, 1.2, 1.4_

  - [ ]* 2.4 Write unit tests for Gate class
    - Create test cases for all state transitions
    - Test timing accuracy and edge cases
    - Verify relay activation/deactivation sequences
    - _Requirements: 2.1, 2.2, 2.3, 2.4_

- [x] 3. Implement LED status indicators





  - [x] 3.1 Create LEDManager class


    - Define LED control methods for solid and blinking states
    - Implement timer-based blinking with 500ms intervals
    - Create state-to-LED mapping logic
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5_

  - [x] 3.2 Integrate LED status with gate states








    - Wire LED updates to gate state changes
    - Implement continuous red LED for closed state
    - Implement continuous green LED for open state
    - Add blinking patterns for opening/closing states
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5_

- [x] 4. Implement button input handling





  - [x] 4.1 Add button debouncing and event detection


    - Configure button GPIO with internal pull-up resistor
    - Implement 50ms debounce timing to prevent false triggers
    - Add button press/release event logging
    - _Requirements: 1.3, 4.3, 5.3_

  - [x] 4.2 Connect button events to gate control


    - Wire button press events to gate toggle functionality
    - Ensure button events are logged via serial communication
    - Prevent button actions during relay activation periods
    - _Requirements: 1.1, 1.2, 4.3_

- [x] 5. Implement MQTT communication system





  - [x] 5.1 Set up Ethernet and MQTT client initialization


    - Configure W5500 Ethernet shield with SPI pins
    - Initialize PubSubClient with broker.hivemq.com:1883
    - Generate random client ID for MQTT connection
    - Add connection status monitoring and logging
    - _Requirements: 7.1, 7.2_

  - [x] 5.2 Implement MQTT status publishing


    - Create status message formatting with gate state
    - Implement 10-second periodic publishing timer
    - Add connection retry logic for network failures
    - Include diagnostic logging for publish events
    - _Requirements: 7.3, 4.2_

  - [x] 5.3 Implement MQTT command subscription


    - Subscribe to gate command topic on connection
    - Parse incoming OPEN/CLOSE/TOGGLE commands
    - Wire MQTT commands to gate control methods
    - Add command logging via serial communication
    - _Requirements: 7.4, 4.2_

  - [ ]* 5.4 Write integration tests for MQTT functionality
    - Test MQTT connection and reconnection scenarios
    - Verify message publishing and command handling
    - Test network failure recovery
    - _Requirements: 7.1, 7.2, 7.3, 7.4_

- [x] 6. Implement sensor input and voltage divider interface

  - [x] 6.1 Configure sensor GPIO and voltage divider

    - Set up GPIO 22 for sensor input with proper voltage divider
    - Implement sensor reading with debouncing
    - Add sensor state logging for diagnostics
    - _Requirements: 2.1, 2.2, 5.3_

  - [x] 6.2 Integrate sensor readings with gate state machine

    - Wire sensor readings to state determination logic
    - Implement HIGH=closed, LOW=open/moving logic
    - Add sensor malfunction detection and error handling
    - _Requirements: 2.1, 2.2, 2.5_

- [x] 7. Implement relay control and safety mechanisms

  - [x] 7.1 Create relay control interface

    - Configure relay GPIO pins as outputs
    - Implement relay activation with automatic 500ms timeout
    - Add relay operation logging via serial communication
    - _Requirements: 1.4, 4.4_


  - [ ] 7.2 Add safety and error handling
    - Implement relay timeout protection to prevent motor damage
    - Add error logging for relay failures
    - Ensure safe state operation during error conditions

    - _Requirements: 1.4, 5.4_


- [x] 8. Integrate all components in main loop

  - [ ] 8.1 Create main application loop structure
    - Initialize all components in setup() function
    - Implement main loop with component updates
    - Ensure loop execution completes within 1 second

    - Add initialization logging at startup
    - _Requirements: 4.1, 5.1, 5.2_

  - [ ] 8.2 Wire component interactions and event handling
    - Connect button events to gate control
    - Link gate state changes to LED updates
    - Integrate MQTT commands with gate operations
    - Ensure proper error handling and system stability
    - _Requirements: 1.1, 1.2, 2.3, 3.1, 7.4_

  - [ ]* 8.3 Add comprehensive system testing
    - Test complete gate operation cycles
    - Verify MQTT communication end-to-end
    - Test error recovery and safety mechanisms
    - _Requirements: 5.4, 6.1, 6.2, 6.3_

- [ ] 9. Fix MQTT implementation to use Ethernet instead of WiFi
  - [ ] 9.1 Replace WiFi with Ethernet W5500 shield implementation
    - Remove WiFi dependencies and replace with Ethernet library
    - Configure W5500 SPI pins (CS: GPIO5, MOSI: GPIO23, MISO: GPIO19, SCK: GPIO18)
    - Implement DHCP or static IP configuration for Ethernet
    - Update MQTTManager to use EthernetClient instead of WiFiClient
    - _Requirements: 7.1, 7.2_

  - [ ] 9.2 Test and verify Ethernet MQTT functionality
    - Verify MQTT connection works with Ethernet shield
    - Test status publishing and command subscription over Ethernet
    - Add Ethernet connection status monitoring and logging
    - _Requirements: 7.1, 7.2, 7.3, 7.4_

- [ ] 10. Configure for Wokwi simulation compatibility
  - [ ] 10.1 Ensure Wokwi compilation compatibility
    - Verify all libraries compile for ESP32 target in Wokwi
    - Test GPIO operations in simulation environment
    - Ensure serial output is visible in simulation console
    - _Requirements: 6.1, 6.2, 6.3, 6.4_

  - [ ] 10.2 Add simulation-specific configurations
    - Create Wokwi-compatible pin assignments if needed
    - Add simulation testing scenarios for button and sensor inputs
    - Verify MQTT functionality works in Wokwi environment (may need WiFi fallback for simulation)
    - _Requirements: 6.2, 6.3_

- [ ] 11. Create PCB design and bill of materials
  - [x] 11.1 Design PCB layout with ESP32 and components





    - Create PCB design with ESP32 DevKit v1 at center
    - Integrate W5500 Ethernet module with SPI connections
    - Add 2-channel relay module with optocoupler isolation
    - Include LED indicators, button input, and sensor connections
    - Add power distribution with 5V and 3.3V planes
    - _Requirements: 8.1_

  - [ ] 11.2 Generate bill of materials (BOM)
    - Create comprehensive BOM with part numbers and quantities
    - Include ESP32, W5500 Ethernet module, relays, LEDs, resistors
    - Add connectors, capacitors, protection components
    - Specify safety features like TVS diodes and fuses
    - _Requirements: 8.2_

- [ ] 12. Create Sommer Twist 350 integration manual
  - [ ] 12.1 Create wiring diagram and connection manual
    - Document terminal block connections based on Sommer manual
    - Specify voltage divider circuit for 24V to 3.3V sensor interface
    - Detail relay connections for OPEN/CLOSE commands
    - Include safety considerations and grounding requirements
    - _Requirements: 9.1_

  - [ ] 12.2 Write installation and testing procedures
    - Create step-by-step installation guide
    - Document testing procedures for gate operation
    - Include troubleshooting guide for common issues
    - Add safety warnings and electrical precautions
    - _Requirements: 9.1_