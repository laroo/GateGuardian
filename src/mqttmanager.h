/**
 * MQTTManager.h - ESP32 Swing Gate Controller MQTT Manager Header
 * 
 * Defines the MQTTManager class interface for MQTT communication
 * over Ethernet using W5500 shield for remote gate control and status reporting.
 */

#ifndef MQTTManager_h
#define MQTTManager_h

#include "Arduino.h"
#include <PubSubClient.h>
#include <arduino-timer.h>
#include "gate.h"  // Include gate.h for GateState enum

#define WOKWI_SIMULATION 1


// Forward declaration for WiFi client
class WiFiClient;

// ============================================================================
// MQTT MANAGER CLASS DECLARATION
// ============================================================================
class MQTTManager {
public:
    /**
     * Constructor - Initialize MQTT manager
     * @param broker MQTT broker hostname
     * @param port MQTT broker port
     * @param clientId Unique client identifier
     * @param statusTopic Topic for publishing gate status
     * @param commandTopic Topic for subscribing to gate commands
     */
    MQTTManager(const char* broker, int port, const char* clientId, 
                const char* statusTopic, const char* commandTopic);
    
    /**
     * Destructor - Clean up resources
     */
    ~MQTTManager();
    
    /**
     * Initialize MQTT manager and Ethernet connection
     * Must be called after GPIO pins are configured
     */
    void initialize();
    
    /**
     * Update MQTT connection and handle messages
     * Should be called regularly in main loop
     */
    void update();
    
    /**
     * Connect to MQTT broker
     * @return true if connection successful
     */
    bool connect();
    
    /**
     * Publish gate status to MQTT broker
     * @param status Gate status string to publish
     * @return true if publish successful
     */
    bool publishStatus(const String& status);
    
    /**
     * Check if MQTT client is connected
     * @return true if connected to broker
     */
    bool isConnected();
    
    /**
     * Set gate controller reference for command handling
     * @param gate Pointer to gate controller instance
     */
    void setGateController(Gate* gate);
    
    /**
     * Enable/disable automatic status publishing
     * @param enabled true to enable periodic publishing
     */
    void setAutoPublish(bool enabled);

private:
    // MQTT configuration
    char _broker[64];           // MQTT broker hostname
    int _port;                  // MQTT broker port
    char _clientId[32];         // Unique client ID
    char _statusTopic[64];      // Status publishing topic
    char _commandTopic[64];     // Command subscription topic
    
    // Network and MQTT clients
    WiFiClient* _wifiClient;    // WiFi client for network connection
    PubSubClient* _mqttClient;  // MQTT client for broker communication
    
    // Timer management
    Timer<> _publishTimer;      // Timer for periodic status publishing
    Timer<> _reconnectTimer;    // Timer for connection retry attempts
    
    // State tracking
    bool _initialized;          // Flag indicating initialization complete
    bool _wifiConnected;        // Flag indicating WiFi connection status
    bool _autoPublishEnabled;   // Flag for automatic status publishing
    unsigned long _lastPublish; // Timestamp of last status publish
    unsigned long _lastConnectionAttempt; // Timestamp of last connection attempt
    int _reconnectAttempts;     // Number of consecutive reconnection attempts
    
    // Gate controller reference
    Gate* _gateController;      // Pointer to gate controller for command handling
    
    // Private methods
    bool _initializeWiFi();
    void _onMessageReceived(char* topic, byte* payload, unsigned int length);
    static void _messageCallback(char* topic, byte* payload, unsigned int length);
    bool _publishTimerCallback(void* argument);
    bool _reconnectTimerCallback(void* argument);
    void _handleCommand(const String& command);
    String _formatStatusMessage(GateState state);
    void _logConnectionStatus();
    void _logPublishEvent(const String& message, bool success);
    void _logCommandReceived(const String& command);
    
    // Static instance pointer for callback handling
    static MQTTManager* _instance;
};

#endif // MQTTManager_h