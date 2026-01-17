#pragma once
#include <Arduino.h>
#include <vector>
#include "Config.h"
#include "BoardConfig.h"
#include <memory>
class AudioService {
 public:
  void Begin();
  bool Open();
  void Close();
  bool IsOpened() const;
  std::unique_ptr<std::vector<uint8_t>> CaptureFrame(int sample_rate, int frame_duration_ms);
 private:
  bool opened_{false};
  uint32_t last_ts_{0};
  bool use_i2s_{false};
  int port_{0};
  int rate_{Config::AUDIO_SAMPLE_RATE};
  int frame_ms_{Config::AUDIO_FRAME_MS};
  int bclk_{Config::I2S_PIN_BCLK};
  int lrclk_{Config::I2S_PIN_LRCLK};
  int din_{Config::I2S_PIN_DIN};
};
