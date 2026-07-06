#pragma once

#include <Arduino.h>
#include <pgmspace.h>

constexpr uint8_t I2C_SDA_PIN = 0;
constexpr uint8_t I2C_SCL_PIN = 2;
constexpr uint32_t I2C_CLOCK_HZ = 100000;

constexpr uint8_t CHANNELS_PER_ADS = 4;

// Configure only the ADS1115 addresses that may be installed. The firmware will
// probe each address at boot and continue with any subset that is detected.
// ADS1=ADDR->VCC, ADS2=ADDR->GND, ADS3=ADDR->SCL, ADS4=ADDR->SDA.
const uint8_t ADS_ADDRESSES[] PROGMEM = {0x49, 0x48, 0x4B, 0x4A};
constexpr uint8_t ADS_COUNT = sizeof(ADS_ADDRESSES) / sizeof(ADS_ADDRESSES[0]);
constexpr uint8_t SENSOR_COUNT = ADS_COUNT * CHANNELS_PER_ADS;

// Replace these example values with calibration measurements for every sensor.
// dryRaw may be greater than wetRaw; the conversion formula handles either order.
const int16_t dryRaw[SENSOR_COUNT] PROGMEM = {
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
};

const int16_t wetRaw[SENSOR_COUNT] PROGMEM = {
    9000, 9000, 9000, 9000,
    9000, 9000, 9000, 9000,
    9000, 9000, 9000, 9000,
    9000, 9000, 9000, 9000,
};

constexpr uint32_t DEFAULT_PUBLISH_INTERVAL_MS = 10000;
constexpr uint32_t MIN_PUBLISH_INTERVAL_MS = 1000;
constexpr uint32_t MAX_PUBLISH_INTERVAL_MS = 3600000;
constexpr uint32_t WIFI_RETRY_INTERVAL_MS = 15000;
constexpr uint32_t MQTT_RETRY_INTERVAL_MS = 5000;
constexpr uint32_t OLED_PAGE_INTERVAL_MS = 4000;

constexpr bool OLED_ENABLED = true;
constexpr uint8_t OLED_ADDRESS = 0x3C;
constexpr uint8_t OLED_WIDTH = 128;
constexpr uint8_t OLED_HEIGHT = 64;
constexpr uint8_t OLED_PAGE_COUNT = ADS_COUNT + 1;

// Sized for metadata plus configured ADS objects and channel readings.
constexpr size_t SENSOR_JSON_BASE_CAPACITY = 512;
constexpr size_t SENSOR_JSON_ADS_CAPACITY = 256;
constexpr size_t SENSOR_JSON_CHANNEL_CAPACITY = 128;
constexpr size_t SENSOR_JSON_CAPACITY =
    SENSOR_JSON_BASE_CAPACITY + (ADS_COUNT * SENSOR_JSON_ADS_CAPACITY) +
    (SENSOR_COUNT * SENSOR_JSON_CHANNEL_CAPACITY);

