#include "soil_reader.h"

#include <Wire.h>

void SoilReader::begin() {
  for (uint8_t adsIndex = 0; adsIndex < ADS_COUNT; ++adsIndex) {
    const uint8_t address = pgm_read_byte(&ADS_ADDRESSES[adsIndex]);
    online_[adsIndex] = ads_[adsIndex].begin(address, &Wire);
    if (online_[adsIndex]) {
      ads_[adsIndex].setGain(GAIN_ONE);
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
  const int32_t dry = static_cast<int16_t>(pgm_read_word(&dryRaw[sensorIndex]));
  const int32_t wet = static_cast<int16_t>(pgm_read_word(&wetRaw[sensorIndex]));
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

