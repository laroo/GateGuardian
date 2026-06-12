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

  - [x] 5.2 Implement MQTT status publishing


    - Create status message formatting with gate state
    - Implement 10-second periodic publishing timer
    - Add connection retry logic for network failures
    - Include diagnostic logging for publish events

  - [x] 5.3 Implement MQTT command subscription


    - Subscribe to gate command topic on connection
    - Parse incoming OPEN/CLOSE/TOGGLE commands
    - Wire MQTT commands to gate control methods
    - Add command logging via serial communication

  - [ ]* 5.4 Write integration tests for MQTT functionality
    - Test MQTT connection and reconnection scenarios
    - Verify message publishing and command handling
    - Test network failure recovery

- [x] 6. Implement boot delay and software reset for power outage safety

  - [x] 6.1 Add boot delay compile-time constant
    - Add `BOOT_DELAY_MS` constant to `config.h` with default value of 15000 (15 seconds)
    - _Requirements: 8.1, 8.6_

  - [x] 6.2 Implement boot delay logic in `setup()`
    - At the very start of `setup()`, before any other initialization, force relay pins LOW with `pinMode` + `digitalWrite`
    - Initialize serial and print boot delay countdown messages (once per second)
    - Blink both LEDs in a distinct pattern during the delay to indicate boot-delay state
    - After the delay completes, call `ESP.restart()` to perform a clean reboot
    - Use a flag (e.g. RTC memory or a boot-reason check) to distinguish first boot from the software-reset boot, so the delay only runs once
    - _Requirements: 8.1, 8.2, 8.3, 8.4, 8.5_

- [x] 7. Implement web server and OTA updates

  - [x] 7.1 Set up HTTP web server on port 80
    - Initialize `WebServer` on port 80
    - Register root endpoint (`/`) returning a GateGuardian identification response
    - _Requirements: 9.1, 9.2_

  - [x] 7.2 Implement gate control endpoints
    - Register `/gate/open` endpoint to command gate open
    - Register `/gate/close` endpoint to command gate close
    - Register `/gate/stop` endpoint to command gate stop
    - Each endpoint returns a text confirmation response
    - _Requirements: 9.3, 9.4, 9.5_

  - [x] 7.3 Integrate ElegantOTA for firmware updates
    - Initialize ElegantOTA with the web server instance via `ElegantOTA.begin(&server)`
    - Configure OTA authentication via `OTA_USERNAME` and `OTA_PASSWORD` compile-time constants in `config.h`
    - Call `ElegantOTA.loop()` in the main loop
    - _Requirements: 9.6, 9.7_

  - [x] 7.4 Handle web requests in main loop
    - Call `server.handleClient()` and `ElegantOTA.loop()` when network connection is active
    - _Requirements: 9.8_
