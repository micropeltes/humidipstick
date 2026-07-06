# ESP-01 Soil Moisture Monitor

Humidipstick is a PlatformIO firmware project for an ESP-01/ESP8266 soil
moisture monitor. It reads up to 16 soil moisture channels through four ADS1115
ADC modules, shows device status and sensor values on an SSD1306 OLED display,
and publishes structured readings to an MQTT broker.

The project is intended for garden, greenhouse, hydroponic, and learning setups
where a small WiFi-connected board needs to monitor several analog moisture
sensors.

## Features

- ESP-01/ESP8266 firmware built with the Arduino framework and PlatformIO.
- Four ADS1115 modules on one I2C bus, with four channels each.
- Per-sensor dry and wet calibration values.
- Moisture conversion from raw ADC values to percentage values.
- MQTT publishing for sensor data and retained online/offline status.
- MQTT command handling for immediate reads, publish interval changes, and OLED
  page selection.
- SSD1306 OLED status pages for WiFi, MQTT, IP address, uptime, and ADS channel
  readings.
- Graceful startup when an ADS1115 module or OLED display is missing.

## Hardware

- ESP-01 or compatible ESP8266 board.
- Up to four ADS1115 ADC modules.
- Up to 16 analog soil moisture sensors.
- Optional SSD1306 OLED display.
- MQTT broker reachable from the ESP8266 network.

Default I2C wiring:

| Signal | ESP-01 pin |
| ------ | ---------- |
| SDA    | GPIO0      |
| SCL    | GPIO2      |

Default I2C addresses:

| Device | Address |
| ------ | ------- |
| ADS1115 #1 | `0x49` |
| ADS1115 #2 | `0x48` |
| ADS1115 #3 | `0x4B` |
| ADS1115 #4 | `0x4A` |
| OLED | `0x3C` |

## Configuration

Copy the example credentials file:

```sh
cp include/credentials.example.h include/credentials.h
```

Then edit `include/credentials.h` with your WiFi network, MQTT broker, MQTT
client ID, and MQTT base topic. OLED and sensor-bus settings live in
`include/config.h`.

Configure `ADS_ADDRESSES` in `include/config.h` with only the ADS1115 addresses
you may install; the firmware probes those addresses at boot and keeps running
when any subset is missing. Calibrate every sensor in `include/config.h` by
replacing the example `dryRaw` and `wetRaw` values with measurements from your
own sensors. The conversion supports both sensor types where dry readings are
higher than wet readings and the reverse. Set `OLED_ENABLED` and `OLED_ADDRESS`
in `include/config.h` to enable, disable, or move the optional display.

## Build and Upload

```sh
pio run
pio run -t upload
```

The default PlatformIO environment is `esp01_1m`.

## MQTT

Sensor readings are published to:

```text
<MQTT_BASE_TOPIC>/sensor
```

Device status is published to:

```text
<MQTT_BASE_TOPIC>/status
```

Commands are received from:

```text
<MQTT_BASE_TOPIC>/command
```

Example command payload:

```json
{
  "read_now": true,
  "interval_ms": 10000,
  "oled_page": 1
}
```

Command fields:

- `read_now`: request an immediate sensor read and publish.
- `interval_ms`: set the publish interval, constrained by the minimum and
  maximum values in `include/config.h`.
- `oled_page`: select the OLED page. Page `0` is system status, and the
  remaining pages show ADS module readings.

## License

This project is provided under a custom non-commercial license.

You may:

- Try, run, and test this project for personal purposes.
- Study the source code and learn from the implementation.
- Modify the project for private, educational, research, or evaluation use.

You may not, without prior written permission from the project owner:

- Use this project, or a modified version of it, for commercial purposes.
- Sell, rent, sublicense, or commercially distribute this project.
- Integrate this project into a paid product, paid service, client project, or
  revenue-generating system.
- Use this project to provide paid installation, monitoring, automation, or
  consulting services.

All rights not explicitly granted above are reserved by the project owner. This
custom license is intended to allow experimentation and learning while
prohibiting commercial use.
