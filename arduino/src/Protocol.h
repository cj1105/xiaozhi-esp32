#pragma once
#include <Arduino.h>
#include <functional>
#include <string>
#include <vector>
#include <memory>

struct AudioStreamPacketA {
  int sample_rate = 0;
  int frame_duration = 0;
  uint32_t timestamp = 0;
  std::vector<uint8_t> payload;
};

enum ListeningModeA {
  kListeningModeAutoStopA,
  kListeningModeManualStopA,
  kListeningModeRealtimeA
};

enum AbortReasonA {
  kAbortReasonNoneA,
  kAbortReasonWakeWordDetectedA
};

class ProtocolA {
 public:
  virtual ~ProtocolA() {}
  void OnIncomingAudio(std::function<void(std::unique_ptr<AudioStreamPacketA>)> cb) { on_in_audio_ = cb; }
  void OnIncomingJson(std::function<void(const String&)> cb) { on_in_json_ = cb; }
  void OnConnected(std::function<void()> cb) { on_connected_ = cb; }
  void OnDisconnected(std::function<void()> cb) { on_disconnected_ = cb; }
  virtual bool Start() = 0;
  virtual void Poll() = 0;
  virtual bool OpenAudioChannel() = 0;
  virtual void CloseAudioChannel() = 0;
  virtual bool IsAudioChannelOpened() const = 0;
  virtual bool SendAudio(std::unique_ptr<AudioStreamPacketA> packet) = 0;
  virtual bool SendText(const String& text) = 0;
  virtual void SendWakeWordDetected(const String& wake_word) = 0;
  virtual void SendStartListening(ListeningModeA mode) = 0;
  virtual void SendStopListening() = 0;
  virtual void SendAbortSpeaking(AbortReasonA reason) = 0;

 protected:
  std::function<void(std::unique_ptr<AudioStreamPacketA>)> on_in_audio_;
  std::function<void(const String&)> on_in_json_;
  std::function<void()> on_connected_;
  std::function<void()> on_disconnected_;
};
