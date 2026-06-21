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
#include "webmanager.h"
#include "DHT.h"
// #include "RemoteDebug.h"


// #include <PubSubClient.h>

#include "config.h"

#include "esp_system.h"
#include "esp32-hal-gpio.h"
#include "gate.h"
#include "ledmanager.h"
#include "mqttmanager.h"
#include <SPI.h>
#include <Network.h>

EMACDriver driver(ETH_PHY_LAN8720, 23, 18, 16);   // note powerPin = 16 required
// EMACDriver driver(ETH_PHY_LAN8720, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_POWER);
// EthernetClient   ethClient;

EthernetClient ethClient;
WiFiClient wifiClient;

// Pointer to the active client (Ethernet or WiFi)
NetworkClient* activeClient = nullptr;

// Connection check result (used by network event handler)
int connectionStatus = 0;

// DHTesp dhtSensor;
// config.sensor1Pin, DHTesp::DHT22

// #define DHTPIN 4
// #define DHTTYPE DHT22
// DHT dhtSensor(DHTPIN, DHTTYPE);

// RemoteDebug Debug;


// ============================================================================
// NETWORK EVENT HANDLER
// ============================================================================
// WARNING: This function is called from a separate FreeRTOS task (thread)!
void onNetworkEvent(arduino_event_id_t event, arduino_event_info_t info) {
  Serial.printf("[Network-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("[ETH] Ethernet started");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("[ETH] Ethernet stopped");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("[ETH] Ethernet connected - Link UP");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("[ETH] Ethernet disconnected - Link DOWN");
      connectionStatus = 0;
      activeClient = nullptr;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("[ETH] Obtained IP address: ");
      Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
      Serial.print("[ETH] Gateway: ");
      Serial.println(IPAddress(info.got_ip.ip_info.gw.addr));
      Serial.print("[ETH] Netmask: ");
      Serial.println(IPAddress(info.got_ip.ip_info.netmask.addr));
      connectionStatus = 1;
      activeClient = &ethClient;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
      Serial.println("[ETH] Ethernet IPv6 is preferred");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("[WiFi] WiFi client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      Serial.println("[WiFi] WiFi client stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("[WiFi] Connected to access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("[WiFi] Disconnected from WiFi access point");
      if (connectionStatus == 2) {
        connectionStatus = 0;
        activeClient = nullptr;
      }
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("[WiFi] Obtained IP address: ");
      Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
      connectionStatus = 2;
      activeClient = &wifiClient;
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      Serial.println("[WiFi] Lost IP address");
      if (connectionStatus == 2) {
        connectionStatus = 0;
        activeClient = nullptr;
      }
      break;
    default:
      break;
  }
}

// ============================================================================
// CONFIGURATION STRUCTURE
// ============================================================================
struct Config {
  char clientId[32]; // Random client ID generated at startup

  // Timing Settings
  unsigned long gateOperationTime = 20000; // 20 seconds (Requirement 2)
  unsigned long relayPulseTime = 500;      // 500ms (Requirement 1)
  unsigned long publishInterval = 10000;   // 10 seconds (Requirement 7)
  unsigned long blinkInterval = 500;       // 500ms (Requirement 3)
  unsigned long debounceTime = 50;         // 50ms button debounce

  // GPIO Pins
  int redLedPin = 17;
  int greenLedPin = 5;

  int gateLightsPin = 33;
  int gateLockPin = 32;
  int externalRelayPin = 35;  // Input only
  int photoEyePin = 36;  // Input only

  int sensor1Pin = 4;
  int sensor2Pin = 2;

  int openRelayPin = 15;
  int closeRelayPin = 12;
  int stopRelayPin = 14;

};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
Config config;
Gate *gate = nullptr;
LEDManager *ledManager = nullptr;
MQTTManager *mqttManager = nullptr;

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
void handleBootDelay();
// void handleButtonInput();
void printConfigSummary();
int checkConnection();
bool checkConnectionCallback(void *);
bool reportConnectionStatusCallback(void *);
// bool checkInputCallback(void *);
NetworkClient* getActiveClient();


// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {

  // Boot delay for power outage safety (Requirement 8)
  handleBootDelay();

  // Initialize GPIO pins (Requirement 5.1)
  initializeGPIO();

  // Initialize serial communication at 115200 baud (Requirement 4.1)
  Serial.begin(115200);
  delay(100); // Allow serial to initialize

  // Print initialization messages (Requirement 4.1)
  Serial.println("[INIT] ESP32 Gate Controller v1.0 starting...");
  Serial.print("[INIT] Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  // dhtSensor.begin();
  // float temperature = dhtSensor.readTemperature();
  // float humidity = dhtSensor.readHumidity();
  // Serial.println("Temp:     " + String(temperature, 2) + "°C");
  // Serial.println("Humidity: " + String(humidity, 1) + "%");

  // Generate random client ID for MQTT
  randomSeed(analogRead(0));
  sprintf(config.clientId, "esp32_gate_%06X", random(0xFFFFFF));
  Serial.print("[INIT] MQTT Client ID: ");
  Serial.println(config.clientId);

  // Debug.begin(config.clientId);    // Initialize the WiFi server
  // Debug.setResetCmdEnabled(true);  // Enable the reset command
  // Debug.showProfiler(false);        // Profiler (Good to measure times, to optimize codes)
  // Debug.showColors(true);          // Colors
  // Serial.println("[RemoteDebug] Initialized");
  
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

  // Register network event listener
  Network.onEvent(onNetworkEvent);
  Serial.println("[INIT] Network event listener registered");

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

  // Initialize MQTT Manager (Requirements 7.1, 7.2)
  mqttManager = new MQTTManager(MQTT_BROKER, MQTT_PORT, 
                                config.clientId, MQTT_TOPIC_STATUS, 
                                MQTT_TOPIC_COMMAND, MQTT_USERNAME, MQTT_PASSWORD);
  if (mqttManager) {
    mqttManager->initialize(activeClient);
    
    // Set gate controller reference for command handling
    if (gate) {
      mqttManager->setGateController(gate);
    }
    
    Serial.println("[INIT] MQTT manager initialized");
  } else {
    Serial.println("[ERROR] Failed to initialize MQTT manager");
  }

  Serial.println("[INIT] Setting up web server...");
  setupWebServer(config.clientId);

  // Print configuration summary
  printConfigSummary();

  // Schedule checkConnection to run every 1000ms (1 second)
  mainTimer.every(5000, checkConnectionCallback);
  Serial.println("[INIT] Connection check scheduled every 1 second");

  // Schedule input to run every 1000ms
  // mainTimer.every(10000, checkInputCallback);
  // Serial.println("[INIT] input check scheduled every 1 second");


  // Schedule connection status reporting every 2000ms (2 seconds)
  mainTimer.every(5000, reportConnectionStatusCallback);
  Serial.println("[INIT] Connection status reporting scheduled every 2 seconds");

  Serial.println("[INIT] System initialization complete");
  Serial.println("======================================");
}

// Timer callback for connection checking
bool checkConnectionCallback(void *) {
  connectionStatus = checkConnection();
  return true; // Repeat the timer
}

// Timer callback for reporting connection status
bool reportConnectionStatusCallback(void *) {
  if (connectionStatus == 1) {
    Serial.println("Connected to Ethernet");
  }
  else if (connectionStatus == 2) {
    Serial.println("Connected to Wi-Fi");
  }
  else {
    Serial.println("Not Connected");
  }
  return true; // Repeat the timer
}

int checkConnection() {
  // Check if Ethernet is available
  if(ethClient.connected() && (Ethernet.linkStatus() == LinkON)) {
    Serial.println("Existing Ethernet connection");
    activeClient = &ethClient;
    return 1;
  }
  else if (Ethernet.linkStatus() == LinkON) {
    // Use Ethernet connection
    Serial.println("Ethernet LINKON");
    if (!ethClient.connected()) {
      Serial.println("Connecting via Ethernet...");
        // beginMicros = micros();
        WiFi.disconnect();
        //delay(5000);
        activeClient = &ethClient;
        return 1;
    }
  }
  // else{
  //   Serial.println("Not Successfull Ethernet Connection");
  // }

 if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Existing Wi-Fi connection");
    activeClient = &wifiClient;
    return 2;
  }

  // Use Wi-Fi connection
  WiFi.begin("Wokwi-GUEST", "", 6);
  Serial.println("Connecting via Wi-Fi...");
  for (int i = 0; i < 50; i++) {
    if ((WiFi.status() == WL_CONNECTED))
      Serial.println(" CONNECTED");
      break;
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wi-Fi ");
    // Serial.println(WiFi.localIP());
    // Additional Wi-Fi initialization if needed
    activeClient = &wifiClient;
    return 2;
  }
  else{
    Serial.println("Wi-Fi not Connected");
  }

  activeClient = nullptr;
  return 0;
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {

    static unsigned long lastUpdate = 0;

    // Update button state every 1ms
    if (millis() - lastUpdate >= 1) {
        lastUpdate = millis();
        // gateLightsButton.update();
        // gateLockButton.update();
        // externalRelayButton.update();
        // photoEyeButton.update();    
    }



  // Connection status is now reported by timer callback every 2 seconds
  if (activeClient && connectionStatus > 0) {

    // static unsigned long lastNetworkUpdate = 0;
    // if (millis() - lastNetworkUpdate >= 500) {
    //     lastNetworkUpdate = millis(); 
    //     Serial.println("Active Network Loop!");
    // }

    loopWebServer();

    // Update MQTT manager (Requirements 7.1, 7.2, 7.3, 7.4)
    if (mqttManager) {
      mqttManager->setClient(activeClient);
      mqttManager->update();
    }
    // Debug.handle();

  }

  unsigned long loopStart = millis();

  // Handle button input with debouncing
  // handleButtonInput();

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
// BOOT DELAY (Requirement 8)
// ============================================================================
void handleBootDelay() {
  // Only run boot delay on power-on reset, not after software reset
  esp_reset_reason_t reason = esp_reset_reason();
  if (reason == ESP_RST_SW) {
    // This is the clean reboot after boot delay — skip
    Serial.begin(115200);
    delay(100);
    Serial.println("[BOOT] Software reset detected, skipping boot delay");
    return;
  }

  // Force relay pins LOW immediately to prevent spurious signals
  pinMode(config.openRelayPin, OUTPUT);
  pinMode(config.closeRelayPin, OUTPUT);
  pinMode(config.stopRelayPin, OUTPUT);
  digitalWrite(config.openRelayPin, LOW);
  digitalWrite(config.closeRelayPin, LOW);
  digitalWrite(config.stopRelayPin, LOW);

  // Initialize LEDs for boot delay indication
  pinMode(config.redLedPin, OUTPUT);
  pinMode(config.greenLedPin, OUTPUT);

  Serial.begin(115200);
  delay(100);
  Serial.println("[BOOT] Power-on detected, starting boot delay...");
  Serial.print("[BOOT] Waiting ");
  Serial.print(BOOT_DELAY_MS / 1000);
  Serial.println(" seconds for Sommer Twist 350 to initialize");

  unsigned long startTime = millis();
  unsigned long remaining = BOOT_DELAY_MS;
  int lastSecond = -1;
  bool ledState = false;

  while (remaining > 0) {
    unsigned long elapsed = millis() - startTime;
    remaining = (elapsed >= BOOT_DELAY_MS) ? 0 : (BOOT_DELAY_MS - elapsed);
    int currentSecond = remaining / 1000;

    // Print countdown once per second
    if (currentSecond != lastSecond) {
      lastSecond = currentSecond;
      Serial.print("[BOOT] Countdown: ");
      Serial.print(currentSecond + 1);
      Serial.println("s remaining");
    }

    // Blink both LEDs together (250ms on/off) as boot-delay indicator
    ledState = (millis() / 250) % 2 == 0;
    digitalWrite(config.redLedPin, ledState ? HIGH : LOW);
    digitalWrite(config.greenLedPin, ledState ? HIGH : LOW);

    // Keep relay pins forced LOW during delay
    digitalWrite(config.openRelayPin, LOW);
    digitalWrite(config.closeRelayPin, LOW);
    digitalWrite(config.stopRelayPin, LOW);

    delay(10);
  }

  // Turn LEDs off before restart
  digitalWrite(config.redLedPin, LOW);
  digitalWrite(config.greenLedPin, LOW);

  Serial.println("[BOOT] Boot delay complete, performing software reset...");
  Serial.flush();
  delay(100);
  ESP.restart();
}

// ============================================================================
// GPIO INITIALIZATION
// ============================================================================
void initializeGPIO() {

  // Configure LED outputs
  pinMode(config.redLedPin, OUTPUT);
  pinMode(config.greenLedPin, OUTPUT);
  digitalWrite(config.redLedPin, LOW);
  digitalWrite(config.greenLedPin, LOW);

  // Configure relay outputs
  pinMode(config.openRelayPin, OUTPUT);
  pinMode(config.closeRelayPin, OUTPUT);
  pinMode(config.stopRelayPin, OUTPUT);
  digitalWrite(config.openRelayPin, LOW);
  digitalWrite(config.closeRelayPin, LOW);
  digitalWrite(config.stopRelayPin, LOW);
  
  // Configure sensor input with internal pull-up  
  pinMode(config.gateLightsPin, INPUT_PULLUP);
  pinMode(config.gateLockPin, INPUT_PULLUP);
  pinMode(config.externalRelayPin, INPUT);
  pinMode(config.photoEyePin, INPUT);

  // Configure sensor input with internal pull-up
  pinMode(config.sensor1Pin, INPUT);
  pinMode(config.sensor2Pin, INPUT);
  
}

// ============================================================================
// CONFIGURATION SUMMARY
// ============================================================================
void printConfigSummary() {
  Serial.println("[CONFIG] System Configuration:");
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

// ============================================================================
// GET ACTIVE CLIENT
// ============================================================================
/**
 * Returns a pointer to the active network client (Ethernet or WiFi)
 * based on the current connection status.
 * 
 * @return NetworkClient* Pointer to active client, or nullptr if no connection
 */
NetworkClient* getActiveClient() {
  return activeClient;
}
