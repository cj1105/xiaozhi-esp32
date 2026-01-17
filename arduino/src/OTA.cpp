#include "OTA.h"
#include <HTTPClient.h>
#include <Update.h>

bool OTA::UpgradeFromUrl(const String& url, const String& version) {
  HTTPClient http;
  if (!http.begin(url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  int len = http.getSize();
  WiFiClient* stream = http.getStreamPtr();
  if (!Update.begin(len)) { http.end(); return false; }
  size_t written = 0;
  uint8_t buff[1024];
  while (http.connected() && (len > 0 || len == -1)) {
    size_t avail = stream->available();
    if (avail) {
      int read = stream->readBytes(buff, ((avail > sizeof(buff)) ? sizeof(buff) : avail));
      if (read <= 0) break;
      Update.write(buff, read);
      written += read;
      if (len > 0) len -= read;
    } else {
      delay(1);
    }
  }
  bool ok = Update.end();
  http.end();
  return ok;
}

static String json_get(const String& json, const String& key) {
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

bool OTA::CheckAndUpgrade(const String& check_url, const String& current_version, String& new_version, String& new_url) {
  HTTPClient http;
  if (!http.begin(check_url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  String body = http.getString();
  http.end();
  new_version = json_get(body, "version");
  new_url = json_get(body, "url");
  if (new_version.length() == 0 || new_url.length() == 0) return false;
  if (new_version == current_version) return true;
  bool ok = UpgradeFromUrl(new_url, new_version);
  return ok;
}
