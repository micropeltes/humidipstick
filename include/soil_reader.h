#pragma once

#include <Adafruit_ADS1X15.h>
#include <Arduino.h>

#include "config.h"

struct SoilReading {
  int16_t raw = 0;
  float voltage = 0.0F;
  float percent = 0.0F;
  bool valid = false;
};

class SoilReader {
 public:
  void begin();
  void readAll();
  bool isOnline(uint8_t adsIndex) const;
  const SoilReading* readings() const;
  const SoilReading& reading(uint8_t adsIndex, uint8_t channel) const;

 private:
  float rawToPercent(int16_t raw, uint8_t sensorIndex) const;

  Adafruit_ADS1115 ads_[ADS_COUNT];
  bool online_[ADS_COUNT] = {};
  SoilReading readings_[SENSOR_COUNT];
};

