/**
 * MQTTManager.cpp - ESP32 Swing Gate Controller MQTT Manager Implementation
 *
 * Implementation of MQTT communication over Ethernet for remote gate control
 * and status reporting using W5500 Ethernet shield and PubSubClient library.
 */

#include "Arduino.h"
#include "mqttmanager.h"
#include <ETH.h>

// Static instance pointer for callback handling
MQTTManager* MQTTManager::_instance = nullptr;

// ============================================================================
// MQTT MANAGER CLASS IMPLEMENTATION
// ============================================================================

MQTTManager::MQTTManager(const char* broker, int port, const char* clientId,
                         const char* statusTopic, const char* commandTopic)
    : _port(port), _ethClient(nullptr), _mqttClient(nullptr),
      _initialized(false), _wifiConnected(false), _autoPublishEnabled(true),
      _lastPublish(0), _lastConnectionAttempt(0), _reconnectAttempts(0),
      _gateController(nullptr) {
    
    // Copy configuration strings
    strncpy(_broker, broker, sizeof(_broker) - 1);
    _broker[sizeof(_broker) - 1] = '\0';
    
    strncpy(_clientId, clientId, sizeof(_clientId) - 1);
    _clientId[sizeof(_clientId) - 1] = '\0';
    
    strncpy(_statusTopic, statusTopic, sizeof(_statusTopic) - 1);
    _statusTopic[sizeof(_statusTopic) - 1] = '\0';
    
    strncpy(_commandTopic, commandTopic, sizeof(_commandTopic) - 1);
    _commandTopic[sizeof(_commandTopic) - 1] = '\0';
    
    // Set static instance for callback handling
    _instance = this;
    
    Serial.println("[MQTT] MQTTManager constructor called");
}

MQTTManager::~MQTTManager() {
    Serial.println("[MQTT] MQTTManager destructor called");
    
    if (_mqttClient) {
        _mqttClient->disconnect();
        delete _mqttClient;
    }
    
    if (_ethClient) {
        delete _ethClient;
    }
    
    _instance = nullptr;
}

void MQTTManager::initialize(NetworkClient* activeClient) {
    Serial.println("[MQTT] Initializing MQTT manager...");
    
    // Initialize WiFi connection
    // if (!_initializeWiFi()) {
    //     Serial.println("[ERROR] Failed to initialize WiFi connection");
    //     return;
    // }
    
    // Create MQTT client
    _ethClient = activeClient;
    
    // if (!_ethClient) {
    //     Serial.println("[ERROR] Failed to create ethernet client");
    //     return;
    // }
    
    _mqttClient = new PubSubClient;
    if (!_mqttClient) {
        Serial.println("[ERROR] Failed to create MQTT client");
        return;
    }
    
    // Configure MQTT client
    _mqttClient->setServer(_broker, _port);
    _mqttClient->setCallback(_messageCallback);
    
    _initialized = true;
    
    Serial.print("[MQTT] MQTT manager initialized - Broker: ");
    Serial.print(_broker);
    Serial.print(":");
    Serial.println(_port);
    Serial.print("[MQTT] Client ID: ");
    Serial.println(_clientId);
    Serial.print("[MQTT] Status topic: ");
    Serial.println(_statusTopic);
    Serial.print("[MQTT] Command topic: ");
    Serial.println(_commandTopic);
    
    // Attempt initial connection
    // connect();
}

void MQTTManager::update() {
    static unsigned long lastNetworkUpdate = 0;
    bool debug = false;
    // if (millis() - lastNetworkUpdate >= 500) {
    //     lastNetworkUpdate = millis(); 
    //     debug = true;
    // }

    
    if (!_initialized) {
        if (debug) Serial.println("[MQTT] Not initialized...");
        return;
    }

    if (!_ethClient) {
        if (debug) Serial.println("[MQTT] No client...");
        return;
    };

    // mqttClient.setClient(*activeClient);
    // if (!mqttClient.connected()) {
    //   mqttReconnect();
    // }
    // mqttClient.loop();
    
    // Tick timers
    _publishTimer.tick();
    _reconnectTimer.tick();

    // Update the client reference in case connection switched between Ethernet/WiFi
    if (_ethClient) {
        if (debug) Serial.println("[MQTT] Setting client...");
        _mqttClient->setClient(*_ethClient);
    }
    
    // Handle MQTT client loop
    if (_mqttClient && _mqttClient->connected()) {
        if (debug) Serial.println("[MQTT] Looping...");
        _mqttClient->loop();
    } else {
        if (debug) Serial.println("[MQTT] Not connected...");
        // Connection lost, attempt reconnection
        unsigned long currentTime = millis();
        if (currentTime - _lastConnectionAttempt >= 10000) { // Retry every 10 seconds
            Serial.println("[MQTT] Connection lost, attempting reconnection...");
            connect();
        }
    }
}

bool MQTTManager::connect() {
    if (!_initialized || !_mqttClient) {
        Serial.println("[ERROR] MQTT manager not initialized");
        return false;
    }
    
    _lastConnectionAttempt = millis();
    
    Serial.print("[MQTT] Attempting to connect to broker: ");
    Serial.print(_broker);
    Serial.print(":");
    Serial.println(_port);
    
    // Attempt MQTT connection
    bool connected = _mqttClient->connect(_clientId);
    
    if (connected) {
        Serial.println("[MQTT] Connected to broker successfully");
        _reconnectAttempts = 0;
        
        // Subscribe to command topic
        if (_mqttClient->subscribe(_commandTopic)) {
            Serial.print("[MQTT] Subscribed to command topic: ");
            Serial.println(_commandTopic);
        } else {
            Serial.print("[ERROR] Failed to subscribe to command topic: ");
            Serial.println(_commandTopic);
        }
        
        // Set up periodic status publishing if enabled
        if (_autoPublishEnabled) {
            _publishTimer.every(10000, [](void* manager) -> bool {
                return static_cast<MQTTManager*>(manager)->_publishTimerCallback(nullptr);
            }, this);
            Serial.println("[MQTT] Automatic status publishing enabled (10-second interval)");
        }
        
        _logConnectionStatus();
        
    } else {
        _reconnectAttempts++;
        Serial.print("[ERROR] MQTT connection failed, rc=");
        Serial.print(_mqttClient->state());
        Serial.print(", attempt #");
        Serial.println(_reconnectAttempts);
        
        // Set up reconnection timer if not already running
        if (_reconnectAttempts == 1) {
            _reconnectTimer.every(30000, [](void* manager) -> bool {
                return static_cast<MQTTManager*>(manager)->_reconnectTimerCallback(nullptr);
            }, this);
        }
    }
    
    return connected;
}

bool MQTTManager::publishStatus(const String& status) {
    Serial.println("[MQTT] publish status...");
    if (!_initialized || !_mqttClient || !_mqttClient->connected()) {
        Serial.println("[ERROR] MQTT not connected, cannot publish status");
        return false;
    }
    
    // Create status message
    String message;
    if (_gateController) {
        message = _formatStatusMessage(_gateController->getState());
    } else {
        message = status; // Use provided status if no gate controller
    }
    
    // Publish message
    bool success = _mqttClient->publish(_statusTopic, message.c_str());
    
    if (success) {
        _lastPublish = millis();
    }
    
    _logPublishEvent(message, success);
    return success;
}

bool MQTTManager::isConnected() {
    return _initialized && _mqttClient && _mqttClient->connected();
}

void MQTTManager::setClient(NetworkClient* client) {
    _ethClient = client;
    // Serial.println("[MQTT] Client set");
}

void MQTTManager::setGateController(Gate* gate) {
    _gateController = gate;
    Serial.println("[MQTT] Gate controller reference set");
}

void MQTTManager::setAutoPublish(bool enabled) {
    _autoPublishEnabled = enabled;
    Serial.print("[MQTT] Automatic publishing ");
    Serial.println(enabled ? "enabled" : "disabled");
}

// ============================================================================
// PRIVATE METHODS
// ============================================================================

// bool MQTTManager::_initializeWiFi() {
//     Serial.println("[MQTT] Initializing WiFi connection...");
    
//     // WiFi credentials for Wokwi simulation
//     const char* ssid = "Wokwi-GUEST";
//     const char* password = "";
    
//     // Set WiFi mode to station
//     WiFi.mode(WIFI_STA);
    
//     // Begin WiFi connection
//     WiFi.begin(ssid, password);
//     Serial.print("[MQTT] Connecting to WiFi network: ");
//     Serial.println(ssid);
    
//     // Wait for connection with timeout
//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts < 20) {
//         delay(500);
//         Serial.print(".");
//         attempts++;
//     }
    
//     if (WiFi.status() == WL_CONNECTED) {
//         _wifiConnected = true;
//         Serial.println();
//         Serial.println("[MQTT] WiFi connected successfully!");
        
//         // Print network configuration
//         Serial.print("[MQTT] IP address: ");
//         Serial.println(WiFi.localIP());
//         Serial.print("[MQTT] Gateway: ");
//         Serial.println(WiFi.gatewayIP());
//         Serial.print("[MQTT] Subnet mask: ");
//         Serial.println(WiFi.subnetMask());
//         Serial.print("[MQTT] DNS server: ");
//         Serial.println(WiFi.dnsIP());
//         Serial.print("[MQTT] RSSI: ");
//         Serial.print(WiFi.RSSI());
//         Serial.println(" dBm");
//     } else {
//         _wifiConnected = false;
//         Serial.println();
//         Serial.println("[ERROR] Failed to connect to WiFi");
//         Serial.print("[ERROR] WiFi status: ");
//         Serial.println(WiFi.status());
//     }
    
//     return _wifiConnected;
// }

void MQTTManager::_onMessageReceived(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String command;
    command.reserve(length + 1);
    for (unsigned int i = 0; i < length; i++) {
        command += (char)payload[i];
    }
    
    Serial.print("[MQTT] Message received on topic: ");
    Serial.println(topic);
    Serial.print("[MQTT] Payload: ");
    Serial.println(command);
    
    // Handle command if it's on the command topic
    if (strcmp(topic, _commandTopic) == 0) {
        _handleCommand(command);
    }
}

void MQTTManager::_messageCallback(char* topic, byte* payload, unsigned int length) {
    if (_instance) {
        _instance->_onMessageReceived(topic, payload, length);
    }
}

bool MQTTManager::_publishTimerCallback(void* argument) {
    // Publish current gate status
    if (_gateController && isConnected()) {
        publishStatus(_gateController->getStateString());
    }
    return true; // Continue periodic publishing
}

bool MQTTManager::_reconnectTimerCallback(void* argument) {
    // Attempt reconnection
    if (!isConnected()) {
        connect();
    }
    
    // Stop timer if connected, continue if still disconnected
    return !isConnected();
}

void MQTTManager::_handleCommand(const String& command) {
    _logCommandReceived(command);
    
    if (!_gateController) {
        Serial.println("[ERROR] No gate controller available for command handling");
        return;
    }
    
    // Parse and execute command (Requirements 7.4, 4.2)
    String upperCommand = command;
    upperCommand.toUpperCase();
    upperCommand.trim();
    
    if (upperCommand == "OPEN") {
        Serial.println("[MQTT] Executing OPEN command");
        _gateController->openGate();
    } else if (upperCommand == "CLOSE") {
        Serial.println("[MQTT] Executing CLOSE command");
        _gateController->closeGate();
    } else if (upperCommand == "STOP") {
        Serial.println("[MQTT] Executing STOP command");
        _gateController->stopGate();
    } else if (upperCommand == "TOGGLE") {
        Serial.println("[MQTT] Executing TOGGLE command");
        _gateController->toggle();
    } else {
        Serial.print("[ERROR] Unknown MQTT command: ");
        Serial.println(command);
    }
}

String MQTTManager::_formatStatusMessage(GateState state) {
    // Create JSON-formatted status message as per design document
    String message = "{";
    message += "\"device_id\":\"" + String(_clientId) + "\",";
    message += "\"timestamp\":" + String(millis() / 1000) + ",";
    message += "\"state\":\"" + String(_gateController ? _gateController->getStateString() : "UNKNOWN") + "\",";
    message += "\"sensor_raw\":" + String(_gateController ? "true" : "false") + ",";
    message += "\"uptime\":" + String(millis() / 1000);
    message += "}";
    
    return message;
}

void MQTTManager::_logConnectionStatus() {
    Serial.println("[MQTT] Connection status:");
    Serial.print("  WiFi: ");
    Serial.println(_wifiConnected ? "Connected" : "Disconnected");
    Serial.print("  MQTT: ");
    Serial.println(isConnected() ? "Connected" : "Disconnected");
    Serial.print("  Broker: ");
    Serial.print(_broker);
    Serial.print(":");
    Serial.println(_port);
}

void MQTTManager::_logPublishEvent(const String& message, bool success) {
    if (success) {
        Serial.print("[MQTT] Status published: ");
        Serial.println(message);
    } else {
        Serial.print("[ERROR] Failed to publish status: ");
        Serial.println(message);
    }
}

void MQTTManager::_logCommandReceived(const String& command) {
    Serial.print("[MQTT] Command received: ");
    Serial.println(command);
}