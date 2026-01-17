#pragma once
#include "audio/wake_word.h"

class EspWakeWord : public WakeWord {
 public:
  EspWakeWord() = default;
  ~EspWakeWord() override = default;

  bool Initialize(AudioCodec* codec, srmodel_list_t* models_list) override;
  void Feed(const std::vector<int16_t>& data) override;
  void OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) override;
  void Start() override;
  void Stop() override;
  size_t GetFeedSize() override;
  void EncodeWakeWordData() override;
  bool GetWakeWordOpus(std::vector<uint8_t>& opus) override;
  const std::string& GetLastDetectedWakeWord() const override;

 private:
  std::function<void(const std::string&)> cb_;
  std::string last_;
  bool running_{false};
  size_t feed_size_{320};
};

