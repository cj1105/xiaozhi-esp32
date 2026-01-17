#pragma once
#include <Arduino.h>

class Adafruit_NeoPixel;

class Led {
 public:
  explicit Led(uint8_t pin);
  void Begin();
  void Set(bool on);
 private:
  uint8_t pin_;
  Adafruit_NeoPixel* pixel_{nullptr};
};

