/**
 * LEDManager.cpp - ESP32 Swing Gate Controller LED Manager Implementation
 *
 * Implementation of LED status indicators with solid and blinking patterns
 * based on gate state. Provides visual feedback for gate status.
 */

#include "Arduino.h"
#include "ledmanager.h"


// ============================================================================
// LED MANAGER CLASS IMPLEMENTATION
// ============================================================================

LEDManager::LEDManager(int redPin, int greenPin)
    : _redPin(redPin), _greenPin(greenPin), _blinkState(false),
      _redBlinking(false), _greenBlinking(false), _bothBlinking(false),
      _initialized(false), _redLedState(false), _greenLedState(false) {
  Serial.println("[LED] LEDManager constructor called");
}

LEDManager::~LEDManager() {
  Serial.println("[LED] LEDManager destructor called");
  allOff();
}

void LEDManager::initialize() {
  Serial.println("[LED] Initializing LED manager...");

  // Note: GPIO pins are already configured in main setup()
  // Ensure both LEDs start in OFF state
  allOff();

  _initialized = true;

  Serial.print("[LED] LED manager initialized - Red pin: ");
  Serial.print(_redPin);
  Serial.print(", Green pin: ");
  Serial.println(_greenPin);
}

void LEDManager::update() {
  if (!_initialized)
    return;

  // Tick the blink timer
  _blinkTimer.tick();
}

void LEDManager::setStatus(GateState state) {
  if (!_initialized) {
    Serial.println("[LED] LED manager not initialized, ignoring status update");
    return;
  }

  Serial.print("[LED] Setting LED status for gate state: ");

  switch (state) {
  case GATE_CLOSED:
    Serial.println("CLOSED (solid red)");
    solidRed();
    break;

  case GATE_OPEN:
    Serial.println("OPEN (solid green)");
    solidGreen();
    break;

  case GATE_OPENING:
    Serial.println("OPENING (blinking green)");
    blinkGreen();
    break;

  case GATE_CLOSING:
    Serial.println("CLOSING (blinking red)");
    blinkRed();
    break;

  case GATE_UNKNOWN:
  default:
    Serial.println("UNKNOWN (blinking both LEDs)");
    blinkBoth();
    break;
  }
}

void LEDManager::solidRed() {
  _stopBlinking();
  _setRedLED(true);
  _setGreenLED(false);
  Serial.println("[LED] Red LED set to solid ON");
}

void LEDManager::solidGreen() {
  _stopBlinking();
  _setRedLED(false);
  _setGreenLED(true);
  Serial.println("[LED] Green LED set to solid ON");
}

void LEDManager::blinkRed() {
  _stopBlinking();
  _setGreenLED(false); // Ensure green is off

  _redBlinking = true;
  _blinkState = true;
  _setRedLED(true); // Start with LED on

  // Set up timer for 500ms blinking interval
  _blinkTimer.every(
      500,
      [](void *manager) -> bool {
        return static_cast<LEDManager *>(manager)->_blinkTimerCallback(nullptr);
      },
      this);

  Serial.println("[LED] Red LED set to BLINKING (500ms interval)");
}

void LEDManager::blinkGreen() {
  _stopBlinking();
  _setRedLED(false); // Ensure red is off

  _greenBlinking = true;
  _blinkState = true;
  _setGreenLED(true); // Start with LED on

  // Set up timer for 500ms blinking interval
  _blinkTimer.every(
      500,
      [](void *manager) -> bool {
        return static_cast<LEDManager *>(manager)->_blinkTimerCallback(nullptr);
      },
      this);

  Serial.println("[LED] Green LED set to BLINKING (500ms interval)");
}

void LEDManager::blinkBoth() {
  _stopBlinking();

  _bothBlinking = true;
  _blinkState = true;
  _setRedLED(true);   // Start with both LEDs on
  _setGreenLED(true);

  // Set up timer for 500ms blinking interval
  _blinkTimer.every(
      500,
      [](void *manager) -> bool {
        return static_cast<LEDManager *>(manager)->_blinkTimerCallback(nullptr);
      },
      this);

  Serial.println("[LED] Both LEDs set to BLINKING (500ms interval)");
}

void LEDManager::allOff() {
  _stopBlinking();
  _setRedLED(false);
  _setGreenLED(false);
  Serial.println("[LED] All LEDs turned OFF");
}

// ============================================================================
// PRIVATE METHODS
// ============================================================================

void LEDManager::_setRedLED(bool state) {
  if (_redLedState != state) {
    _redLedState = state;
    digitalWrite(_redPin, state ? HIGH : LOW);
  }
}

void LEDManager::_setGreenLED(bool state) {
  if (_greenLedState != state) {
    _greenLedState = state;
    digitalWrite(_greenPin, state ? HIGH : LOW);
  }
}

void LEDManager::_stopBlinking() {
  if (_redBlinking || _greenBlinking || _bothBlinking) {
    // Note: arduino-timer doesn't have cancel(), so we just reset our flags
    // The timer callback will check these flags and stop automatically
    _redBlinking = false;
    _greenBlinking = false;
    _bothBlinking = false;
    _blinkState = false;
    Serial.println("[LED] Blinking stopped");
  }
}

bool LEDManager::_blinkTimerCallback(void *argument) {
  // Check if we should continue blinking
  if (!_redBlinking && !_greenBlinking && !_bothBlinking) {
    return false; // Stop the timer
  }

  // Toggle blink state
  _blinkState = !_blinkState;

  // Update the appropriate LED(s) based on which one(s) are blinking
  if (_bothBlinking) {
    _setRedLED(_blinkState);
    _setGreenLED(_blinkState);
  } else if (_redBlinking) {
    _setRedLED(_blinkState);
  } else if (_greenBlinking) {
    _setGreenLED(_blinkState);
  }

  return true; // Continue blinking
}