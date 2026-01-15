#pragma once

#include "wled.h"

/*
 * Self-Test Usermod
 * 
 * Responds to ST command with diagnostic information in JSON format.
 * Tests memory, WiFi, LED configuration, and performs a visual LED test.
 */

class UsermodSelfTest : public Usermod {
private:
  bool enabled = true;
  unsigned long lastTest = 0;
  
  void addTestMessage(JsonArray& data, const char* name, String value, const char* result) {
    JsonObject object = data.createNestedObject();
    object["name"] = name;
    object["value"] = value;
    object["result"] = result;
  }
  
  void runSelfTest() {
    DynamicJsonDocument doc(2048);
    JsonArray data = doc.to<JsonArray>();
    
    // Test 1: Basic chip information
    addTestMessage(data, "WLED Version", String(versionString), "OK");
    addTestMessage(data, "Mac Address", WiFi.macAddress(), "OK");
    
    #ifdef ESP32
    addTestMessage(data, "Chip Model", String(ESP.getChipModel()), "OK");
    addTestMessage(data, "Chip Revision", String(ESP.getChipRevision()), "OK");
    addTestMessage(data, "Available Cores", String(ESP.getChipCores()), 
                   ESP.getChipCores() == 2 ? "OK" : "NOK");
    addTestMessage(data, "CPU Frequency", String(ESP.getCpuFreqMHz()) + "MHz", 
                   ESP.getCpuFreqMHz() >= 160 ? "OK" : "NOK");
    #endif
    
    // Test 2: Memory
    uint32_t freeHeap = ESP.getFreeHeap();
    addTestMessage(data, "Free Heap", String(freeHeap / 1024) + "kb", 
                   freeHeap > 50000 ? "OK" : "NOK");
    
    #ifdef ESP32
    uint32_t heapSize = ESP.getHeapSize();
    addTestMessage(data, "Heap Size", String(heapSize / 1024) + "kb", 
                   heapSize > 100000 ? "OK" : "NOK");
    
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t freePsram = ESP.getFreePsram();
    addTestMessage(data, "PSRAM Size", String(psramSize / 1024) + "kb", 
                   psramSize > 0 ? "OK" : "NOK");
    addTestMessage(data, "Free PSRAM", String(freePsram / 1024) + "kb", 
                   psramSize > 0 && freePsram > 0 ? "OK" : "NOK");
    #endif
    
    #ifdef ESP8266
    uint32_t freeSketch = ESP.getFreeSketchSpace();
    addTestMessage(data, "Free Sketch Space", String(freeSketch / 1024) + "kb", 
                   freeSketch > 10000 ? "OK" : "NOK");
    #endif
    
    // Test 3: LED Strip Configuration
    uint16_t ledCount = strip.getLengthTotal();
    addTestMessage(data, "LED Count", String(ledCount), 
                   ledCount > 0 ? "OK" : "NOK");
    
    byte numBusses = BusManager::getNumBusses();
    addTestMessage(data, "Number of Busses", String(numBusses), 
                   numBusses > 0 ? "OK" : "NOK");
    
    // Get detailed bus information
    for (byte b = 0; b < numBusses; b++) {
      Bus* curBus = BusManager::getBus(b);
      if (curBus != nullptr && curBus->isOk()) {
        uint8_t pins[OUTPUT_MAX_PINS];
        byte numPins = curBus->getPins(pins);
        String pinStr = "";
        for (byte p = 0; p < numPins; p++) {
          if (p > 0) pinStr += ",";
          pinStr += String(pins[p]);
        }
        
        String busInfo = "Type=" + String(curBus->getType()) + 
                        ", Len=" + String(curBus->getLength()) + 
                        ", Pins=" + pinStr;
        addTestMessage(data, ("Bus " + String(b)).c_str(), busInfo, 
                      curBus->isOk() ? "OK" : "NOK");
      }
    }
    
    addTestMessage(data, "Current Brightness", String(bri), "OK");
    
    // Test 4: Microphone (I2S)
    #if defined(I2S_SDPIN) && (I2S_SDPIN >= 0)
    // Microphone is configured via build flags
    addTestMessage(data, "Microphone Type", "ICS-43434 I2S", "OK");
    addTestMessage(data, "I2S SD Pin", String(I2S_SDPIN), "OK");
    #ifdef I2S_WSPIN
    addTestMessage(data, "I2S WS Pin", String(I2S_WSPIN), "OK");
    #endif
    #ifdef I2S_CKPIN
    addTestMessage(data, "I2S CK Pin", String(I2S_CKPIN), "OK");
    #endif
    #ifdef DMTYPE
    String dmType = "";
    switch(DMTYPE) {
      case 0: dmType = "Analog/Disabled"; break;
      case 1: dmType = "I2S Generic"; break;
      case 2: dmType = "ES7243"; break;
      case 3: dmType = "SPH0645"; break;
      case 4: dmType = "I2S+MCLK"; break;
      case 5: dmType = "PDM"; break;
      default: dmType = "Unknown";
    }
    addTestMessage(data, "Audio Type", dmType, "OK");
    #endif
    #else
    addTestMessage(data, "Microphone", "Not configured", "NOK");
    #endif
    
    // Test 5: File System
    #ifdef ESP32
    uint32_t totalBytes = WLED_FS.totalBytes();
    uint32_t usedBytes = WLED_FS.usedBytes();
    addTestMessage(data, "FS Total", String(totalBytes / 1024) + "kb", 
                   totalBytes > 0 ? "OK" : "NOK");
    addTestMessage(data, "FS Used", String(usedBytes / 1024) + "kb", "OK");
    addTestMessage(data, "FS Free", String((totalBytes - usedBytes) / 1024) + "kb", 
                   (totalBytes - usedBytes) > 10000 ? "OK" : "NOK");
    #endif
    
    #ifdef ESP8266
    FSInfo fs_info;
    WLED_FS.info(fs_info);
    addTestMessage(data, "FS Total", String(fs_info.totalBytes / 1024) + "kb", 
                   fs_info.totalBytes > 0 ? "OK" : "NOK");
    addTestMessage(data, "FS Used", String(fs_info.usedBytes / 1024) + "kb", "OK");
    #endif
    
    // Test 6: Build information
    addTestMessage(data, "Build Date", String(__DATE__), "OK");
    addTestMessage(data, "Build Time", String(__TIME__), "OK");
    
    // Test 7: Uptime
    addTestMessage(data, "Uptime", String(millis() / 1000) + "s", "OK");
    
    // Output JSON to serial
    serializeJson(doc, Serial);
    Serial.println();
    
    // Test 8: Run visual LED test
    runVisualLedTest();
    
    lastTest = millis();
  }
  
  void runVisualLedTest() {
    // Get main segment and save current state
    uint8_t mainSeg = strip.getMainSegmentId();
    Segment& seg = strip.getSegment(mainSeg);
    uint8_t oldMode = seg.mode;
    uint32_t oldCol0 = seg.colors[0];
    uint32_t oldCol1 = seg.colors[1];
    uint32_t oldCol2 = seg.colors[2];
    uint8_t oldBri = bri;
    
    // Switch to Solid effect (FX_MODE_STATIC = 0)
    seg.setMode(FX_MODE_STATIC);
    bri = 128;
    briT = 128;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
    
    // Test RED
    seg.colors[0] = RED;
    seg.colors[1] = BLACK;
    seg.colors[2] = BLACK;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
    for (int i = 0; i < 20; i++) { strip.service(); delay(50); }
    
    // Test GREEN
    seg.colors[0] = GREEN;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
    for (int i = 0; i < 20; i++) { strip.service(); delay(50); }
    
    // Test BLUE
    seg.colors[0] = BLUE;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
    for (int i = 0; i < 20; i++) { strip.service(); delay(50); }
    
    // Test WHITE
    seg.colors[0] = WHITE;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
    for (int i = 0; i < 20; i++) { strip.service(); delay(50); }
    
    // Restore original state
    seg.setMode(oldMode);
    seg.colors[0] = oldCol0;
    seg.colors[1] = oldCol1;
    seg.colors[2] = oldCol2;
    bri = oldBri;
    briT = oldBri;
    stateUpdated(CALL_MODE_DIRECT_CHANGE);
  }

public:
  void setup() {
    Serial.println(F("\nSelf-Test Usermod initialized."));
    Serial.println(F("Send '{\"ST\":true}' to run self-test with visual LED test"));
  }

  void loop() {
    // Don't read serial directly - it conflicts with WLED's JSON API
  }
  
  /*
   * addToJsonInfo() - add custom info to /json/info
   */
  void addToJsonInfo(JsonObject& root) {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");
    
    JsonArray selftest = user.createNestedArray("Self-Test");
    selftest.add(F("{\"ST\":true}"));
  }
  
  /*
   * readFromJsonState() - process commands from JSON API
   */
  void readFromJsonState(JsonObject& root) {
    // Check if "ST" command was sent via JSON (self-test with visual)
    if (root["ST"].is<bool>() && root["ST"].as<bool>()) {
      runSelfTest();
    }
  }

  uint16_t getId() {
    return USERMOD_ID_SELFTEST;
  }
};
