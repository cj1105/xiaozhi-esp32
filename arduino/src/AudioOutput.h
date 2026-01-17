#pragma once
#include <Arduino.h>
#include <vector>
#include "BoardConfig.h"

class AudioOutput {
 public:
  void Begin(int sample_rate);
  bool Open();
  void Close();
  bool IsOpened() const;
  bool PlayPCMFrame(const std::vector<uint8_t>& frame);
  bool PlayPcmFile(const char* path, int sample_rate);
  void Beep(int freq_hz, int duration_ms, int sample_rate);
 private:
  bool opened_{false};
  int rate_{16000};
  int port_{0};
  int bclk_{BOARD_I2S_PIN_SPK_BCLK};
  int lrclk_{BOARD_I2S_PIN_SPK_LRCK};
  int dout_{BOARD_I2S_PIN_DOUT};
};

