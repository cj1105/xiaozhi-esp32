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
