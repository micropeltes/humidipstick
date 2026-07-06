#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

#include "config.h"
#include "credentials.h"
#include "mqtt_client.h"
#include "oled_ui.h"
#include "soil_reader.h"

namespace {

SoilReader soilReader;
MqttClient mqttClient;
OledUi oledUi;

uint32_t publishIntervalMs = DEFAULT_PUBLISH_INTERVAL_MS;
uint32_t lastPublishMs = 0;
uint32_t lastWifiAttemptMs = 0;
bool publishRequested = true;

void startWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWifiAttemptMs = millis();
}

void maintainWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastWifiAttemptMs >= WIFI_RETRY_INTERVAL_MS) {
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    lastWifiAttemptMs = now;
  }
}

void handleMqttCommand(const uint8_t* payload, size_t length) {
  StaticJsonDocument<192> command;
  const DeserializationError error = deserializeJson(command, payload, length);
  if (error) {
    return;
  }

  if (command["read_now"] | false) {
    publishRequested = true;
  }

  if (command.containsKey("interval_ms")) {
    const uint32_t requestedInterval = command["interval_ms"].as<uint32_t>();
    publishIntervalMs =
        constrain(requestedInterval, MIN_PUBLISH_INTERVAL_MS, MAX_PUBLISH_INTERVAL_MS);
  }

  if (command.containsKey("oled_page")) {
    const int requestedPage = command["oled_page"].as<int>();
    oledUi.setPage(static_cast<uint8_t>(
        constrain(requestedPage, 0, static_cast<int>(OLED_PAGE_COUNT - 1))));
  }
}

void publishSensorData() {
  soilReader.readAll();

  StaticJsonDocument<SENSOR_JSON_CAPACITY> document;
  document["device_id"] = MQTT_CLIENT_ID;
  document["wifi_rssi"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
  document["uptime_ms"] = millis();

  JsonArray adsArray = document.createNestedArray("ads");
  const SoilReading* readings = soilReader.readings();

  for (uint8_t adsIndex = 0; adsIndex < ADS_COUNT; ++adsIndex) {
    JsonObject adsObject = adsArray.createNestedObject();
    adsObject["id"] = adsIndex + 1;

    char addressText[5];
    const uint8_t address = pgm_read_byte(&ADS_ADDRESSES[adsIndex]);
    snprintf(addressText, sizeof(addressText), "0x%02X", address);
    adsObject["address"] = addressText;
    adsObject["online"] = soilReader.isOnline(adsIndex);

    JsonArray channels = adsObject.createNestedArray("channels");
    for (uint8_t channel = 0; channel < CHANNELS_PER_ADS; ++channel) {
      const SoilReading& reading =
          readings[(adsIndex * CHANNELS_PER_ADS) + channel];
      JsonObject channelObject = channels.createNestedObject();
      channelObject["ch"] = channel;
      channelObject["raw"] = reading.raw;
      channelObject["voltage"] = reading.voltage;
      channelObject["percent"] = reading.percent;
    }
  }

  if (document.overflowed()) {
    return;
  }

  mqttClient.publishSensor(document);
}

}  // namespace

void setup() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_HZ);
  soilReader.begin();
  if (OLED_ENABLED) {
    oledUi.begin();
  }

  mqttClient.begin(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASSWORD,
                   MQTT_CLIENT_ID, MQTT_BASE_TOPIC);
  mqttClient.setCommandHandler(handleMqttCommand);

  startWifi();
}

void loop() {
  maintainWifi();
  mqttClient.loop();

  const uint32_t now = millis();
  if (publishRequested || now - lastPublishMs >= publishIntervalMs) {
    publishRequested = false;
    lastPublishMs = now;
    publishSensorData();
  }

  if (OLED_ENABLED) {
    oledUi.update(soilReader, mqttClient.connected());
  }
  yield();
}

