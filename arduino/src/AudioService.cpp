#include "AudioService.h"
#include <stdlib.h>
#include <memory>
#include "driver/i2s.h"

void AudioService::Begin() {
  opened_ = false;
  last_ts_ = millis();
  port_ = Config::I2S_PORT;
  rate_ = Config::AUDIO_SAMPLE_RATE;
  frame_ms_ = Config::AUDIO_FRAME_MS;
  bclk_ = Config::I2S_PIN_BCLK;
  lrclk_ = Config::I2S_PIN_LRCLK;
  din_ = Config::I2S_PIN_DIN;
  // Override with Board-specific pins when available
#if defined(BOARD_I2S_PIN_BCLK)
  bclk_ = BOARD_I2S_PIN_BCLK;
#endif
#if defined(BOARD_I2S_PIN_LRCLK)
  lrclk_ = BOARD_I2S_PIN_LRCLK;
#endif
#if defined(BOARD_I2S_PIN_DIN)
  din_ = BOARD_I2S_PIN_DIN;
#endif
  use_i2s_ = (bclk_ >= 0 && lrclk_ >= 0 && din_ >= 0);
  if (use_i2s_) {
    i2s_config_t cfg{};
    cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
    cfg.sample_rate = rate_;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.intr_alloc_flags = 0;
    cfg.dma_buf_count = 4;
    cfg.dma_buf_len = 256;
    cfg.use_apll = false;
    i2s_driver_install((i2s_port_t)port_, &cfg, 0, NULL);
    i2s_pin_config_t pins{};
    pins.bck_io_num = bclk_;
    pins.ws_io_num = lrclk_;
    pins.data_out_num = I2S_PIN_NO_CHANGE;
    pins.data_in_num = din_;
    i2s_set_pin((i2s_port_t)port_, &pins);
    i2s_set_sample_rates((i2s_port_t)port_, rate_);
  }
}

bool AudioService::Open() {
  opened_ = true;
  last_ts_ = millis();
  return true;
}

void AudioService::Close() {
  opened_ = false;
}

bool AudioService::IsOpened() const {
  return opened_;
}

std::unique_ptr<std::vector<uint8_t>> AudioService::CaptureFrame(int sample_rate, int frame_duration_ms) {
  if (!opened_) return nullptr;
  int samples = sample_rate * frame_duration_ms / 1000;
  int bytes = samples * 2;
  auto buf = std::make_unique<std::vector<uint8_t>>();
  buf->resize(bytes);
  if (use_i2s_) {
    size_t read_bytes = 0;
    i2s_read((i2s_port_t)port_, buf->data(), bytes, &read_bytes, 10);
    if ((int)read_bytes < bytes) {
      for (int i = (int)read_bytes; i < bytes; ++i) (*buf)[i] = 0;
    }
  } else {
    for (int i = 0; i < bytes; ++i) (*buf)[i] = (uint8_t)random(0, 256);
  }
  return buf;
}
