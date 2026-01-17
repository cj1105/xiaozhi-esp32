#include "WiFiService.h"

void WiFiService::Begin(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

bool WiFiService::IsConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiService::Poll() {
  // if (WiFi.getMode() != WIFI_STA) {
  //   WiFi.mode(WIFI_STA);
  // }
}
