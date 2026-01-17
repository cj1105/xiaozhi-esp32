#include "Button.h"

Button::Button(uint8_t pin) : pin_(pin) {}

void Button::Begin() {
  pinMode(pin_, INPUT_PULLUP);
}

bool Button::IsPressed() const {
  return digitalRead(pin_) == LOW;
}

