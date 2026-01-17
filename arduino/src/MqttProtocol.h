#pragma once
#include "Protocol.h"
#include <WiFiClient.h>
#include <PubSubClient.h>

class MqttProtocol : public ProtocolA {
 public:
  MqttProtocol(const char* host, uint16_t port, const char* client_id, const char* topic_up, const char* topic_down);
  bool Start() override;
  void Poll() override;
  bool OpenAudioChannel() override;
  void CloseAudioChannel() override;
  bool IsAudioChannelOpened() const override;
  bool SendAudio(std::unique_ptr<AudioStreamPacketA> packet) override;
  bool SendText(const String& text) override;
  void SendWakeWordDetected(const String& wake_word) override;
  void SendStartListening(ListeningModeA mode) override;
  void SendStopListening() override;
  void SendAbortSpeaking(AbortReasonA reason) override;

 private:
  WiFiClient wifi_;
  PubSubClient client_;
  String topic_up_;
  String topic_down_;
  String client_id_;
  bool audio_opened_;
  void HandleMessage(char* topic, uint8_t* payload, unsigned int length);
};
