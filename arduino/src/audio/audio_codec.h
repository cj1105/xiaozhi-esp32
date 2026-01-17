#pragma once
#include <Arduino.h>
#include <vector>

class AudioCodec {
 public:
  virtual ~AudioCodec() = default;

  virtual void SetOutputVolume(int volume) { output_volume_ = volume; }
  virtual void SetInputGain(float gain) { input_gain_ = gain; }
  virtual void EnableInput(bool enable) { input_enabled_ = enable; }
  virtual void EnableOutput(bool enable) { output_enabled_ = enable; }

  virtual void OutputData(std::vector<int16_t>& data) { Write(data.data(), (int)data.size()); }
  virtual bool InputData(std::vector<int16_t>& data) { return Read(data.data(), (int)data.size()) > 0; }
  virtual void Start() { EnableInput(true); EnableOutput(true); }

  bool duplex() const { return duplex_; }
  bool input_reference() const { return input_reference_; }
  int input_sample_rate() const { return input_sample_rate_; }
  int output_sample_rate() const { return output_sample_rate_; }
  int input_channels() const { return input_channels_; }
  int output_channels() const { return output_channels_; }
  int output_volume() const { return output_volume_; }
  float input_gain() const { return input_gain_; }
  bool input_enabled() const { return input_enabled_; }
  bool output_enabled() const { return output_enabled_; }

 protected:
  bool duplex_ = false;
  bool input_reference_ = false;
  bool input_enabled_ = false;
  bool output_enabled_ = false;
  int input_sample_rate_ = 0;
  int output_sample_rate_ = 0;
  int input_channels_ = 1;
  int output_channels_ = 1;
  int output_volume_ = 70;
  float input_gain_ = 0.0f;

  virtual int Read(int16_t* dest, int samples) = 0;
  virtual int Write(const int16_t* data, int samples) = 0;
};

