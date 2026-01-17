#include "audio/codecs/dummy_audio_codec.h"
#include <Arduino.h>

DummyAudioCodec::DummyAudioCodec(int input_sample_rate, int output_sample_rate) {
  duplex_ = true;
  input_sample_rate_ = input_sample_rate;
  output_sample_rate_ = output_sample_rate;
}

int DummyAudioCodec::Read(int16_t* dest, int samples) {
  for (int i = 0; i < samples; ++i) {
    dest[i] = (int16_t)random(-300, 300);
  }
  return samples;
}

int DummyAudioCodec::Write(const int16_t* data, int samples) {
  (void)data;
  return samples;
}

