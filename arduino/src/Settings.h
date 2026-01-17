#pragma once
#include <Arduino.h>
#include <Preferences.h>

class Settings {
 public:
  void Begin(const char* ns);
  void SetString(const char* key, const String& value);
  String GetString(const char* key, const String& def = "");
  void SetInt(const char* key, int value);
  int GetInt(const char* key, int def = 0);
 private:
  Preferences prefs_;
};

