#include "Display.h"
#include "Board.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "BoardConfig.h"

void Display::Initialize(Board* board, DeviceStateMachineA* sm) {
  sm_ = sm;
  last_ = sm_ ? sm_->GetState() : kDeviceStateUnknownA;
  Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
  u8g2_.begin();
  if (BOARD_DISPLAY_MIRROR_X && BOARD_DISPLAY_MIRROR_Y) {
    u8g2_.setFlipMode(1);
  } else {
    u8g2_.setFlipMode(0);
  }
  u8g2_.clearBuffer();
  u8g2_.setFont(u8g2_font_6x12_tf);
  u8g2_.drawStr(0, 12, "XIAOZHI");
  u8g2_.sendBuffer();
}

void Display::Poll() {
  if (!sm_) return;
  DeviceStateA cur = sm_->GetState();
  if (cur != last_) {
    u8g2_.clearBuffer();
    u8g2_.setFont(u8g2_font_6x12_tf);
    u8g2_.drawStr(0, 12, Name(cur));
    u8g2_.sendBuffer();
    last_ = cur;
  }
}

const char* Display::Name(DeviceStateA s) const {
  switch (s) {
    case kDeviceStateUnknownA: return "Unknown";
    case kDeviceStateStartingA: return "Starting";
    case kDeviceStateWifiConfiguringA: return "WiFiConfiguring";
    case kDeviceStateIdleA: return "Idle";
    case kDeviceStateConnectingA: return "Connecting";
    case kDeviceStateListeningA: return "Listening";
    case kDeviceStateSpeakingA: return "Speaking";
    case kDeviceStateUpgradingA: return "Upgrading";
    case kDeviceStateActivatingA: return "Activating";
    case kDeviceStateAudioTestingA: return "AudioTesting";
    case kDeviceStateFatalErrorA: return "FatalError";
    default: return "Unknown";
  }
}

