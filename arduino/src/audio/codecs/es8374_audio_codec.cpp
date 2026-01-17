#include "audio/codecs/es8374_audio_codec.h"

Es8374AudioCodec::Es8374AudioCodec(void* i2c_master_handle, int i2c_port, int input_sample_rate, int output_sample_rate,
                                   int mclk, int bclk, int ws, int dout, int din,
                                   int pa_pin, uint8_t es8374_addr, bool use_mclk)
    : NoAudioCodecDuplex(input_sample_rate, output_sample_rate, bclk, ws, dout, din) {
  (void)i2c_master_handle;
  (void)i2c_port;
  (void)mclk;
  (void)pa_pin;
  (void)es8374_addr;
  (void)use_mclk;
}

