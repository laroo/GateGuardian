/**
 * Gate.h - ESP32 Swing Gate Controller Header
 * 
 * Defines the Gate class interface and related enumerations
 * for controlling and monitoring a Sommer Twist 350 swing gate.
 */

#ifndef Gate_h
#define Gate_h

#include "Arduino.h"
#include <arduino-timer.h>

// ============================================================================
// GPIO PIN DEFINITIONS
// ============================================================================
// LED Indicators
extern const int PIN_LED_GATE_CLOSED;  // Red LED: closed (blink = closing)
extern const int PIN_LED_GATE_OPEN;    // Green LED: open (blink = opening)

// Relay Controls
extern const int PIN_RELAY_GATE_CLOSE; // Close relay control
extern const int PIN_RELAY_GATE_OPEN;  // Open relay control

// Sensor Input
extern const int PIN_SENSOR_GATE_OPEN;  // High when gate is closed; Low when gate is: open, opening or closing

// Button Input
extern const int PIN_BUTTON;           // Manual control button

// ============================================================================
// GATE STATE ENUMERATION
// ============================================================================
enum GateState : byte {
    GATE_UNKNOWN,   // Initial state or sensor malfunction
    GATE_CLOSED,    // Gate is fully closed (sensor HIGH)
    GATE_OPENING,   // Gate is in process of opening
    GATE_OPEN,      // Gate is fully open (sensor LOW, stable)
    GATE_CLOSING    // Gate is in process of closing
};

// ============================================================================
// GATE CLASS DECLARATION
// ============================================================================
class Gate {
public:
    /**
     * Constructor - Initialize gate controller
     */
    Gate();
    
    /**
     * Destructor - Clean up resources
     */
    ~Gate();
    
    /**
     * Initialize gate controller
     * Must be called after GPIO pins are configured
     */
    void initialize();
    
    /**
     * Update gate state machine
     * Should be called regularly in main loop
     */
    void update();
    
    /**
     * Toggle gate state (open if closed, close if open)
     * Equivalent to button press functionality
     */
    void toggle();
    
    /**
     * Command gate to open
     * Only effective if gate is currently closed
     */
    void openGate();
    
    /**
     * Command gate to close  
     * Only effective if gate is currently open
     */
    void closeGate();
    
    /**
     * Get current gate state
     * @return Current GateState enumeration value
     */
    GateState getState() const;
    
    /**
     * Check if gate is currently moving
     * @return true if gate is opening or closing
     */
    bool isMoving() const;
    
    /**
     * Check if relay is currently active
     * @return true if relay is currently activated
     */
    bool isRelayActive() const;
    
    /**
     * Get state as string for logging/MQTT
     * @return String representation of current state
     */
    String getStateString() const;

private:
    // Timer management
    Timer<> _stateTimer;        // Timer for gate operation timing
    Timer<> _relayTimer;        // Timer for relay pulse control
    
    // State tracking
    GateState _currentState;    // Current gate state
    GateState _previousState;   // Previous state for change detection
    bool _sensorState;          // Current sensor reading
    bool _previousSensorState;  // Previous sensor reading for debouncing
    
    // Timing variables
    unsigned long _lastStateChange;     // Timestamp of last state change
    unsigned long _lastSensorRead;      // Timestamp of last sensor reading
    unsigned long _relayActivationTime; // Timestamp when relay was activated
    
    // Control flags
    bool _relayActive;          // Flag indicating relay is currently active
    bool _initialized;          // Flag indicating initialization complete
    
    // Private methods
    void _updateGateState(GateState newState);
    bool _readSensor();
    void _activateRelay(int relayPin, const char* relayName);
    void _deactivateRelays();
    void _logStateChange(GateState oldState, GateState newState);
    bool _isValidStateTransition(GateState from, GateState to);
    void _handleBootupState();
};

#endif // Gate_h
