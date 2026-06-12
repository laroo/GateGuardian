# Requirements Document

## Introduction

The ESP32 Swing Gate Controller is an embedded system that manages the operation of an automated swing gate: Sommer Twist 350. The system provides manual control via a button, monitors gate position through sensors, controls gate movement via relays, and provides visual feedback through LEDs. The controller ensures safe and reliable gate operation with proper state management and timing controls.

## Requirements

### Requirement 1

**User Story:** As a property owner, I want to control my swing gate with a button press, so that I can conveniently open and close the gate without manual operation.

#### Acceptance Criteria

1. WHEN the button is pressed AND the gate is closed THEN the system SHALL activate the gate opening relay for 500ms
2. WHEN the button is pressed AND the gate is open THEN the system SHALL activate the gate closing relay for 500ms
3. WHEN the button is pressed THEN the system SHALL prevent multiple activations until the button is released
4. WHEN a relay is activated THEN the system SHALL automatically deactivate it after 500ms to prevent motor damage

### Requirement 2

**User Story:** As a property owner, I want to know the current status of my gate, so that I can understand whether it's open, closed, or in motion.

#### Acceptance Criteria

1. WHEN the gate sensor reads HIGH THEN the system SHALL determine the gate is closed
2. WHEN the gate sensor reads LOW THEN the system SHALL determine the gate is open, opening, or closing
3. WHEN the gate state changes from closed to not-closed THEN the system SHALL update the gate status accordingly. It will take around 20 seconds for the gate to fully open. Closed state is instant when the sensor reads HIGH.
4. WHEN the gate reaches a stable position THEN the system SHALL update the status to either GATE_OPEN or GATE_CLOSED
5. WHEN system boots and the gate sensor reads LOW THEN assume the gate is opening or closing. Wait 20 seconds to determine the new state: HIGH = gate is closed, LOW = gate is open
6. WHEN the gate is manually closed or closed by external factors THEN the system SHALL detect the sensor change from LOW to HIGH and immediately update the state to GATE_CLOSED regardless of the previous state
7. WHEN the gate state is unknown AND a button press occurs THEN the system SHALL determine the current state based on the sensor reading and operate the gate accordingly (HIGH sensor = treat as closed and open, LOW sensor = treat as open and close)

### Requirement 3

**User Story:** As a property owner, I want visual indicators of my gate's status, so that I can see the gate state from a distance.

#### Acceptance Criteria

1. WHEN the gate is closed THEN the system SHALL illuminate the red LED continuously
2. WHEN the gate is open THEN the system SHALL illuminate the green LED continuously
3. WHEN the gate is opening THEN the system SHALL blink the green LED
4. WHEN the gate is closing THEN the system SHALL blink the red LED
5. WHEN the gate status is unknown THEN the system SHALL blink both LEDs


### Requirement 4

**User Story:** As a system administrator, I want the gate controller to provide diagnostic information, so that I can troubleshoot issues and monitor system health.

#### Acceptance Criteria

1. WHEN the system starts up THEN it SHALL output initialization messages via serial communication at 115200 baud
2. WHEN the gate state changes THEN the system SHALL log the state transition via serial communication
3. WHEN button presses are detected THEN the system SHALL log the button events via serial communication
4. WHEN relay operations occur THEN the system SHALL log the relay activation and deactivation via serial communication

### Requirement 5

**User Story:** As a property owner, I want the gate controller to operate reliably in various conditions, so that my gate system remains functional over time.

#### Acceptance Criteria

1. WHEN the system is powered on THEN it SHALL initialize all GPIO pins to their correct modes within 2 seconds
2. WHEN the main loop executes THEN it SHALL complete each cycle within 1 second to ensure responsive operation
3. WHEN sensor readings are taken THEN the system SHALL debounce inputs to prevent false triggers
4. WHEN the system encounters an error condition THEN it SHALL continue operating in a safe state without crashing

### Requirement 6

**User Story:** As a developer, I want the gate controller to be testable in a simulation environment, so that I can develop and debug the system without physical hardware.

#### Acceptance Criteria

1. WHEN the code is compiled for Wokwi simulation THEN it SHALL build successfully with the ESP32 target
2. WHEN running in Wokwi THEN all GPIO operations SHALL be compatible with the simulation environment
3. WHEN simulated THEN the system SHALL respond to virtual button presses and sensor inputs
4. WHEN debugging THEN serial output SHALL be visible in the simulation console


### Requirement 7

**User Story:** As a property owner, I want to publish the gate state using MQTT using an ethernet shield connected to the ESP32

#### Acceptance Criteria

1. Use library PubSubClient https://github.com/knolleary/pubsubclient to connect to a MQTT server
2. Make the MQTT broker configurable, use broker.hivemq.com:1883 with a random client ID for testing.
2. Publish a topic with the current gate state every 10 seconds
3. Subscribe a topic and listen for a gate open or close value

### Requirement 8

**User Story:** As a property owner, I want the ESP32 gate controller to boot safely after a power outage, so that the Sommer Twist 350 gate motor is not locked up by spurious relay signals during ESP32 startup.

#### Background

During a power outage both the Sommer Twist 350 and the ESP32 boot simultaneously. The ESP32 GPIO pins used for relay control can float HIGH during startup, causing the relay module to activate and send unintended open/close signals to the Sommer motor controller. This leaves the Sommer in an unresponsive state where it ignores all direct (non-ESP32) gate commands. The current workaround is to manually press the ESP32 reset button.

#### Acceptance Criteria

1. WHEN the ESP32 boots THEN the system SHALL wait a configurable delay (default 15 seconds) before performing normal initialization, to allow the Sommer Twist 350 to fully boot first
2. WHEN the boot delay is active THEN the relay GPIO pins SHALL be held LOW to prevent any spurious signals to the gate motor
3. WHEN the boot delay completes THEN the system SHALL perform a software reset via `ESP.restart()` to ensure a clean initialization with stable GPIO states
4. WHEN the boot delay is active THEN the system SHALL blink both LEDs in a distinct pattern to indicate the boot-delay state
5. WHEN the boot delay is active THEN the system SHALL output serial messages indicating the boot delay countdown
6. The boot delay duration SHALL be a compile-time constant configurable in `config.h`

### Requirement 9

**User Story:** As a property owner, I want to access the gate controller from a webpage, so that I can control the gate and update the firmware remotely without physical access to the ESP32.

#### Acceptance Criteria

1. WHEN the ESP32 has a network connection THEN the system SHALL run an HTTP web server on port 80
2. WHEN a user accesses the root endpoint (`/`) THEN the system SHALL return an HTML page that serves as the GateGuardian dashboard
3. WHEN a user accesses `/gate/open` THEN the system SHALL command the gate to open and return a confirmation response
4. WHEN a user accesses `/gate/close` THEN the system SHALL command the gate to close and return a confirmation response
5. WHEN a user accesses `/gate/stop` THEN the system SHALL command the gate to stop and return a confirmation response
6. WHEN a user accesses the ElegantOTA endpoint THEN the system SHALL provide an over-the-air firmware update interface
7. The OTA update SHALL require authentication via configurable `OTA_USERNAME` and `OTA_PASSWORD` compile-time constants in `config.h`
8. The web server and OTA handler SHALL be updated in the main loop to process incoming requests
9. WHEN a user accesses `/status` THEN the system SHALL return a JSON response containing `clientId`, `uptime` (in seconds), and `gateState` (from `getStateString()`)
10. The homepage dashboard SHALL display the `clientId`, `uptime` (formatted as HH:MM:SS), and current gate state, updated every second without a full page reload
11. The homepage SHALL use htmx (loaded from CDN) to poll `/status` every second and update the displayed values in place
12. The homepage SHALL include "Open" and "Close" buttons that each trigger their respective `/gate/open` and `/gate/close` endpoints using htmx without a full page reload
13. The homepage SHALL use Pico CSS (loaded from CDN) for styling, providing a clean and minimal layout

### Requirement 10

**User Story:** As a developer, I want to run automated unit tests for the Gate state machine on a host machine, so that I can verify all gate state transitions and sensor interactions without physical hardware.

#### Background

The `Gate` class uses `millis()` for all timing decisions and calls `_readSensorLock()`, `_readSensorLights()`, `_readSensorPhotoEye()`, and `_readSensorExternalRelay()` to obtain hardware sensor values. Both must be injectable in tests so that a complete gate operation sequence (e.g. boot → open command → 20 s travel → OPEN state) can be replayed at arbitrary speed without real hardware or real time delays.

#### Acceptance Criteria

1. The `Gate` class SHALL expose its four `_readSensor*()` methods and its time source as `virtual protected`, so that a test subclass can override them to inject controlled values without modifying production logic.

2. A `GateTestHarness` subclass of `Gate` SHALL be provided in the test directory that:
   - Overrides `_readSensorLock()`, `_readSensorLights()`, `_readSensorPhotoEye()`, and `_readSensorExternalRelay()` to return values from the currently active sequence row
   - Overrides the time source (`_millis()`) to return a simulated timestamp instead of the real `millis()`

3. Gate operation sequences SHALL be defined in CSV files with the following format:
   - One header row: `timestamp_ms,sensorLock,sensorLights,sensorPhotoEye,sensorExternalRelay,expectedState`
   - Each data row specifies sensor values (0 = off / false, 1 = on / true) that are active **from** `timestamp_ms` until the next row's timestamp
   - `expectedState` is the gate state string (`UNKNOWN`, `CLOSED`, `OPENING`, `OPEN`, `CLOSING`) that `getStateString()` SHALL return at that timestamp; an empty value means no assertion is made at that row
   - CSV files SHALL be stored in `test/sequences/`

4. The test runner SHALL, for each sequence file:
   - Parse the CSV into an ordered list of rows
   - For each row, advance the simulated clock to `timestamp_ms`, set the sensor values from that row, and call `gate.update()`
   - After calling `update()`, WHEN `expectedState` is non-empty THEN assert that `gate.getStateString()` equals `expectedState`
   - Report each failed assertion with the sequence filename, the row's `timestamp_ms`, the expected state, and the actual state

5. The test suite SHALL include sequence files covering at minimum the following scenarios:
   - **boot_closed**: Gate sensor reads lock=closed at t=0 → gate reaches `CLOSED`
   - **boot_open**: Gate sensor reads lock=open at t=0, stable after 20 s → gate reaches `OPEN`
   - **open_cycle**: Gate starts `CLOSED`, open command at t=100 ms, sensor stays open for 20 s → gate reaches `OPEN`
   - **close_cycle**: Gate starts `OPEN`, close command at t=100 ms, sensor goes closed after 20 s → gate reaches `CLOSED`
   - **manual_close**: Gate is `OPEN`, lock sensor goes HIGH without a close command → gate transitions immediately to `CLOSED`

6. The tests SHALL be compiled and run in a PlatformIO `native` environment so they execute on the host machine without any ESP32 hardware or toolchain.

7. WHEN all sequence assertions pass THEN the test run SHALL exit with code 0; WHEN any assertion fails THEN it SHALL exit with a non-zero code and print a summary of all failures.

### Requirement 11

**User Story:** As a developer, I want the ESP32 to publish live sensor and state snapshots to a dedicated MQTT topic in the same CSV row format used by the unit test sequences, so that I can capture real gate behaviour and turn it directly into a new test file.

#### Acceptance Criteria

1. WHEN the compile-time flag `MQTT_SEQUENCE_RECORD` is defined THEN the system SHALL publish a sequence record row after every `publishStatus()` call; WHEN the flag is not defined the feature SHALL be compiled out entirely.

2. The sequence record SHALL be published to the topic `gateguardian/sequence` as a single CSV-formatted payload with the columns `timestamp_ms,sensorLock,sensorLights,sensorPhotoEye,sensorExternalRelay,state` (no header row, values match the unit test CSV format: 0/1 for sensors, state string for state).

3. `timestamp_ms` in the published payload SHALL be the value of `millis()` at the time of publishing.

4. The `MQTT_SEQUENCE_RECORD` build flag SHALL be documented in `private_config.template.ini` as an optional flag and in `config.h` with a default of disabled.
