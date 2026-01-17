#include "WebSocketProtocol.h"
#include <Arduino.h>

static void ParseWsUrl(const char* url, String& host, uint16_t& port, String& path) {
  String u = url;
  bool secure = false;
  if (u.startsWith("wss://")) { secure = true; u = u.substring(6); }
  else if (u.startsWith("ws://")) { u = u.substring(5); }
  int slash = u.indexOf('/');
  String hostport = slash >= 0 ? u.substring(0, slash) : u;
  path = slash >= 0 ? u.substring(slash) : "/";
  int colon = hostport.indexOf(':');
  if (colon >= 0) {
    host = hostport.substring(0, colon);
    port = hostport.substring(colon + 1).toInt();
  } else {
    host = hostport;
    port = secure ? 443 : 80;
  }
}

WebSocketProtocol::WebSocketProtocol(const char* url) : audio_opened_(false) {
  ParseWsUrl(url, host_, port_, path_);
  ws_.begin(host_.c_str(), port_, path_.c_str());
  ws_.onEvent([this](WStype_t t, uint8_t* p, size_t l) { OnEvent(t, p, l); });
}

bool WebSocketProtocol::Start() {
  return true;
}

bool WebSocketProtocol::OpenAudioChannel() {
  audio_opened_ = true;
  return true;
}

void WebSocketProtocol::CloseAudioChannel() {
  audio_opened_ = false;
}

bool WebSocketProtocol::IsAudioChannelOpened() const {
  return audio_opened_;
}

bool WebSocketProtocol::SendAudio(std::unique_ptr<AudioStreamPacketA> packet) {
  if (!audio_opened_) return false;
  ws_.sendBIN(packet->payload.data(), packet->payload.size());
  return true;
}

bool WebSocketProtocol::SendText(const String& text) {
  String tmp = text;
  ws_.sendTXT(tmp);
  return true;
}

void WebSocketProtocol::SendWakeWordDetected(const String& wake_word) {
  String tmp = wake_word;
  ws_.sendTXT(tmp);
}

void WebSocketProtocol::SendStartListening(ListeningModeA mode) {
  ws_.sendTXT("start_listening");
}

void WebSocketProtocol::SendStopListening() {
  ws_.sendTXT("stop_listening");
}

void WebSocketProtocol::SendAbortSpeaking(AbortReasonA reason) {
  ws_.sendTXT("abort_speaking");
}

void WebSocketProtocol::Poll() {
  ws_.loop();
}

void WebSocketProtocol::OnEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_CONNECTED) {
    if (on_connected_) on_connected_();
  } else if (type == WStype_DISCONNECTED) {
    if (on_disconnected_) on_disconnected_();
  } else if (type == WStype_TEXT) {
    String msg;
    msg.reserve(length);
    for (size_t i = 0; i < length; ++i) msg += (char)payload[i];
    if (on_in_json_) on_in_json_(msg);
  } else if (type == WStype_BIN) {
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
