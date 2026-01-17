#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiService {
 public:
  void Begin(const char* ssid, const char* password);
  bool IsConnected() const;
  void Poll();
};
