#include "audio/codecs/no_audio_codec.h"
#include <vector>
#include <cmath>

static void InstallTx(i2s_port_t port, int sample_rate, int bclk, int ws, int dout) {
  i2s_config_t cfg{};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate = sample_rate;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = 0;
  cfg.dma_buf_count = AUDIO_CODEC_DMA_DESC_NUM;
  cfg.dma_buf_len = AUDIO_CODEC_DMA_FRAME_NUM;
  cfg.use_apll = false;
  cfg.tx_desc_auto_clear = true;
  i2s_driver_install(port, &cfg, 0, NULL);
  i2s_pin_config_t pins{};
  pins.bck_io_num = bclk;
  pins.ws_io_num = ws;
  pins.data_out_num = dout;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  i2s_set_pin(port, &pins);
  i2s_zero_dma_buffer(port);
  i2s_start(port);
}

static void InstallRx(i2s_port_t port, int sample_rate, int bclk, int ws, int din) {
  i2s_config_t cfg{};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
  cfg.sample_rate = sample_rate;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = 0;
  cfg.dma_buf_count = AUDIO_CODEC_DMA_DESC_NUM;
  cfg.dma_buf_len = AUDIO_CODEC_DMA_FRAME_NUM;
  cfg.use_apll = false;
  cfg.tx_desc_auto_clear = true;
  i2s_driver_install(port, &cfg, 0, NULL);
  i2s_pin_config_t pins{};
  pins.bck_io_num = bclk;
  pins.ws_io_num = ws;
  pins.data_out_num = I2S_PIN_NO_CHANGE;
  pins.data_in_num = din;
  i2s_set_pin(port, &pins);
  i2s_zero_dma_buffer(port);
  i2s_start(port);
}

NoAudioCodec::~NoAudioCodec() {
}

NoAudioCodecDuplex::NoAudioCodecDuplex(int input_sample_rate, int output_sample_rate, int bclk, int ws, int dout, int din) {
  duplex_ = true;
  input_sample_rate_ = input_sample_rate;
  output_sample_rate_ = output_sample_rate;
  InstallTx(port_, output_sample_rate_, bclk, ws, dout);
  InstallRx(port_, input_sample_rate_, bclk, ws, din);
}

int NoAudioCodecDuplex::Write(const int16_t* data, int samples) {
  std::lock_guard<std::mutex> lock(data_if_mutex_);
  std::vector<int32_t> buffer(samples);
  int32_t volume_factor = (int32_t)(pow((double)output_volume_ / 100.0, 2) * 65536.0);
  for (int i = 0; i < samples; i++) {
    int64_t temp = (int64_t)data[i] * volume_factor;
    if (temp > INT32_MAX) buffer[i] = INT32_MAX;
    else if (temp < INT32_MIN) buffer[i] = INT32_MIN;
    else buffer[i] = (int32_t)temp;
  }
  size_t bytes_written = 0;
  i2s_write(port_, buffer.data(), samples * sizeof(int32_t), &bytes_written, portMAX_DELAY);
  return (int)(bytes_written / sizeof(int32_t));
}

int NoAudioCodecDuplex::Read(int16_t* dest, int samples) {
  size_t bytes_read = 0;
  std::vector<int32_t> buffer(samples);
  i2s_read(port_, buffer.data(), samples * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  int got = (int)(bytes_read / sizeof(int32_t));
  for (int i = 0; i < got; i++) {
    int32_t value = buffer[i] >> 12;
    if (value > INT16_MAX) value = INT16_MAX;
    if (value < -INT16_MAX) value = -INT16_MAX;
    dest[i] = (int16_t)value;
  }
  return got;
}

NoAudioCodecSimplex::NoAudioCodecSimplex(int input_sample_rate, int output_sample_rate, int spk_bclk, int spk_ws, int spk_dout, int mic_sck, int mic_ws, int mic_din) {
  duplex_ = false;
  input_sample_rate_ = input_sample_rate;
  output_sample_rate_ = output_sample_rate;
  InstallTx(tx_port_, output_sample_rate_, spk_bclk, spk_ws, spk_dout);
  InstallRx(rx_port_, input_sample_rate_, mic_sck, mic_ws, mic_din);
}

int NoAudioCodecSimplex::Write(const int16_t* data, int samples) {
  std::lock_guard<std::mutex> lock(data_if_mutex_);
  std::vector<int32_t> buffer(samples);
  int32_t volume_factor = (int32_t)(pow((double)output_volume_ / 100.0, 2) * 65536.0);
  for (int i = 0; i < samples; i++) {
    int64_t temp = (int64_t)data[i] * volume_factor;
    if (temp > INT32_MAX) buffer[i] = INT32_MAX;
    else if (temp < INT32_MIN) buffer[i] = INT32_MIN;
    else buffer[i] = (int32_t)temp;
  }
  size_t bytes_written = 0;
  i2s_write(tx_port_, buffer.data(), samples * sizeof(int32_t), &bytes_written, portMAX_DELAY);
  return (int)(bytes_written / sizeof(int32_t));
}

int NoAudioCodecSimplex::Read(int16_t* dest, int samples) {
  size_t bytes_read = 0;
  std::vector<int32_t> buffer(samples);
  i2s_read(rx_port_, buffer.data(), samples * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  int got = (int)(bytes_read / sizeof(int32_t));
  for (int i = 0; i < got; i++) {
    int32_t value = buffer[i] >> 12;
    if (value > INT16_MAX) value = INT16_MAX;
    if (value < -INT16_MAX) value = -INT16_MAX;
    dest[i] = (int16_t)value;
  }
  return got;
}

