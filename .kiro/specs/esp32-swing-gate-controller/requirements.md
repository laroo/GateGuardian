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
