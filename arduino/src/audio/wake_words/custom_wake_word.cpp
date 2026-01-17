#include "audio/wake_words/custom_wake_word.h"

bool CustomWakeWord::Initialize(AudioCodec* codec, srmodel_list_t* models_list) {
  (void)codec;
  (void)models_list;
  return true;
}

void CustomWakeWord::Feed(const std::vector<int16_t>& data) {
  (void)data;
}

void CustomWakeWord::OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) {
  cb_ = std::move(callback);
}

void CustomWakeWord::Start() {
  running_ = true;
}

void CustomWakeWord::Stop() {
  running_ = false;
}

size_t CustomWakeWord::GetFeedSize() {
  return feed_size_;
}

void CustomWakeWord::EncodeWakeWordData() {
}

bool CustomWakeWord::GetWakeWordOpus(std::vector<uint8_t>& opus) {
  (void)opus;
  return false;
}

const std::string& CustomWakeWord::GetLastDetectedWakeWord() const {
  return last_;
}

