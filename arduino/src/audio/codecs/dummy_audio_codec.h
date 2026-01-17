#pragma once
#include "audio/audio_codec.h"

class DummyAudioCodec : public AudioCodec {
 public:
  DummyAudioCodec(int input_sample_rate, int output_sample_rate);

 protected:
  int Read(int16_t* dest, int samples) override;
  int Write(const int16_t* data, int samples) override;
};

