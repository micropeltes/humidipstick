#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

using MqttCommandHandler = void (*)(const uint8_t* payload, size_t length);

class MqttClient {
 public:
  void begin(const char* host, uint16_t port, const char* user,
             const char* password, const char* clientId,
             const char* baseTopic);
  void loop();
  bool connected();
  bool publishSensor(const JsonDocument& document);
  void setCommandHandler(MqttCommandHandler handler);

 private:
  static void callbackThunk(char* topic, uint8_t* payload,
                            unsigned int length);
  void handleCallback(char* topic, uint8_t* payload, unsigned int length);
  void reconnect();
  void buildTopics();

  WiFiClient wifiClient_;
  PubSubClient client_{wifiClient_};
  const char* host_ = nullptr;
  uint16_t port_ = 1883;
  const char* user_ = nullptr;
  const char* password_ = nullptr;
  const char* clientId_ = nullptr;
  const char* baseTopic_ = nullptr;
  char sensorTopic_[128] = {};
  char statusTopic_[128] = {};
  char commandTopic_[128] = {};
  uint32_t lastReconnectAttemptMs_ = 0;
  MqttCommandHandler commandHandler_ = nullptr;
  static MqttClient* instance_;
};

