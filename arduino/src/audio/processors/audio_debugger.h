#pragma once
#include <atomic>
#include "audio/audio_processor.h"

class AudioDebugger : public AudioProcessor {
 public:
  void Initialize(AudioCodec* codec, int frame_duration_ms, srmodel_list_t* models_list) override;
  void Feed(std::vector<int16_t>&& data) override;
  void Start() override;
  void Stop() override;
  bool IsRunning() override;
  void OnOutput(std::function<void(std::vector<int16_t>&& data)> callback) override;
  void OnVadStateChange(std::function<void(bool speaking)> callback) override;
  size_t GetFeedSize() override;
  void EnableDeviceAec(bool enable) override;

 private:
  std::function<void(std::vector<int16_t>&&)> on_output_;
  std::function<void(bool)> on_vad_;
  std::atomic<bool> running_{false};
  size_t feed_size_{0};
  unsigned long last_ms_{0};
};

