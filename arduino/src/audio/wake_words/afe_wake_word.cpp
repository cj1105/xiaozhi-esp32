#include "audio/wake_words/afe_wake_word.h"

bool AfeWakeWord::Initialize(AudioCodec* codec, srmodel_list_t* models_list) {
  (void)codec;
  (void)models_list;
  return true;
}

void AfeWakeWord::Feed(const std::vector<int16_t>& data) {
  (void)data;
}

void AfeWakeWord::OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) {
  cb_ = std::move(callback);
}

void AfeWakeWord::Start() {
  running_ = true;
}

void AfeWakeWord::Stop() {
  running_ = false;
}

size_t AfeWakeWord::GetFeedSize() {
  return feed_size_;
}

void AfeWakeWord::EncodeWakeWordData() {
}

bool AfeWakeWord::GetWakeWordOpus(std::vector<uint8_t>& opus) {
  (void)opus;
  return false;
}

const std::string& AfeWakeWord::GetLastDetectedWakeWord() const {
  return last_;
}

