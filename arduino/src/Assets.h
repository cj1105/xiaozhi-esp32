#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

class Assets {
 public:
  bool Begin();
  File Open(const char* path);
};

