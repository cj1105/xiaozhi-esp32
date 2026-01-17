#include "Display.h"
#include "Board.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "BoardConfig.h"
#include <WiFi.h>

static uint8_t I2cScanFirst() {
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) return addr;
  }
  return 0;
}

void Display::Initialize(Board* board, DeviceStateMachineA* sm) {
  sm_ = sm;
  last_ = sm_ ? sm_->GetState() : kDeviceStateUnknownA;
  last_draw_ms_ = 0;
  pinMode(BOARD_I2C_SDA, INPUT_PULLUP);
  pinMode(BOARD_I2C_SCL, INPUT_PULLUP);
  Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
  Wire.setClock(400000);
  uint8_t found = I2cScanFirst();
  if (found != 0) {
    u8g2_.setI2CAddress(found << 1);
  } else {
    u8g2_.setI2CAddress(0x3C << 1);
  }
  u8g2_.begin();
  if (BOARD_DISPLAY_MIRROR_X && BOARD_DISPLAY_MIRROR_Y) {
    u8g2_.setFlipMode(1);
  } else {
    u8g2_.setFlipMode(0);
  }
  u8g2_.clearBuffer();
  u8g2_.setFont(u8g2_font_6x12_tf);
  u8g2_.drawStr(0, 12, "XIAOZHI OLED");
  if (found != 0) {
    char buf[20];
    snprintf(buf, sizeof(buf), "I2C:0x%02X", found);
    u8g2_.drawStr(0, 28, buf);
  } else {
    u8g2_.drawStr(0, 28, "I2C:NOT FOUND");
  }
  u8g2_.sendBuffer();
}

void Display::Poll() {
  if (!sm_) return;
  DeviceStateA cur = sm_->GetState();
  unsigned long now = millis();
  if (cur != last_ || (now - last_draw_ms_ >= 1000)) {
    u8g2_.clearBuffer();
    u8g2_.setFont(u8g2_font_6x12_tf);
    u8g2_.drawStr(0, 12, Name(cur));
    if (WiFi.status() == WL_CONNECTED) {
      String ip = WiFi.localIP().toString();
      u8g2_.drawStr(0, 28, ip.c_str());
    } else {
      u8g2_.drawStr(0, 28, "WiFi:DISCONNECTED");
    }
    u8g2_.sendBuffer();
    last_ = cur;
    last_draw_ms_ = now;
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

