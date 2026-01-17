#pragma once
#include <driver/i2s.h>
#include <mutex>
#include "audio/audio_codec.h"

#define AUDIO_CODEC_DMA_DESC_NUM 6
#define AUDIO_CODEC_DMA_FRAME_NUM 240

class NoAudioCodec : public AudioCodec {
 public:
  ~NoAudioCodec() override;

 protected:
  std::mutex data_if_mutex_;
};

class NoAudioCodecDuplex : public NoAudioCodec {
 public:
  NoAudioCodecDuplex(int input_sample_rate, int output_sample_rate, int bclk, int ws, int dout, int din);

 protected:
  int Read(int16_t* dest, int samples) override;
  int Write(const int16_t* data, int samples) override;

 private:
  i2s_port_t port_{I2S_NUM_0};
};

class NoAudioCodecSimplex : public NoAudioCodec {
 public:
  NoAudioCodecSimplex(int input_sample_rate, int output_sample_rate, int spk_bclk, int spk_ws, int spk_dout, int mic_sck, int mic_ws, int mic_din);

 protected:
  int Read(int16_t* dest, int samples) override;
  int Write(const int16_t* data, int samples) override;

 private:
  i2s_port_t tx_port_{I2S_NUM_0};
  i2s_port_t rx_port_{I2S_NUM_1};
};

