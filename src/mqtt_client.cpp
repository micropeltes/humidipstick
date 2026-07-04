#include "mqtt_client.h"

#include <cstring>

MqttClient* MqttClient::instance_ = nullptr;

void MqttClient::begin(const char* host, uint16_t port, const char* user,
                       const char* password, const char* clientId,
                       const char* baseTopic) {
  host_ = host;
  port_ = port;
  user_ = user;
  password_ = password;
  clientId_ = clientId;
  baseTopic_ = baseTopic;

  buildTopics();
  instance_ = this;
  client_.setServer(host_, port_);
  client_.setCallback(callbackThunk);

  Serial.printf("[MQTT] Broker configured: %s:%u\n", host_, port_);
}

void MqttClient::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (!client_.connected()) {
    const uint32_t now = millis();
    if (now - lastReconnectAttemptMs_ >= MQTT_RETRY_INTERVAL_MS) {
      lastReconnectAttemptMs_ = now;
      reconnect();
    }
    return;
  }

  client_.loop();
}

bool MqttClient::connected() {
  return client_.connected();
}

bool MqttClient::publishSensor(const JsonDocument& document) {
  if (!client_.connected()) {
    return false;
  }

  const size_t payloadLength = measureJson(document);
  if (!client_.beginPublish(sensorTopic_, payloadLength, false)) {
    return false;
  }

  const size_t written = serializeJson(document, client_);
  const bool ended = client_.endPublish();
  return ended && written == payloadLength;
}

void MqttClient::setCommandHandler(MqttCommandHandler handler) {
  commandHandler_ = handler;
}

void MqttClient::callbackThunk(char* topic, uint8_t* payload,
                               unsigned int length) {
  if (instance_ != nullptr) {
    instance_->handleCallback(topic, payload, length);
  }
}

void MqttClient::handleCallback(char* topic, uint8_t* payload,
                                unsigned int length) {
  Serial.printf("[MQTT] Message received on %s (%u bytes)\n", topic, length);
  if (strcmp(topic, commandTopic_) == 0 && commandHandler_ != nullptr) {
    commandHandler_(payload, length);
  }
}

void MqttClient::reconnect() {
  Serial.printf("[MQTT] Connecting as %s...\n", clientId_);

  bool connected = false;
  if (user_ != nullptr && user_[0] != '\0') {
    connected =
        client_.connect(clientId_, user_, password_, statusTopic_, 0, true,
                        "offline");
  } else {
    connected =
        client_.connect(clientId_, statusTopic_, 0, true, "offline");
  }

  if (!connected) {
    Serial.printf("[MQTT] Connect failed, state=%d\n", client_.state());
    return;
  }

  Serial.println(F("[MQTT] Connected"));
  client_.publish(statusTopic_, "online", true);
  if (client_.subscribe(commandTopic_)) {
    Serial.printf("[MQTT] Subscribed to %s\n", commandTopic_);
  } else {
    Serial.println(F("[MQTT] Command subscription failed"));
  }
}

void MqttClient::buildTopics() {
  snprintf(sensorTopic_, sizeof(sensorTopic_), "%s/sensor", baseTopic_);
  snprintf(statusTopic_, sizeof(statusTopic_), "%s/status", baseTopic_);
  snprintf(commandTopic_, sizeof(commandTopic_), "%s/command", baseTopic_);
}

