#pragma once
#include <Arduino.h>

namespace Config {
  static const char* WIFI_SSID = "";
  static const char* WIFI_PASSWORD = "";
  static const char* MQTT_HOST = "";
  static const uint16_t MQTT_PORT = 1883;
  static const char* MQTT_CLIENT_ID = "xiaozhi-device";
  static const char* MQTT_TOPIC_UP = "xiaozhi/up";
  static const char* MQTT_TOPIC_DOWN = "xiaozhi/down";
  static const char* WS_URL = "";
  static const char* FW_VERSION = "1.0.0";
  static const char* OTA_CHECK_URL = "";
  static const char* DEVICE_ID = "bread-compact-wifi";
  static const char* CONFIG_URL = "";
  static const int AUDIO_SAMPLE_RATE = 16000;
  static const int AUDIO_FRAME_MS = 20;
  static const int I2S_PORT = 0;
  static const int I2S_PIN_BCLK = -1;
  static const int I2S_PIN_LRCLK = -1;
  static const int I2S_PIN_DIN = -1;
}
