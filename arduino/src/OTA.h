#pragma once
#include <Arduino.h>

class OTA {
 public:
  bool UpgradeFromUrl(const String& url, const String& version);
  bool CheckAndUpgrade(const String& check_url, const String& current_version, String& new_version, String& new_url);
};
