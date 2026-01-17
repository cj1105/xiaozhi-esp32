#include "Settings.h"

void Settings::Begin(const char* ns) {
  prefs_.begin(ns, false);
}

void Settings::SetString(const char* key, const String& value) {
  prefs_.putString(key, value);
}

String Settings::GetString(const char* key, const String& def) {
  if (!prefs_.isKey(key)) return def;
  return prefs_.getString(key, def);
}

void Settings::SetInt(const char* key, int value) {
  prefs_.putInt(key, value);
}

int Settings::GetInt(const char* key, int def) {
  if (!prefs_.isKey(key)) return def;
  return prefs_.getInt(key, def);
}
