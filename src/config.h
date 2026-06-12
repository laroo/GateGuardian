#pragma once

// Stringify helper macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// OTA credentials (passed as bare identifiers via -D flags, stringified here)
#ifndef OTA_USERNAME
#error Need to define OTA_USERNAME
#endif
#ifndef OTA_PASSWORD
#error Need to define OTA_PASSWORD
#endif
#define OTA_USERNAME_STR TOSTRING(OTA_USERNAME)
#define OTA_PASSWORD_STR TOSTRING(OTA_PASSWORD)

// MQTT broker
#ifndef MQTT_BROKER
#define MQTT_BROKER "test.mosquitto.org"
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif


#ifndef MQTT_USERNAME
#define MQTT_USERNAME nullptr
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD nullptr
#endif

// Boot delay (Requirement 8) - delay in ms before normal initialization after power-on
#ifndef BOOT_DELAY_MS
#define BOOT_DELAY_MS 15000
#endif

// MQTT live sequence recording (Requirement 11)
// Define MQTT_SEQUENCE_RECORD to publish CSV rows to gateguardian/sequence after
// every publishStatus() call. Disabled by default; enable via build flag in private_config.ini.
// #define MQTT_SEQUENCE_RECORD

// MQTT topic prefix
#ifndef MQTT_TOPIC_STATUS
#define MQTT_TOPIC_STATUS "gateguardian/status"
#endif
#ifndef MQTT_TOPIC_COMMAND
#define MQTT_TOPIC_COMMAND "gateguardian/command"
#endif
