#include "audio/codecs/box_audio_codec.h"

BoxAudioCodec::BoxAudioCodec(void* i2c_master_handle, int input_sample_rate, int output_sample_rate,
                             int mclk, int bclk, int ws, int dout, int din,
                             int pa_pin, uint8_t es8311_addr, uint8_t es7210_addr, bool input_reference)
    : NoAudioCodecDuplex(input_sample_rate, output_sample_rate, bclk, ws, dout, din) {
  (void)i2c_master_handle;
  (void)mclk;
  (void)pa_pin;
  (void)es8311_addr;
  (void)es7210_addr;
  input_reference_ = input_reference;
}

