#include "soil_reader.h"

#include <Wire.h>

void SoilReader::begin() {
  for (uint8_t adsIndex = 0; adsIndex < ADS_COUNT; ++adsIndex) {
    online_[adsIndex] = ads_[adsIndex].begin(ADS_ADDRESSES[adsIndex], &Wire);
    if (online_[adsIndex]) {
      ads_[adsIndex].setGain(GAIN_ONE);
      Serial.printf("[ADS] ADS%u online at 0x%02X\n", adsIndex + 1,
                    ADS_ADDRESSES[adsIndex]);
    } else {
      Serial.printf("[ADS] ADS%u NOT detected at 0x%02X; continuing\n",
                    adsIndex + 1, ADS_ADDRESSES[adsIndex]);
    }
  }
}

void SoilReader::readAll() {
  for (uint8_t adsIndex = 0; adsIndex < ADS_COUNT; ++adsIndex) {
    for (uint8_t channel = 0; channel < CHANNELS_PER_ADS; ++channel) {
      const uint8_t sensorIndex = (adsIndex * CHANNELS_PER_ADS) + channel;
      SoilReading& result = readings_[sensorIndex];

      if (!online_[adsIndex]) {
        result = SoilReading{};
        continue;
      }

      result.raw = ads_[adsIndex].readADC_SingleEnded(channel);
      result.voltage = ads_[adsIndex].computeVolts(result.raw);
      result.percent = rawToPercent(result.raw, sensorIndex);
      result.valid = true;

      Serial.printf("[ADS%u CH%u] raw=%d voltage=%.3fV moisture=%.1f%%\n",
                    adsIndex + 1, channel, result.raw, result.voltage,
                    result.percent);
      yield();
    }
  }
}

bool SoilReader::isOnline(uint8_t adsIndex) const {
  return adsIndex < ADS_COUNT && online_[adsIndex];
}

const SoilReading* SoilReader::readings() const {
  return readings_;
}

const SoilReading& SoilReader::reading(uint8_t adsIndex,
                                      uint8_t channel) const {
  static const SoilReading invalidReading;
  if (adsIndex >= ADS_COUNT || channel >= CHANNELS_PER_ADS) {
    return invalidReading;
  }
  return readings_[(adsIndex * CHANNELS_PER_ADS) + channel];
}

float SoilReader::rawToPercent(int16_t raw, uint8_t sensorIndex) const {
  const int32_t dry = dryRaw[sensorIndex];
  const int32_t wet = wetRaw[sensorIndex];
  const int32_t span = wet - dry;

  if (span == 0) {
    return 0.0F;
  }

  // Equivalent to mapping dryRaw -> 0% and wetRaw -> 100%. This naturally
  // supports the common capacitive-sensor case where dryRaw > wetRaw.
  const float percent =
      (static_cast<float>(raw - dry) * 100.0F) / static_cast<float>(span);
  return constrain(percent, 0.0F, 100.0F);
}

