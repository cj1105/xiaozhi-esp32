#include "Application.h"

Application::Application() : initialized_(false), last_tick_ms_(0) {}

void Application::Initialize() {
  Serial.println("[APP] Initialize begin");
  if (initialized_) return;
  board_ = std::make_unique<Board>();
  display_ = std::make_unique<Display>();
  wifi_ = std::make_unique<WiFiService>();
  ota_ = std::make_unique<OTA>();
  settings_ = std::make_unique<Settings>();
  settings_->Begin("xiaozhi");
  assets_ = std::make_unique<Assets>();
  audio_ = std::make_unique<AudioService>();
  board_->Initialize();
  state_machine_.TransitionTo(kDeviceStateStartingA);
  display_->Initialize(board_.get(), &state_machine_);
  {
    String ssid = settings_->GetString("wifi_ssid", Config::WIFI_SSID);
    String pwd = settings_->GetString("wifi_pwd", Config::WIFI_PASSWORD);
    if (ssid.length() == 0) {
      state_machine_.TransitionTo(kDeviceStateWifiConfiguringA);
      StartProvisioningAP();
    } else {
      wifi_->Begin(ssid.c_str(), pwd.c_str());
      state_machine_.TransitionTo(kDeviceStateConnectingA);
    }
  }
  state_machine_.TransitionTo(kDeviceStateConnectingA);
  assets_->Begin();
  audio_->Begin();
  Serial.println("[APP] Initialize end");
  initialized_ = true;
  last_tick_ms_ = millis();
  last_hb_ms_ = millis();
  last_ota_ms_ = millis();
}

void Application::Tick() {
  if (!initialized_) return;
  board_->Poll();
  display_->Poll();
  wifi_->Poll();
  if (web_) web_->handleClient();
  bool now_connected = wifi_->IsConnected();
  if (now_connected != wifi_connected_) {
    wifi_connected_ = now_connected;
    if (wifi_connected_) {
      board_->SetLed(true);
      state_machine_.TransitionTo(kDeviceStateIdleA);
      if (!cfg_loaded_) {
        if (Config::CONFIG_URL && strlen(Config::CONFIG_URL) > 0) {
          remote_cfg_.Fetch(Config::CONFIG_URL, settings_.get());
        }
        cfg_loaded_ = true;
      }
      if (!protocol_) {
        String mh = settings_->GetString("mqtt_host", Config::MQTT_HOST);
        int mp = settings_->GetInt("mqtt_port", Config::MQTT_PORT);
        String mc = settings_->GetString("mqtt_client_id", Config::MQTT_CLIENT_ID);
        String mu = settings_->GetString("mqtt_up", Config::MQTT_TOPIC_UP);
        String md = settings_->GetString("mqtt_down", Config::MQTT_TOPIC_DOWN);
        String ws = settings_->GetString("ws_url", Config::WS_URL);
        if (mh.length() > 0) {
          protocol_.reset(new MqttProtocol(mh.c_str(), (uint16_t)mp, mc.c_str(), mu.c_str(), md.c_str()));
        } else if (ws.length() > 0) {
          protocol_.reset(new WebSocketProtocol(ws.c_str()));
        }
        if (protocol_) {
          protocol_->OnConnected([this]() { state_machine_.TransitionTo(kDeviceStateIdleA); });
          protocol_->OnDisconnected([this]() { state_machine_.TransitionTo(kDeviceStateConnectingA); });
          protocol_->OnIncomingJson([this](const String& s){ HandleJsonMessage(s); });
        }
      }
      if (protocol_ && !protocol_started_) {
        protocol_started_ = protocol_->Start();
      }
      StopProvisioningAP();
    } else {
      board_->SetLed(false);
      state_machine_.TransitionTo(kDeviceStateConnectingA);
      protocol_started_ = false;
    }
  }
  if (protocol_) {
    protocol_->Poll();
    if (protocol_->IsAudioChannelOpened() && audio_ && audio_->IsOpened()) {
      int sample_rate = 16000;
      int frame_ms = 20;
      auto payload = audio_->CaptureFrame(sample_rate, frame_ms);
      if (payload) {
        auto pkt = std::make_unique<AudioStreamPacketA>();
        pkt->sample_rate = sample_rate;
        pkt->frame_duration = frame_ms;
        pkt->timestamp = millis();
        pkt->payload = std::move(*payload);
        protocol_->SendAudio(std::move(pkt));
      }
    }
  }
  unsigned long now = millis();
  if (now - last_tick_ms_ >= 100) {
    last_tick_ms_ = now;
  }
  if (wifi_connected_ && protocol_ && protocol_started_) {
    if (now - last_hb_ms_ >= 5000) {
      last_hb_ms_ = now;
      String hb = String("{\"cmd\":\"heartbeat\",\"ts\":") + String(now) + String(",\"ver\":\"") + String(settings_->GetString("fw_version", Config::FW_VERSION)) + String("\"}");
      protocol_->SendText(hb);
    }
    if (Config::OTA_CHECK_URL && strlen(Config::OTA_CHECK_URL) > 0) {
      if (now - last_ota_ms_ >= 600000) {
        last_ota_ms_ = now;
        CheckOta();
      }
    }
  }
}

bool Application::IsInitialized() const {
  return initialized_;
}

void Application::HandleJsonMessage(const String& msg) {
  String cmd = ExtractJsonValue(msg, "cmd");
  if (cmd.length() == 0) cmd = msg;
  if (cmd == "start_listening") {
    state_machine_.TransitionTo(kDeviceStateListeningA);
    if (protocol_) {
      protocol_->OpenAudioChannel();
      audio_->Open();
    }
  } else if (cmd == "stop_listening") {
    state_machine_.TransitionTo(kDeviceStateIdleA);
    if (protocol_) {
      protocol_->CloseAudioChannel();
      audio_->Close();
    }
  } else if (cmd == "speak") {
    state_machine_.TransitionTo(kDeviceStateSpeakingA);
  } else if (cmd == "upgrade") {
    String url = ExtractJsonValue(msg, "url");
    state_machine_.TransitionTo(kDeviceStateUpgradingA);
    bool ok = ota_->UpgradeFromUrl(url, "");
    state_machine_.TransitionTo(ok ? kDeviceStateIdleA : kDeviceStateFatalErrorA);
  }
}

void Application::StartProvisioningAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("xiaozhi-setup", "xiaozhi123");
  web_.reset(new WebServer(80));
  web_->on("/", [this]() {
    String html = "<html><body><form action='/config' method='get'>"
                  "SSID:<input name='ssid'><br/>"
                  "PWD:<input name='pwd' type='password'><br/>"
                  "<input type='submit' value='Save'></form></body></html>";
    web_->send(200, "text/html", html);
  });
  web_->on("/config", [this]() {
    String ssid = web_->arg("ssid");
    String pwd = web_->arg("pwd");
    if (ssid.length() > 0) {
      settings_->SetString("wifi_ssid", ssid);
      settings_->SetString("wifi_pwd", pwd);
      web_->send(200, "text/plain", "OK, rebooting to STA");
      StopProvisioningAP();
      WiFi.mode(WIFI_STA);
      wifi_->Begin(ssid.c_str(), pwd.c_str());
      state_machine_.TransitionTo(kDeviceStateConnectingA);
    } else {
      web_->send(400, "text/plain", "Missing ssid");
    }
  });
  web_->begin();
}

void Application::StopProvisioningAP() {
  if (web_) {
    web_->stop();
    web_.reset();
  }
}

String Application::ExtractJsonValue(const String& json, const String& key) {
  int pos = json.indexOf("\"" + key + "\"");
  if (pos < 0) return "";
  int colon = json.indexOf(':', pos);
  if (colon < 0) return "";
  int start = json.indexOf('"', colon + 1);
  if (start < 0) return "";
  int end = json.indexOf('"', start + 1);
  if (end < 0) return "";
  return json.substring(start + 1, end);
}

void Application::CheckOta() {
  String cur = settings_->GetString("fw_version", Config::FW_VERSION);
  String new_ver;
  String new_url;
  bool ok = ota_->CheckAndUpgrade(Config::OTA_CHECK_URL, cur, new_ver, new_url);
  if (ok && new_ver.length() > 0 && new_ver != cur) {
    state_machine_.TransitionTo(kDeviceStateUpgradingA);
    settings_->SetString("fw_version", new_ver);
    if (protocol_) {
      String msg = String("{\"cmd\":\"upgrade_done\",\"version\":\"") + new_ver + String("\"}");
      protocol_->SendText(msg);
    }
    state_machine_.TransitionTo(kDeviceStateIdleA);
  }
}

