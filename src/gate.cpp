/**
 * Gate.cpp - ESP32 Swing Gate Controller Implementation
 * 
 * Basic implementation stub for the Gate class.
 * This provides the foundation structure that will be expanded
 * in subsequent tasks.
 */

#include "Arduino.h"
#include "gate.h"

// ============================================================================
// GPIO PIN DEFINITIONS (extern declarations from header)
// ============================================================================
const int PIN_LED_GATE_CLOSED = 12;  // Red LED: closed (blink = closing)
const int PIN_LED_GATE_OPEN = 14;    // Green LED: open (blink = opening)
const int PIN_RELAY_GATE_CLOSE = 0;  // Close relay control
const int PIN_RELAY_GATE_OPEN = 2;   // Open relay control
const int PIN_SENSOR_GATE_OPEN = 22; // Gate position sensor
const int PIN_BUTTON = 13;           // Manual control button

// ============================================================================
// GATE CLASS IMPLEMENTATION
// ============================================================================

Gate::Gate() : 
    _currentState(GATE_UNKNOWN),
    _previousState(GATE_UNKNOWN),
    _sensorState(false),
    _previousSensorState(false),
    _lastStateChange(0),
    _lastSensorRead(0),
    _relayActivationTime(0),
    _relayActive(false),
    _initialized(false)
{
    Serial.println("[GATE] Gate controller constructor called");
}

Gate::~Gate() {
    Serial.println("[GATE] Gate controller destructor called");
}

void Gate::initialize() {
    Serial.println("[GATE] Initializing gate controller...");
    
    // Note: GPIO pins are already configured in main setup()
    // Read initial sensor state and determine boot-up state
    _sensorState = _readSensor();
    _previousSensorState = _sensorState;
    
    _initialized = true;
    _lastStateChange = millis();
    _lastSensorRead = millis();
    
    // Handle boot-up state detection
    _handleBootupState();
    
    Serial.println("[GATE] Gate controller initialized successfully");
}

void Gate::update() {
    if (!_initialized) return;
    
    // Tick timers first
    _stateTimer.tick();
    _relayTimer.tick();
    
    // Read sensor state with debouncing
    unsigned long currentTime = millis();
    if (currentTime - _lastSensorRead >= 50) { // 50ms debounce
        bool newSensorState = _readSensor();
        if (newSensorState != _previousSensorState) {
            _sensorState = newSensorState;
            _previousSensorState = newSensorState;
            _lastSensorRead = currentTime;
            
            Serial.print("[SENSOR] Sensor state changed to: ");
            Serial.println(_sensorState ? "HIGH (closed)" : "LOW (open/moving)");
        }
    }
    
    // Safety check: ensure relay is deactivated after 500ms even if timer fails
    if (_relayActive && (currentTime - _relayActivationTime >= 500)) {
        Serial.println("[SAFETY] Relay timeout - forcing deactivation");
        _deactivateRelays();
    }
    
    // State machine logic
    switch (_currentState) {
        case GATE_UNKNOWN:
            // Determine initial state based on sensor
            if (_sensorState) {
                // Sensor HIGH - gate is closed (instant detection)
                _updateGateState(GATE_CLOSED);
            } else {
                // Sensor is LOW - could be open, opening, or closing
                // Wait for 20 seconds to determine stable state
                if (currentTime - _lastStateChange >= 20000) {
                    if (_sensorState) {
                        // Sensor HIGH after 20s - gate is closed (instant)
                        _updateGateState(GATE_CLOSED);
                    } else {
                        // Sensor LOW after 20s - gate is open
                        _updateGateState(GATE_OPEN);
                    }
                }
            }
            break;
            
        case GATE_CLOSED:
            // Gate is closed - sensor should be HIGH
            if (!_sensorState) {
                // Sensor went LOW - gate is no longer closed
                // Since we were closed, assume opening
                _updateGateState(GATE_OPENING);
            }
            break;
            
        case GATE_OPENING:
            // Check for instant closed state detection (sensor HIGH)
            if (_sensorState) {
                // Sensor HIGH - gate is closed (instant detection per Requirement 2.3)
                _updateGateState(GATE_CLOSED);
            } else if (currentTime - _lastStateChange >= 20000) {
                // Gate takes ~20 seconds to fully open (Requirement 2.3)
                // Sensor LOW after 20s - gate is now fully open
                _updateGateState(GATE_OPEN);
            }
            break;
            
        case GATE_OPEN:
            // Gate is open - sensor should be LOW
            if (_sensorState) {
                // Sensor HIGH - gate is closed (instant detection per Requirement 2.3 & 2.6)
                // This handles manual closure or external factors closing the gate
                _updateGateState(GATE_CLOSED);
            }
            break;
            
        case GATE_CLOSING:
            // Check for instant closed state detection (sensor HIGH)
            if (_sensorState) {
                // Sensor HIGH - gate is closed (instant detection per Requirement 2.3)
                _updateGateState(GATE_CLOSED);
            } else if (currentTime - _lastStateChange >= 20000) {
                // Gate closing - check if 20 seconds elapsed
                // Sensor LOW after 20s - gate is still open (operation failed)
                _updateGateState(GATE_OPEN);
            }
            break;
    }
}

void Gate::toggle() {
    Serial.println("[BUTTON] Button pressed - Gate command: TOGGLE");
    
    if (!_initialized) {
        Serial.println("[ERROR] Gate not initialized, ignoring toggle command");
        return;
    }
    
    // Prevent multiple activations during gate movement
    if (isMoving()) {
        Serial.println("[GATE] Gate is moving, ignoring toggle command");
        return;
    }
    
    // Prevent activation while relay is active
    if (_relayActive) {
        Serial.println("[GATE] Relay is active, ignoring toggle command");
        return;
    }
    
    switch (_currentState) {
        case GATE_CLOSED:
            openGate();
            break;
        case GATE_OPEN:
            closeGate();
            break;
        case GATE_UNKNOWN:
            Serial.println("[GATE] Gate state unknown - attempting to determine state and operate");
            // In unknown state, try to determine current state based on sensor and operate accordingly
            if (_sensorState) {
                // Sensor HIGH - gate is closed, so open it
                Serial.println("[GATE] Sensor HIGH in unknown state - treating as closed, opening gate");
                _updateGateState(GATE_CLOSED);
                openGate();
            } else {
                // Sensor LOW - gate is likely open, so close it
                Serial.println("[GATE] Sensor LOW in unknown state - treating as open, closing gate");
                _updateGateState(GATE_OPEN);
                closeGate();
            }
            break;
        default:
            Serial.println("[GATE] Gate is moving, toggle ignored");
            break;
    }
}

void Gate::openGate() {
    Serial.println("[BUTTON] Button pressed - Gate command: OPEN");
    
    if (!_initialized) {
        Serial.println("[ERROR] Gate not initialized, ignoring open command");
        return;
    }
    
    // Only allow opening if gate is closed
    if (_currentState != GATE_CLOSED) {
        Serial.println("[GATE] Gate is not closed, cannot open");
        return;
    }
    
    // Prevent activation while relay is active
    if (_relayActive) {
        Serial.println("[GATE] Relay is active, ignoring open command");
        return;
    }
    
    // Activate open relay for 500ms
    _activateRelay(PIN_RELAY_GATE_OPEN, "Open");
    
    // Update state to opening
    _updateGateState(GATE_OPENING);
}

void Gate::closeGate() {
    Serial.println("[BUTTON] Button pressed - Gate command: CLOSE");
    
    if (!_initialized) {
        Serial.println("[ERROR] Gate not initialized, ignoring close command");
        return;
    }
    
    // Only allow closing if gate is open
    if (_currentState != GATE_OPEN) {
        Serial.println("[GATE] Gate is not open, cannot close");
        return;
    }
    
    // Prevent activation while relay is active
    if (_relayActive) {
        Serial.println("[GATE] Relay is active, ignoring close command");
        return;
    }
    
    // Activate close relay for 500ms
    _activateRelay(PIN_RELAY_GATE_CLOSE, "Close");
    
    // Update state to closing
    _updateGateState(GATE_CLOSING);
}

GateState Gate::getState() const {
    return _currentState;
}

bool Gate::isMoving() const {
    return (_currentState == GATE_OPENING || _currentState == GATE_CLOSING);
}

bool Gate::isRelayActive() const {
    return _relayActive;
}

String Gate::getStateString() const {
    switch (_currentState) {
        case GATE_UNKNOWN:  return "UNKNOWN";
        case GATE_CLOSED:   return "CLOSED";
        case GATE_OPENING:  return "OPENING";
        case GATE_OPEN:     return "OPEN";
        case GATE_CLOSING:  return "CLOSING";
        default:            return "INVALID";
    }
}

// ============================================================================
// PRIVATE METHODS (Stubs for future implementation)
// ============================================================================

void Gate::_updateGateState(GateState newState) {
    if (_currentState != newState) {
        // Validate state transition
        if (!_isValidStateTransition(_currentState, newState)) {
            Serial.print("[ERROR] Invalid state transition attempted: ");
            Serial.print(getStateString());
            Serial.print(" -> ");
            
            // Temporarily set state to get new state string
            GateState temp = _currentState;
            _currentState = newState;
            Serial.println(getStateString());
            _currentState = temp;
            
            return; // Don't change state if transition is invalid
        }
        
        _logStateChange(_currentState, newState);
        _previousState = _currentState;
        _currentState = newState;
        _lastStateChange = millis();
    }
}

bool Gate::_readSensor() {
    // Read sensor state - HIGH when gate is closed, LOW when open/moving
    return digitalRead(PIN_SENSOR_GATE_OPEN);
}

void Gate::_activateRelay(int relayPin, const char* relayName) {
    if (_relayActive) {
        Serial.println("[ERROR] Relay already active, cannot activate another");
        return;
    }
    
    // Activate the relay
    digitalWrite(relayPin, HIGH);
    _relayActive = true;
    _relayActivationTime = millis();
    
    Serial.print("[RELAY] ");
    Serial.print(relayName);
    Serial.println(" relay activated");
    
    // Set up timer to deactivate relay after 500ms
    _relayTimer.in(500, [](void* gate) -> bool {
        static_cast<Gate*>(gate)->_deactivateRelays();
        return false; // Don't repeat
    }, this);
}

void Gate::_deactivateRelays() {
    // Deactivate both relays to be safe
    digitalWrite(PIN_RELAY_GATE_OPEN, LOW);
    digitalWrite(PIN_RELAY_GATE_CLOSE, LOW);
    
    if (_relayActive) {
        unsigned long activeDuration = millis() - _relayActivationTime;
        Serial.print("[RELAY] Relay deactivated after ");
        Serial.print(activeDuration);
        Serial.println("ms");
    }
    
    _relayActive = false;
    _relayActivationTime = 0;
}

void Gate::_logStateChange(GateState oldState, GateState newState) {
    Serial.print("[STATE] Gate state changed: ");
    
    // Print old state
    switch (oldState) {
        case GATE_UNKNOWN:  Serial.print("UNKNOWN"); break;
        case GATE_CLOSED:   Serial.print("CLOSED"); break;
        case GATE_OPENING:  Serial.print("OPENING"); break;
        case GATE_OPEN:     Serial.print("OPEN"); break;
        case GATE_CLOSING:  Serial.print("CLOSING"); break;
        default:            Serial.print("INVALID"); break;
    }
    
    Serial.print(" -> ");
    
    // Print new state
    switch (newState) {
        case GATE_UNKNOWN:  Serial.println("UNKNOWN"); break;
        case GATE_CLOSED:   Serial.println("CLOSED"); break;
        case GATE_OPENING:  Serial.println("OPENING"); break;
        case GATE_OPEN:     Serial.println("OPEN"); break;
        case GATE_CLOSING:  Serial.println("CLOSING"); break;
        default:            Serial.println("INVALID"); break;
    }
}

bool Gate::_isValidStateTransition(GateState from, GateState to) {
    // Define valid state transitions (Requirement 2.6: Allow manual/external gate closure from any state)
    switch (from) {
        case GATE_UNKNOWN:
            return (to == GATE_CLOSED || to == GATE_OPEN || to == GATE_OPENING || to == GATE_CLOSING);
            
        case GATE_CLOSED:
            return (to == GATE_OPENING || to == GATE_UNKNOWN);
            
        case GATE_OPENING:
            return (to == GATE_OPEN || to == GATE_CLOSED || to == GATE_UNKNOWN);
            
        case GATE_OPEN:
            return (to == GATE_CLOSING || to == GATE_CLOSED || to == GATE_UNKNOWN);
            
        case GATE_CLOSING:
            return (to == GATE_CLOSED || to == GATE_OPEN || to == GATE_UNKNOWN);
            
        default:
            return false;
    }
}

void Gate::_handleBootupState() {
    Serial.println("[GATE] Handling bootup state detection");
    
    if (_sensorState) {
        // Sensor HIGH - gate is closed
        _updateGateState(GATE_CLOSED);
        Serial.println("[GATE] Boot-up: Gate detected as CLOSED (sensor HIGH)");
    } else {
        // Sensor LOW - gate could be open, opening, or closing
        // Set to UNKNOWN and let the state machine determine after 20 seconds
        _updateGateState(GATE_UNKNOWN);
        Serial.println("[GATE] Boot-up: Gate sensor LOW - waiting 20 seconds to determine state");
    }
}
