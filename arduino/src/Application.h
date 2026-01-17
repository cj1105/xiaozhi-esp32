#pragma once
#include <Arduino.h>
#include <memory>
#include <string>
#include "Board.h"
#include "Display.h"
#include "WiFiService.h"
#include "Protocol.h"
#include "MqttProtocol.h"
#include "WebSocketProtocol.h"
#include "DeviceStateMachine.h"
#include "OTA.h"
#include "Config.h"
#include "Settings.h"
#include "Assets.h"
#include <WebServer.h>
#include "AudioService.h"
#include "RemoteConfig.h"

class Application {
 public:
  Application();
  void Initialize();
  void Tick();
  bool IsInitialized() const;

 private:
  bool initialized_;
  std::unique_ptr<Board> board_;
  std::unique_ptr<Display> display_;
  std::unique_ptr<WiFiService> wifi_;
  std::unique_ptr<ProtocolA> protocol_;
  std::unique_ptr<OTA> ota_;
  std::unique_ptr<Settings> settings_;
  std::unique_ptr<Assets> assets_;
  DeviceStateMachineA state_machine_;
  unsigned long last_tick_ms_;
  bool wifi_connected_{false};
  bool protocol_started_{false};
  unsigned long last_hb_ms_{0};
  unsigned long last_ota_ms_{0};
  void CheckOta();
  unsigned long last_cfg_ms_{0};
  bool cfg_loaded_{false};
  RemoteConfig remote_cfg_;
  void HandleJsonMessage(const String& msg);
  std::unique_ptr<WebServer> web_;
  void StartProvisioningAP();
  void StopProvisioningAP();
  String ExtractJsonValue(const String& json, const String& key);
  std::unique_ptr<AudioService> audio_;
};

