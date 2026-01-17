#include "audio/processors/no_audio_processor.h"

void NoAudioProcessor::Initialize(AudioCodec* codec, int frame_duration_ms, srmodel_list_t* models_list) {
  (void)codec;
  (void)models_list;
  int rate = codec ? codec->input_sample_rate() : 16000;
  feed_size_ = (size_t)(rate * frame_duration_ms / 1000);
}

void NoAudioProcessor::Feed(std::vector<int16_t>&& data) {
  if (!running_.load()) return;
  if (on_vad_) on_vad_(true);
  if (on_output_) on_output_(std::move(data));
}

void NoAudioProcessor::Start() {
  running_.store(true);
}

void NoAudioProcessor::Stop() {
  running_.store(false);
  if (on_vad_) on_vad_(false);
}

bool NoAudioProcessor::IsRunning() {
  return running_.load();
}

void NoAudioProcessor::OnOutput(std::function<void(std::vector<int16_t>&& data)> callback) {
  on_output_ = std::move(callback);
}

void NoAudioProcessor::OnVadStateChange(std::function<void(bool speaking)> callback) {
  on_vad_ = std::move(callback);
}

size_t NoAudioProcessor::GetFeedSize() {
  return feed_size_;
}

void NoAudioProcessor::EnableDeviceAec(bool enable) {
  (void)enable;
}

