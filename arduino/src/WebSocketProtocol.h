#pragma once
#include "Protocol.h"
#include <WebSocketsClient.h>

class WebSocketProtocol : public ProtocolA {
 public:
  explicit WebSocketProtocol(const char* url);
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
  WebSocketsClient ws_;
  bool audio_opened_;
  String host_;
  uint16_t port_{80};
  String path_{"/"};
  void OnEvent(WStype_t type, uint8_t* payload, size_t length);
};
