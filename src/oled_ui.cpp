#include "oled_ui.h"

#include <ESP8266WiFi.h>
#include <Wire.h>

OledUi::OledUi()
    : display_(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}

bool OledUi::begin(uint8_t address) {
  address_ = address;

  Wire.beginTransmission(address_);
  if (Wire.endTransmission() != 0) {
    Serial.printf("[OLED] Not detected at 0x%02X; continuing without display\n",
                  address_);
    return false;
  }

  available_ = display_.begin(SSD1306_SWITCHCAPVCC, address_);
  if (!available_) {
    Serial.println(F("[OLED] SSD1306 allocation/init failed"));
    return false;
  }

  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  display_.setTextSize(1);
  display_.setCursor(0, 0);
  display_.println(F("ESP-01 Soil Monitor"));
  display_.println(F("Starting..."));
  display_.display();
  Serial.printf("[OLED] Online at 0x%02X\n", address_);
  return true;
}

void OledUi::update(const SoilReader& soilReader, bool mqttConnected) {
  if (!available_) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastPageChangeMs_ >= OLED_PAGE_INTERVAL_MS) {
    page_ = (page_ + 1) % OLED_PAGE_COUNT;
    lastPageChangeMs_ = now;
  }

  if (now - lastRefreshMs_ < 500) {
    return;
  }
  lastRefreshMs_ = now;

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);

  if (page_ == 0) {
    drawStatusPage(mqttConnected);
  } else {
    drawAdsPage(soilReader, mqttConnected, page_ - 1);
  }

  display_.display();
}

void OledUi::setPage(uint8_t page) {
  page_ = page < OLED_PAGE_COUNT ? page : 0;
  lastPageChangeMs_ = millis();
  lastRefreshMs_ = 0;
}

bool OledUi::available() const {
  return available_;
}

void OledUi::drawStatusPage(bool mqttConnected) {
  display_.println(F("SYSTEM STATUS"));
  display_.print(F("WiFi: "));
  display_.println(WiFi.status() == WL_CONNECTED ? F("ONLINE") : F("OFFLINE"));
  display_.print(F("MQTT: "));
  display_.println(mqttConnected ? F("ONLINE") : F("OFFLINE"));
  display_.print(F("RSSI: "));
  if (WiFi.status() == WL_CONNECTED) {
    display_.print(WiFi.RSSI());
    display_.println(F(" dBm"));
  } else {
    display_.println(F("--"));
  }
  display_.println(F("IP:"));
  if (WiFi.status() == WL_CONNECTED) {
    display_.println(WiFi.localIP());
  } else {
    display_.println(F("0.0.0.0"));
  }
  display_.print(F("Up: "));
  display_.print(millis() / 1000);
  display_.println(F(" s"));
}

void OledUi::drawAdsPage(const SoilReader& soilReader, bool mqttConnected,
                         uint8_t adsIndex) {
  display_.printf("ADS%u 0x%02X %s\n", adsIndex + 1,
                  ADS_ADDRESSES[adsIndex],
                  soilReader.isOnline(adsIndex) ? "OK" : "OFFLINE");
  printConnectionFlags(mqttConnected);

  for (uint8_t channel = 0; channel < CHANNELS_PER_ADS; ++channel) {
    const SoilReading& reading = soilReader.reading(adsIndex, channel);
    if (reading.valid) {
      display_.printf("CH%u %5d %5.1f%%\n", channel, reading.raw,
                      reading.percent);
    } else {
      display_.printf("CH%u -----  --.-%%\n", channel);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    display_.print(WiFi.localIP());
  } else {
    display_.print(F("IP 0.0.0.0"));
  }
}

void OledUi::printConnectionFlags(bool mqttConnected) {
  display_.print(F("W:"));
  display_.print(WiFi.status() == WL_CONNECTED ? F("OK") : F("--"));
  display_.print(F(" M:"));
  display_.println(mqttConnected ? F("OK") : F("--"));
}

