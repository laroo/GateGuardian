/**
 * LEDManager.h - ESP32 Swing Gate Controller LED Manager Header
 * 
 * Defines the LEDManager class interface for controlling LED status indicators
 * based on gate state with solid and blinking patterns.
 */

#ifndef LEDManager_h
#define LEDManager_h

#include "Arduino.h"
#include <arduino-timer.h>
#include "gate.h"  // Include gate.h for GateState enum

// ============================================================================
// LED MANAGER CLASS DECLARATION
// ============================================================================
class LEDManager {
public:
    /**
     * Constructor - Initialize LED manager
     * @param redPin GPIO pin for red LED (gate closed indicator)
     * @param greenPin GPIO pin for green LED (gate open indicator)
     */
    LEDManager(int redPin, int greenPin);
    
    /**
     * Destructor - Clean up resources
     */
    ~LEDManager();
    
    /**
     * Initialize LED manager
     * Must be called after GPIO pins are configured
     */
    void initialize();
    
    /**
     * Update LED states and handle blinking
     * Should be called regularly in main loop
     */
    void update();
    
    /**
     * Set LED status based on gate state
     * @param state Current gate state to display
     */
    void setStatus(GateState state);
    
    /**
     * Turn on red LED continuously (gate closed)
     */
    void solidRed();
    
    /**
     * Turn on green LED continuously (gate open)
     */
    void solidGreen();
    
    /**
     * Blink red LED (gate closing)
     */
    void blinkRed();
    
    /**
     * Blink green LED (gate opening)
     */
    void blinkGreen();
    
    /**
     * Blink both LEDs (unknown state)
     */
    void blinkBoth();
    
    /**
     * Turn off both LEDs
     */
    void allOff();

private:
    // GPIO pins
    int _redPin;        // Red LED pin (closed/closing indicator)
    int _greenPin;      // Green LED pin (open/opening indicator)
    
    // Timer management
    Timer<> _blinkTimer;    // Timer for blinking control
    
    // State tracking
    bool _blinkState;       // Current blink state (on/off)
    bool _redBlinking;      // Flag indicating red LED is blinking
    bool _greenBlinking;    // Flag indicating green LED is blinking
    bool _bothBlinking;     // Flag indicating both LEDs are blinking
    bool _initialized;      // Flag indicating initialization complete
    
    // Current LED states
    bool _redLedState;      // Current red LED state
    bool _greenLedState;    // Current green LED state
    
    // Private methods
    void _setRedLED(bool state);
    void _setGreenLED(bool state);
    void _stopBlinking();
    bool _blinkTimerCallback(void* argument);
};

#endif // LEDManager_h