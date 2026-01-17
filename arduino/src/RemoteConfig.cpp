#include "RemoteConfig.h"
#include <HTTPClient.h>

static String jget(const String& json, const String& key) {
  int p = json.indexOf("\"" + key + "\"");
  if (p < 0) return "";
  int c = json.indexOf(':', p);
  if (c < 0) return "";
  int s = json.indexOf('"', c + 1);
  if (s < 0) return "";
  int e = json.indexOf('"', s + 1);
  if (e < 0) return "";
  return json.substring(s + 1, e);
}

bool RemoteConfig::Fetch(const String& url, Settings* settings) {
  if (!settings) return false;
  HTTPClient http;
  if (!http.begin(url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  String body = http.getString();
  http.end();
  String mqtt_host = jget(body, "mqtt_host");
  String mqtt_port = jget(body, "mqtt_port");
  String mqtt_client_id = jget(body, "mqtt_client_id");
  String mqtt_up = jget(body, "mqtt_up");
  String mqtt_down = jget(body, "mqtt_down");
  String ws_url = jget(body, "ws_url");
  String audio_rate = jget(body, "audio_rate");
  String audio_frame = jget(body, "audio_frame");
  String i2s_bclk = jget(body, "i2s_bclk");
  String i2s_lrclk = jget(body, "i2s_lrclk");
  String i2s_din = jget(body, "i2s_din");
  if (mqtt_host.length()) settings->SetString("mqtt_host", mqtt_host);
  if (mqtt_port.length()) settings->SetInt("mqtt_port", mqtt_port.toInt());
  if (mqtt_client_id.length()) settings->SetString("mqtt_client_id", mqtt_client_id);
  if (mqtt_up.length()) settings->SetString("mqtt_up", mqtt_up);
  if (mqtt_down.length()) settings->SetString("mqtt_down", mqtt_down);
  if (ws_url.length()) settings->SetString("ws_url", ws_url);
  if (audio_rate.length()) settings->SetInt("audio_rate", audio_rate.toInt());
  if (audio_frame.length()) settings->SetInt("audio_frame", audio_frame.toInt());
  if (i2s_bclk.length()) settings->SetInt("i2s_bclk", i2s_bclk.toInt());
  if (i2s_lrclk.length()) settings->SetInt("i2s_lrclk", i2s_lrclk.toInt());
  if (i2s_din.length()) settings->SetInt("i2s_din", i2s_din.toInt());
  settings->SetInt("cfg_loaded", 1);
  return true;
}

