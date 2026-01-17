#pragma once
#include <Arduino.h>
#include "DeviceStateMachine.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "BoardConfig.h"
class Board;

class Display {
 public:
  void Initialize(Board* board, DeviceStateMachineA* sm);
  void Poll();
 private:
  DeviceStateMachineA* sm_{nullptr};
  DeviceStateA last_{kDeviceStateUnknownA};
  unsigned long last_draw_ms_{0};
  const char* Name(DeviceStateA s) const;
 #if BOARD_DISPLAY_HEIGHT == 32
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_{U8G2_R0, U8X8_PIN_NONE};
 #else
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_{U8G2_R0, U8X8_PIN_NONE};
 #endif
};

