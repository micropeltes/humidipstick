#pragma once

// Copy this file to credentials.h and replace the example values.
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

#define MQTT_HOST "192.168.1.10"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt-user"
#define MQTT_PASSWORD "mqtt-password"
#define MQTT_CLIENT_ID "esp01-soil-01"
#define MQTT_BASE_TOPIC "garden/soil/esp01-soil-01"

#define OLED_ADDRESS 0x3C

// The ESP8266 connects only to MQTT_HOST:MQTT_PORT.
// If the broker is reachable only through SSH, create the SSH tunnel on a
// server/router/laptop that is reachable by the ESP; do not run SSH on ESP-01.

