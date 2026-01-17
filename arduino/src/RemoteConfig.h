#pragma once
#include <Arduino.h>
#include "Settings.h"

class RemoteConfig {
 public:
  bool Fetch(const String& url, Settings* settings);
};

