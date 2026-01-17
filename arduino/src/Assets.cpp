#include "Assets.h"
#include "esp_log.h"

bool Assets::Begin() {
  esp_log_level_t prev = esp_log_level_get("SPIFFS");
  esp_log_level_set("SPIFFS", ESP_LOG_NONE);
  bool ok = SPIFFS.begin(true);
  esp_log_level_set("SPIFFS", prev);
  return ok;
}

File Assets::Open(const char* path) {
  return SPIFFS.open(path, "r");
}

bool Assets::Exists(const char* path) {
  return SPIFFS.exists(path);
}

bool Assets::Save(const char* path, const uint8_t* data, size_t len) {
  File f = SPIFFS.open(path, "w");
  if (!f) return false;
  size_t w = f.write(data, len);
  f.close();
  return w == len;
}
