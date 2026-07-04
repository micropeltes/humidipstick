#pragma once

#include <Arduino.h>

constexpr uint8_t I2C_SDA_PIN = 0;
constexpr uint8_t I2C_SCL_PIN = 2;
constexpr uint32_t I2C_CLOCK_HZ = 100000;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

constexpr uint8_t ADS_COUNT = 4;
constexpr uint8_t CHANNELS_PER_ADS = 4;
constexpr uint8_t SENSOR_COUNT = ADS_COUNT * CHANNELS_PER_ADS;

// ADS1=ADDR->VCC, ADS2=ADDR->GND, ADS3=ADDR->SCL, ADS4=ADDR->SDA.
constexpr uint8_t ADS_ADDRESSES[ADS_COUNT] = {0x49, 0x48, 0x4B, 0x4A};

// Replace these example values with calibration measurements for every sensor.
// dryRaw may be greater than wetRaw; the conversion formula handles either order.
constexpr int16_t dryRaw[SENSOR_COUNT] = {
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
    22000, 22000, 22000, 22000,
};

constexpr int16_t wetRaw[SENSOR_COUNT] = {
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

constexpr uint8_t OLED_WIDTH = 128;
constexpr uint8_t OLED_HEIGHT = 64;
constexpr uint8_t OLED_PAGE_COUNT = ADS_COUNT + 1;

// Sized for metadata plus four ADS objects with sixteen channel readings.
constexpr size_t SENSOR_JSON_CAPACITY = 4096;

