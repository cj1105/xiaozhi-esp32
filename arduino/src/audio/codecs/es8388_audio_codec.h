#pragma once
#include "audio/codecs/no_audio_codec.h"

class Es8388AudioCodec : public NoAudioCodecDuplex {
 public:
  Es8388AudioCodec(void* i2c_master_handle, int i2c_port, int input_sample_rate, int output_sample_rate,
                   int mclk, int bclk, int ws, int dout, int din,
                   int pa_pin, uint8_t es8388_addr, bool use_mclk = true);
};

