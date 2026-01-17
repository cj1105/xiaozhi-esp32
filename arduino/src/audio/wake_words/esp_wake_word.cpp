#include "audio/wake_words/esp_wake_word.h"

bool EspWakeWord::Initialize(AudioCodec* codec, srmodel_list_t* models_list) {
  (void)codec;
  (void)models_list;
  return true;
}

void EspWakeWord::Feed(const std::vector<int16_t>& data) {
  (void)data;
}

void EspWakeWord::OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) {
  cb_ = std::move(callback);
}

void EspWakeWord::Start() {
  running_ = true;
}

void EspWakeWord::Stop() {
  running_ = false;
}

size_t EspWakeWord::GetFeedSize() {
  return feed_size_;
}

void EspWakeWord::EncodeWakeWordData() {
}

bool EspWakeWord::GetWakeWordOpus(std::vector<uint8_t>& opus) {
  (void)opus;
  return false;
}

const std::string& EspWakeWord::GetLastDetectedWakeWord() const {
  return last_;
}

