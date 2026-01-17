#include "MqttProtocol.h"

MqttProtocol::MqttProtocol(const char* host, uint16_t port, const char* client_id, const char* topic_up, const char* topic_down)
    : client_(wifi_), topic_up_(topic_up), topic_down_(topic_down), client_id_(client_id), audio_opened_(false) {
  client_.setServer(host, port);
  client_.setCallback([this](char* topic, uint8_t* payload, unsigned int length) { HandleMessage(topic, payload, length); });
}

bool MqttProtocol::Start() {
  if (client_.connected()) return true;
  if (!client_.connect(client_id_.c_str())) return false;
  client_.subscribe(topic_down_.c_str());
  if (on_connected_) on_connected_();
  return true;
}

bool MqttProtocol::OpenAudioChannel() {
  audio_opened_ = true;
  return true;
}

void MqttProtocol::CloseAudioChannel() {
  audio_opened_ = false;
}

bool MqttProtocol::IsAudioChannelOpened() const {
  return audio_opened_;
}

bool MqttProtocol::SendAudio(std::unique_ptr<AudioStreamPacketA> packet) {
  if (!audio_opened_) return false;
  return client_.publish(topic_up_.c_str(), packet->payload.data(), packet->payload.size());
}

bool MqttProtocol::SendText(const String& text) {
  return client_.publish(topic_up_.c_str(), text.c_str());
}

void MqttProtocol::SendWakeWordDetected(const String& wake_word) {
  client_.publish(topic_up_.c_str(), wake_word.c_str());
}

void MqttProtocol::SendStartListening(ListeningModeA mode) {
  client_.publish(topic_up_.c_str(), "start_listening");
}

void MqttProtocol::SendStopListening() {
  client_.publish(topic_up_.c_str(), "stop_listening");
}

void MqttProtocol::SendAbortSpeaking(AbortReasonA reason) {
  client_.publish(topic_up_.c_str(), "abort_speaking");
}

void MqttProtocol::Poll() {
  if (!client_.connected()) {
    if (client_.connect(client_id_.c_str())) {
      client_.subscribe(topic_down_.c_str());
      if (on_connected_) on_connected_();
    }
  }
  client_.loop();
}

void MqttProtocol::HandleMessage(char* topic, uint8_t* payload, unsigned int length) {
  if (length > 0 && payload[0] == '{') {
    String msg;
    msg.reserve(length);
    for (unsigned int i = 0; i < length; ++i) msg += (char)payload[i];
    if (on_in_json_) on_in_json_(msg);
  } else {
    if (on_in_audio_) {
      auto pkt = std::make_unique<AudioStreamPacketA>();
      pkt->sample_rate = 16000;
      pkt->frame_duration = 20;
      pkt->timestamp = millis();
      pkt->payload.assign(payload, payload + length);
      on_in_audio_(std::move(pkt));
    }
  }
}
