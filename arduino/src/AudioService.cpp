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
#if defined(BOARD_I2S_MIC_PORT)
  port_ = BOARD_I2S_MIC_PORT;
#endif
  use_i2s_ = (bclk_ >= 0 && lrclk_ >= 0 && din_ >= 0);
  if (use_i2s_) {
    i2s_config_t cfg{};
    cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
    cfg.sample_rate = rate_;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.intr_alloc_flags = 0;
    cfg.dma_buf_count = 4;
    cfg.dma_buf_len = 256;
    cfg.use_apll = false;
    cfg.tx_desc_auto_clear = true;
    esp_err_t err = i2s_driver_install((i2s_port_t)port_, &cfg, 0, NULL);
    if (err != ESP_OK) {
      Serial.printf("[AUDIO] MIC i2s_driver_install failed: %d\n", (int)err);
      use_i2s_ = false;
      return;
    }
    i2s_pin_config_t pins{};
    pins.bck_io_num = bclk_;
    pins.ws_io_num = lrclk_;
    pins.data_out_num = I2S_PIN_NO_CHANGE;
    pins.data_in_num = din_;
    err = i2s_set_pin((i2s_port_t)port_, &pins);
    if (err != ESP_OK) {
      Serial.printf("[AUDIO] MIC i2s_set_pin failed: %d\n", (int)err);
      use_i2s_ = false;
      return;
    }
    i2s_zero_dma_buffer((i2s_port_t)port_);
    i2s_start((i2s_port_t)port_);
    Serial.printf("[AUDIO] MIC I2S ok port=%d bclk=%d lrck=%d din=%d rate=%d\n", port_, bclk_, lrclk_, din_, rate_);
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
    std::vector<int32_t> in32;
    in32.resize(samples);
    size_t read_bytes = 0;
    esp_err_t err = i2s_read((i2s_port_t)port_, in32.data(), in32.size() * sizeof(int32_t), &read_bytes, 10);
    if (err != ESP_OK) return nullptr;
    int got = (int)read_bytes / (int)sizeof(int32_t);
    int16_t* out16 = (int16_t*)buf->data();
    for (int i = 0; i < samples; ++i) {
      int32_t v = (i < got) ? (in32[i] >> 12) : 0;
      if (v > INT16_MAX) v = INT16_MAX;
      if (v < -INT16_MAX) v = -INT16_MAX;
      out16[i] = (int16_t)v;
    }
  } else {
    for (int i = 0; i < bytes; ++i) (*buf)[i] = (uint8_t)random(0, 256);
  }
  return buf;
}
