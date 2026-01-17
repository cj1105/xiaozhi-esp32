#pragma once
#include "audio/codecs/no_audio_codec.h"

class BoxAudioCodec : public NoAudioCodecDuplex {
 public:
  BoxAudioCodec(void* i2c_master_handle, int input_sample_rate, int output_sample_rate,
                int mclk, int bclk, int ws, int dout, int din,
                int pa_pin, uint8_t es8311_addr, uint8_t es7210_addr, bool input_reference);
};

