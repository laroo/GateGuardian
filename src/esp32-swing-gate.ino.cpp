/**
 * ESP32 Swing Gate Controller
 * Main sketch file for controlling Sommer Twist 350 swing gate
 *
 * Features:
 * - Manual control via button
 * - MQTT remote control via Ethernet
 * - LED status indicators
 * - Gate position sensing
 * - Serial diagnostics at 115200 baud
 */

#include "Arduino.h"
#include <PubSubClient.h>
#include <arduino-timer.h>
#include <EthernetESP32.h>
// #include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>

#include "gate.h"
#include "ledmanager.h"
// #include "mqttmanager.h"
#include <SPI.h>

EMACDriver driver(ETH_PHY_LAN8720, 23, 18, 16);   // note powerPin = 16 required
// EMACDriver driver(ETH_PHY_LAN8720, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_POWER);
// EthernetClient   ethClient;

EthernetClient ethClient;
WiFiClient wifiClient;

WebServer server(80);

// ============================================================================
// CONFIGURATION STRUCTURE
// ============================================================================
struct Config {
  // MQTT Settings (Requirement 7)
  char mqttBroker[64] = "broker.hivemq.com";
  int mqttPort = 1883;
  char clientId[32]; // Random client ID generated at startup
  char statusTopic[64] = "gate/status";
  char commandTopic[64] = "gate/command";

  // Timing Settings
  unsigned long gateOperationTime = 20000; // 20 seconds (Requirement 2)
  unsigned long relayPulseTime = 500;      // 500ms (Requirement 1)
  unsigned long publishInterval = 10000;   // 10 seconds (Requirement 7)
  unsigned long blinkInterval = 500;       // 500ms (Requirement 3)
  unsigned long debounceTime = 50;         // 50ms button debounce

  // GPIO Pins
  int buttonPin = 35;
  int sensorPin = 33;
  int redLedPin = 17;
  int greenLedPin = 5;
  int openRelayPin = 15;
  int closeRelayPin = 12;
  int stopRelayPin = 14;

  // Ethernet SPI Pins
  // int ethernetCSPin = 5;
  // int ethernetMOSIPin = 23;
  // int ethernetMISOPin = 19;
  // int ethernetSCKPin = 18;
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
Config config;
Gate *gate = nullptr;
LEDManager *ledManager = nullptr;
// MQTTManager *mqttManager = nullptr;

// Timer for main loop management
auto mainTimer = timer_create_default();

// Previous gate state for change detection
GateState previousGateState = GATE_UNKNOWN;

// Button handling variables
bool lastButtonState = LOW; // Button is active LOW with pull-up
bool currentButtonState = HIGH;
unsigned long lastButtonChange = 0;
bool buttonPressed = false; // Flag to track button press events

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void initializeGPIO();
void handleButtonInput();
void printConfigSummary();

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize serial communication at 115200 baud (Requirement 4.1)
  Serial.begin(115200);
  delay(100); // Allow serial to initialize

  // Print initialization messages (Requirement 4.1)
  Serial.println("[INIT] ESP32 Gate Controller v1.0 starting...");
  Serial.print("[INIT] Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  // Generate random client ID for MQTT
  randomSeed(analogRead(0));
  sprintf(config.clientId, "esp32_gate_%06X", random(0xFFFFFF));
  Serial.print("[INIT] MQTT Client ID: ");
  Serial.println(config.clientId);

  // Initialize GPIO pins (Requirement 5.1)
  initializeGPIO();

  // Initialize button state after GPIO configuration
  lastButtonState = digitalRead(config.buttonPin);
  currentButtonState = lastButtonState;
  Serial.print("[INIT] Initial button state: ");
  Serial.println(lastButtonState ? "HIGH (not pressed)" : "LOW (pressed)");

  // Initialize Gate controller
  gate = new Gate();
  if (gate) {
    gate->initialize();
    Serial.println("[INIT] Gate controller created and initialized");
  } else {
    Serial.println("[ERROR] Failed to create Gate controller");
  }

  // Initialize LED Manager
  ledManager = new LEDManager(config.redLedPin, config.greenLedPin);
  if (ledManager) {
    ledManager->initialize();

    // Set initial LED state based on gate state
    if (gate) {
      GateState initialState = gate->getState();
      ledManager->setStatus(initialState);
      previousGateState = initialState;
    }

    Serial.println("[INIT] LED manager initialized");
  } else {
    Serial.println("[ERROR] Failed to initialize LED manager");
  }

  Ethernet.init(driver);

  Serial.print("Link 1: ");
  Serial.println(Ethernet.linkStatus()); //Prints 2 = LinkOFF, as expected


  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin()) {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Failed to configure Ethernet using DHCP");
    // while (true) {
    //   delay(1);
    // }
  }

  Serial.print("Link 2: ");
  Serial.println(Ethernet.linkStatus()); //prints 1 = LinkON, even with no cable plugged in

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    // while (true) {
    //   delay(1); // do nothing, no point running without Ethernet hardware
    // }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    //This statement should print but doesn't when I test with no cable
    Serial.println("Ethernet cable is not connected.");
  }

  // if (MDNS.begin("gateguardian")) {
  //   Serial.println("MDNS responder started");
  // }

  // // Initialize MQTT Manager (Requirements 7.1, 7.2)
  // mqttManager = new MQTTManager(config.mqttBroker, config.mqttPort, 
  //                               config.clientId, config.statusTopic, 
  //                               config.commandTopic);
  // if (mqttManager) {
  //   mqttManager->initialize();
    
  //   // Set gate controller reference for command handling
  //   if (gate) {
  //     mqttManager->setGateController(gate);
  //   }
    
  //   Serial.println("[INIT] MQTT manager initialized");
  // } else {
  //   Serial.println("[ERROR] Failed to initialize MQTT manager");
  // }


  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is GateGuardian REUPLOAD1.");
  });
  server.on("/gate/close", []() {
    gate->closeGate();
    server.send(200, "text/plain", "Gate closing...");
  });
  server.on("/gate/open", []() {
    gate->openGate();
    server.send(200, "text/plain", "Gate opening...");
  });
  server.on("/gate/toggle", []() {
    gate->toggle();
    server.send(200, "text/plain", "Gate toggling...");
  });
  
  // server.on("/gate/stop", []() {
  //   gate->stop();
  //   server.send(200, "text/plain", "Gate stopping...");
  // });
  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  // Print configuration summary
  printConfigSummary();

  Serial.println("[INIT] System initialization complete");
  Serial.println("======================================");
}



int checkConnection() {
  // Check if Ethernet is available
  if(ethClient.connected() && (Ethernet.linkStatus() == LinkON))
    return 1;
  if (Ethernet.linkStatus() == LinkON) {
    // Use Ethernet connection
    Serial.println("Ethernet LINKON");
    if (!ethClient.connected()) {
      Serial.println("Connecting via Ethernet...");
        // beginMicros = micros();
        WiFi.disconnect();
        //delay(5000);
        return 1;
    }
  }
  // else{
  //   Serial.println("Not Successfull Ethernet Connection");
  // }

 if (WiFi.status() == WL_CONNECTED)
    return 2;

  // Use Wi-Fi connection
  WiFi.begin("Wokwi-GUEST", "", 6);
  Serial.println("Connecting via Wi-Fi...");
  for (int i = 0; i < 50; i++) {
    if ((WiFi.status() == WL_CONNECTED))
      break;
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    // Serial.print("Connected to Wi-Fi. Local IP: ");
    // Serial.println(WiFi.localIP());
    // Additional Wi-Fi initialization if needed
    return 2;
  }
  else{
    Serial.println("Wi-Fi not Connected");
  }

  return 0;
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  int i = checkConnection();
  if (i == 1) {
    Serial.println("Connected to Ethernet");
  }
  else if (i == 2) {
    Serial.println("Connected to Wi-Fi");
  }
  else{
    Serial.println("Not Connected");
  }

  if (i > 0) {
    server.handleClient();
    ElegantOTA.loop();
  }

  unsigned long loopStart = millis();

  // Handle button input with debouncing
  handleButtonInput();

  // Update gate controller
  if (gate) {
    gate->update();

    // Check for gate state changes and update LEDs
    GateState currentState = gate->getState();
    if (currentState != previousGateState) {
      if (ledManager) {
        ledManager->setStatus(currentState);
      }
      previousGateState = currentState;
    }
  }

  // Update LED manager
  if (ledManager) {
    ledManager->update();
  }

  // Update MQTT manager (Requirements 7.1, 7.2, 7.3, 7.4)
  // if (mqttManager) {
  //   mqttManager->update();
  // }

  // Tick main timer for any scheduled tasks
  mainTimer.tick();

  // Calculate loop execution time
  unsigned long loopTime = millis() - loopStart;

  // Ensure loop completes within 1 second (Requirement 5.2)
  if (loopTime > 1000) {
    Serial.print("[WARNING] Loop execution time exceeded 1 second: ");
    Serial.print(loopTime);
    Serial.println("ms");
  }

  // Small delay to prevent excessive CPU usage
  delay(10);
}

// ============================================================================
// GPIO INITIALIZATION
// ============================================================================
void initializeGPIO() {
  Serial.println("[INIT] Configuring GPIO pins...");

  // Configure button input with internal pull-up
  pinMode(config.buttonPin, INPUT);
  Serial.print("[INIT] Button pin ");
  Serial.print(config.buttonPin);
  Serial.println(" configured as INPUT_PULLUP");
  Serial.print("[INIT] Current button state: ");
  Serial.println(digitalRead(config.buttonPin));

  // Configure sensor input with internal pull-up
  pinMode(config.sensorPin, INPUT_PULLUP);
  Serial.print("[INIT] Sensor pin ");
  Serial.print(config.sensorPin);
  Serial.println(" configured as INPUT_PULLUP");

  // Configure LED outputs
  pinMode(config.redLedPin, OUTPUT);
  pinMode(config.greenLedPin, OUTPUT);
  digitalWrite(config.redLedPin, LOW);
  digitalWrite(config.greenLedPin, LOW);
  Serial.print("[INIT] LED pins ");
  Serial.print(config.redLedPin);
  Serial.print(" and ");
  Serial.print(config.greenLedPin);
  Serial.println(" configured as OUTPUT");

  // Configure relay outputs
  pinMode(config.openRelayPin, OUTPUT);
  pinMode(config.closeRelayPin, OUTPUT);
  pinMode(config.stopRelayPin, OUTPUT);
  digitalWrite(config.openRelayPin, LOW);
  digitalWrite(config.closeRelayPin, LOW);
  digitalWrite(config.stopRelayPin, LOW);
  Serial.print("[INIT] Relay pins: ");
  Serial.print(config.openRelayPin);
  Serial.print(", ");
  Serial.print(config.closeRelayPin);
  Serial.print(", ");
  Serial.print(config.stopRelayPin);
  Serial.println(" configured as OUTPUT");

  Serial.println("[INIT] GPIO pins configured successfully");
}

// ============================================================================
// BUTTON INPUT HANDLING
// ============================================================================
void handleButtonInput() {
  currentButtonState = digitalRead(config.buttonPin);
  unsigned long currentTime = millis();

  // Debug: Log button state changes (remove this in production)
  static unsigned long lastDebugTime = 0;
  if (currentTime - lastDebugTime > 500) { // Every 5 seconds
    Serial.print("[DEBUG] Button state: ");
    Serial.print(currentButtonState ? "HIGH" : "LOW");
    Serial.print(", Gate state: ");
    if (gate) {
      Serial.println(gate->getStateString());
    } else {
      Serial.println("NULL");
    }
    lastDebugTime = currentTime;
  }

  // Check if button state changed and debounce time has passed
  // (Requirement 1.3, 4.3, 5.3)
  if (currentButtonState != lastButtonState &&
      (currentTime - lastButtonChange) >= config.debounceTime) {

    lastButtonChange = currentTime;
    lastButtonState = currentButtonState;

    Serial.print("[BUTTON] Button state changed to: ");
    Serial.println(currentButtonState ? "HIGH (released)" : "LOW (pressed)");

    // Button pressed (LOW due to pull-up resistor)
    if (currentButtonState == LOW) {
      buttonPressed = true;
      Serial.println("[BUTTON] Button pressed - debounced");

      // Check if gate is available and not during relay activation
      // (Requirements 1.1, 1.2, 4.3)
      if (gate) {
        // Prevent button actions during relay activation periods
        if (gate->isRelayActive()) {
          Serial.println("[BUTTON] Relay is active, button action ignored");
        } else if (gate->isMoving()) {
          Serial.println("[BUTTON] Gate is moving, button action ignored");
        } else {
          Serial.println("[BUTTON] Triggering gate toggle");
          gate->toggle();
        }
      } else {
        Serial.println("[ERROR] Gate controller not available");
      }
    } else {
      // Button released (HIGH due to pull-up resistor)
      if (buttonPressed) {
        buttonPressed = false;
        Serial.println("[BUTTON] Button released - debounced");
      }
    }
  }
}

// ============================================================================
// CONFIGURATION SUMMARY
// ============================================================================
void printConfigSummary() {
  Serial.println("[CONFIG] System Configuration:");
  Serial.print("  MQTT Broker: ");
  Serial.print(config.mqttBroker);
  Serial.print(":");
  Serial.println(config.mqttPort);
  Serial.print("  Client ID: ");
  Serial.println(config.clientId);
  Serial.print("  Status Topic: ");
  Serial.println(config.statusTopic);
  Serial.print("  Command Topic: ");
  Serial.println(config.commandTopic);
  Serial.print("  Gate Operation Time: ");
  Serial.print(config.gateOperationTime);
  Serial.println("ms");
  Serial.print("  Relay Pulse Time: ");
  Serial.print(config.relayPulseTime);
  Serial.println("ms");
  Serial.print("  Publish Interval: ");
  Serial.print(config.publishInterval);
  Serial.println("ms");
}
