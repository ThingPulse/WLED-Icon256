#pragma once

#include "wled.h"

#ifdef ESP32
  #include <HTTPUpdate.h>
  #include <HTTPClient.h>
  #include <WiFiClient.h>
#else
  #include <ESP8266httpUpdate.h>
  #include <ESP8266HTTPClient.h>
#endif

class UsermodRemoteOTA : public Usermod {
private:
  bool enabled = true;
  bool initDone = false;
  unsigned long lastCheck = 0;
  unsigned long checkInterval = 3600000; // 1 hour
  String versionCheckUrl = "";
  String firmwareUrl = "";
  bool updateAvailable = false;
  String latestVersion = "";
  String currentVersion = "";
  bool isUpdating = false;
  int updateProgress = 0;
  
  static const char _name[];
  static const char _enabled[];
  static const char _versionCheckUrl[];
  static const char _firmwareUrl[];
  static const char _checkInterval[];

public:
  void setup() {
    initDone = true;
    currentVersion = String(versionString);
    
    #ifdef REMOTE_OTA_VERSION_URL
      if (versionCheckUrl.isEmpty()) {
        versionCheckUrl = String(REMOTE_OTA_VERSION_URL);
      }
    #endif
    
    lastCheck = millis() - checkInterval + 30000;
  }

  void loop() {
    if (!enabled || !initDone || versionCheckUrl.isEmpty()) return;
    
    if (millis() - lastCheck >= checkInterval) {
      lastCheck = millis();
      checkForUpdate();
    }
  }

  void checkForUpdate() {
    if (versionCheckUrl.isEmpty()) return;

    DEBUG_PRINTLN(F("Remote OTA: Checking for updates..."));
    
    HTTPClient http;
    http.begin(versionCheckUrl);
    http.setTimeout(5000);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        const char* version = doc["version"];
        const char* url = doc["url"];
        
        if (version && url) {
          latestVersion = String(version);
          firmwareUrl = String(url);
          
          if (latestVersion != currentVersion) {
            updateAvailable = true;
            DEBUG_PRINTF("Remote OTA: Update available! Current: %s, Latest: %s\n", 
                       currentVersion.c_str(), latestVersion.c_str());
          } else {
            updateAvailable = false;
          }
        }
      }
    }
    
    http.end();
  }
  
  void performUpdate() {
    if (firmwareUrl.isEmpty() || !updateAvailable || isUpdating) return;

    isUpdating = true;
    updateProgress = 0;
    
    #ifdef ESP32
      WiFiClient client;
      HTTPUpdate httpUpdate;
      
      httpUpdate.onProgress([this](int current, int total) {
        this->updateProgress = (current * 100) / total;
      });
      
      t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);
      
      if (ret != HTTP_UPDATE_OK) isUpdating = false;
      
    #else
      WiFiClient client;
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
      
      ESPhttpUpdate.onProgress([this](int current, int total) {
        this->updateProgress = (current * 100) / total;
      });
      
      t_httpUpdate_return ret = ESPhttpUpdate.update(client, firmwareUrl);
      
      if (ret != HTTP_UPDATE_OK) isUpdating = false;
    #endif
  }

  void readFromJsonState(JsonObject& root) override {
    if (root["rmOTA_check"] | false) {
      checkForUpdate();
    }
    
    if (root["rmOTA_update"] | false) {
      performUpdate();
    }
  }

  void addToJsonInfo(JsonObject& root) override {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonArray infoArr = user.createNestedArray(F("Remote OTA"));
    
    if (isUpdating) {
      String progressStr = F("Updating: ");
      progressStr += updateProgress;
      progressStr += F("%");
      infoArr.add(progressStr);
    } else if (updateAvailable) {
      String updateStr = F("Update available: ");
      updateStr += latestVersion;
      infoArr.add(updateStr);
      infoArr.add(F(" 🔄 Update Now"));
    } else {
      infoArr.add(F("Up to date"));
      infoArr.add(currentVersion);
    }
  }

  void addToJsonState(JsonObject& root) override {
    root["rmOTA_available"] = updateAvailable;
    root["rmOTA_version"] = latestVersion;
    root["rmOTA_current"] = currentVersion;
    root["rmOTA_updating"] = isUpdating;
    root["rmOTA_progress"] = updateProgress;
  }

  void addToConfig(JsonObject& root) override {
    JsonObject top = root.createNestedObject(FPSTR(_name));
    top[FPSTR(_enabled)] = enabled;
    top[FPSTR(_versionCheckUrl)] = versionCheckUrl;
    top[FPSTR(_firmwareUrl)] = firmwareUrl;
    top[F("checkIntervalHours")] = checkInterval / 3600000;
  }

  bool readFromConfig(JsonObject& root) override {
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) return false;

    bool configComplete = true;
    
    configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
    configComplete &= getJsonValue(top[FPSTR(_versionCheckUrl)], versionCheckUrl);
    configComplete &= getJsonValue(top[FPSTR(_firmwareUrl)], firmwareUrl);
    
    int hours = checkInterval / 3600000;
    configComplete &= getJsonValue(top[F("checkIntervalHours")], hours);
    checkInterval = hours * 3600000;

    return configComplete;
  }

  uint16_t getId() override {
    return USERMOD_ID_REMOTE_OTA;
  }
};

const char UsermodRemoteOTA::_name[]           PROGMEM = "RemoteOTA";
const char UsermodRemoteOTA::_enabled[]        PROGMEM = "enabled";
const char UsermodRemoteOTA::_versionCheckUrl[] PROGMEM = "versionCheckUrl";
const char UsermodRemoteOTA::_firmwareUrl[]    PROGMEM = "firmwareUrl";
const char UsermodRemoteOTA::_checkInterval[]  PROGMEM = "checkIntervalHours";
