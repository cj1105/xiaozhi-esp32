#include "AudioOutput.h"
#include <FS.h>
#include <SPIFFS.h>
#include <math.h>
#include "driver/i2s.h"

void AudioOutput::Begin(int sample_rate) {
  rate_ = sample_rate;
 #if defined(BOARD_I2S_SPK_PORT)
  port_ = BOARD_I2S_SPK_PORT;
 #endif
  i2s_config_t cfg{};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
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
    Serial.printf("[AUDIO] SPK i2s_driver_install failed: %d\n", (int)err);
    return;
  }
  i2s_pin_config_t pins{};
  pins.bck_io_num = bclk_;
  pins.ws_io_num = lrclk_;
  pins.data_out_num = dout_;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  err = i2s_set_pin((i2s_port_t)port_, &pins);
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] SPK i2s_set_pin failed: %d\n", (int)err);
    return;
  }
  i2s_zero_dma_buffer((i2s_port_t)port_);
  i2s_start((i2s_port_t)port_);
  Serial.printf("[AUDIO] SPK I2S ok port=%d bclk=%d lrck=%d dout=%d rate=%d\n", port_, bclk_, lrclk_, dout_, rate_);
}

bool AudioOutput::Open() {
  opened_ = true;
  return true;
}

void AudioOutput::Close() {
  opened_ = false;
}

bool AudioOutput::IsOpened() const {
  return opened_;
}

bool AudioOutput::PlayPCMFrame(const std::vector<uint8_t>& frame) {
  if (!opened_) return false;
  int samples = (int)frame.size() / 2;
  if (samples <= 0) return false;
  std::vector<int32_t> out;
  out.resize(samples);
  const int16_t* in = (const int16_t*)frame.data();
  for (int i = 0; i < samples; ++i) {
    out[i] = ((int32_t)in[i]) << 12;
  }
  size_t written = 0;
  esp_err_t err = i2s_write((i2s_port_t)port_, out.data(), out.size() * sizeof(int32_t), &written, portMAX_DELAY);
  if (err != ESP_OK) return false;
  return written == out.size() * sizeof(int32_t);
}

bool AudioOutput::PlayPcmFile(const char* path, int sample_rate) {
  if (!SPIFFS.begin(true)) return false;
  File f = SPIFFS.open(path, "r");
  if (!f) return false;
  if (sample_rate != rate_) {
    rate_ = sample_rate;
    i2s_set_clk((i2s_port_t)port_, rate_, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO);
  }
  std::vector<uint8_t> buf;
  buf.resize(1024);
  while (f.available()) {
    int r = f.read(buf.data(), buf.size());
    if (r <= 0) break;
    std::vector<uint8_t> frame(buf.begin(), buf.begin() + r);
    if (!PlayPCMFrame(frame)) break;
    delay(1);
  }
  f.close();
  return true;
}

void AudioOutput::Beep(int freq_hz, int duration_ms, int sample_rate) {
  if (!opened_) Open();
  if (sample_rate != rate_) {
    rate_ = sample_rate;
    i2s_set_clk((i2s_port_t)port_, rate_, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO);
  }
  int total = (sample_rate * duration_ms) / 1000;
  int frame = 240;
  float phase = 0.0f;
  float step = 2.0f * 3.1415926f * ((float)freq_hz / (float)sample_rate);
  std::vector<uint8_t> pcm;
  pcm.resize(frame * 2);
  for (int i = 0; i < total; i += frame) {
    int n = frame;
    if (i + n > total) n = total - i;
    for (int j = 0; j < n; ++j) {
      int16_t s = (int16_t)(sinf(phase) * 12000.0f);
      phase += step;
      ((int16_t*)pcm.data())[j] = s;
    }
    std::vector<uint8_t> chunk(pcm.begin(), pcm.begin() + n * 2);
    PlayPCMFrame(chunk);
  }
}
