#pragma once
#include <Arduino.h>
#include "Led.h"
#include "Button.h"
#include "BoardConfig.h"

class Board {
 public:
  void Initialize();
  void Poll();
  void SetLed(bool on);
 private:
  Led led_{BOARD_LED_PIN};
  Button btn_{BOARD_BTN_BOOT};
};

