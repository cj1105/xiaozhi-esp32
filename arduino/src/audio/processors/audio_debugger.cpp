#include "audio/processors/audio_debugger.h"
#include <Arduino.h>
#include <cmath>

void AudioDebugger::Initialize(AudioCodec* codec, int frame_duration_ms, srmodel_list_t* models_list) {
  (void)models_list;
  int rate = codec ? codec->input_sample_rate() : 16000;
  feed_size_ = (size_t)(rate * frame_duration_ms / 1000);
}

void AudioDebugger::Feed(std::vector<int16_t>&& data) {
  if (!running_.load()) return;
  unsigned long now = millis();
  if (now - last_ms_ >= 1000) {
    last_ms_ = now;
    long long sum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
      sum += llabs((long long)data[i]);
    }
    int avg = data.empty() ? 0 : (int)(sum / (long long)data.size());
    Serial.printf("[AUDIO] avg_abs=%d samples=%d\n", avg, (int)data.size());
  }
  if (on_output_) on_output_(std::move(data));
  if (on_vad_) on_vad_(true);
}

void AudioDebugger::Start() {
  running_.store(true);
  last_ms_ = 0;
}

void AudioDebugger::Stop() {
  running_.store(false);
  if (on_vad_) on_vad_(false);
}

bool AudioDebugger::IsRunning() {
  return running_.load();
}

void AudioDebugger::OnOutput(std::function<void(std::vector<int16_t>&& data)> callback) {
  on_output_ = std::move(callback);
}

void AudioDebugger::OnVadStateChange(std::function<void(bool speaking)> callback) {
  on_vad_ = std::move(callback);
}

size_t AudioDebugger::GetFeedSize() {
  return feed_size_;
}

void AudioDebugger::EnableDeviceAec(bool enable) {
  (void)enable;
}

