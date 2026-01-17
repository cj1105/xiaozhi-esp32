#pragma once
#include <Arduino.h>

class Button {
 public:
  explicit Button(uint8_t pin);
  void Begin();
  bool IsPressed() const;
 private:
  uint8_t pin_;
};

