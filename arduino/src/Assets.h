#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

class Assets {
 public:
  bool Begin();
  File Open(const char* path);
  bool Exists(const char* path);
  bool Save(const char* path, const uint8_t* data, size_t len);
};
