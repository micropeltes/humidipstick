#pragma once

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "soil_reader.h"

class OledUi {
 public:
  OledUi();

  bool begin(uint8_t address);
  void update(const SoilReader& soilReader, bool mqttConnected);
  void setPage(uint8_t page);
  bool available() const;

 private:
  void drawStatusPage(bool mqttConnected);
  void drawAdsPage(const SoilReader& soilReader, bool mqttConnected,
                   uint8_t adsIndex);
  void printConnectionFlags(bool mqttConnected);

  Adafruit_SSD1306 display_;
  uint8_t address_ = 0;
  uint8_t page_ = 0;
  bool available_ = false;
  uint32_t lastPageChangeMs_ = 0;
  uint32_t lastRefreshMs_ = 0;
};

