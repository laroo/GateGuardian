#pragma once

// OTA password
#ifndef OTA_USERNAME
#error Need to define OTA_USERNAME
#endif
#ifndef OTA_PASSWORD
#error Need to define OTA_PASSWORD
#endif

// MQTT broker
#ifndef MQTT_BROKER
#define MQTT_BROKER "broker.hivemq.com"
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif

// MQTT topic prefix
#ifndef MQTT_TOPIC_STATUS
#define MQTT_TOPIC_STATUS "gateguardian/status3"
#endif
#ifndef MQTT_TOPIC_COMMAND
#define MQTT_TOPIC_COMMAND "gateguardian/command3"
#endif
