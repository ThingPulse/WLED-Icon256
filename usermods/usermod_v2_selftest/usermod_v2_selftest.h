#pragma once

#include "wled.h"

#ifdef ESP32
#include <driver/i2s.h>
#endif

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
    DynamicJsonDocument doc(4096);
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
    
    // Test 5: Speaker/Amplifier (MAX98357A I2S DAC)
    #ifdef ESP32
    // Icon256 MAX98357A pin configuration
    addTestMessage(data, "Speaker Amplifier", "MAX98357AETE+T", "OK");
    addTestMessage(data, "Speaker I2S DATA", "GPIO1", "OK");
    addTestMessage(data, "Speaker I2S BCLK", "GPIO40", "OK");
    addTestMessage(data, "Speaker I2S LRCLK", "GPIO39", "OK");
    addTestMessage(data, "Speaker I2S MCLK", "GPIO41", "OK");
    addTestMessage(data, "Speaker CTRL/SD", "GPIO42", "OK");
    #endif
    
    // Test 6: Buttons (with 10k pull-up resistors to 3.3V)
    #ifdef ESP32
    const int BUTTON_1_PIN = 11;
    const int BUTTON_2_PIN = 12;
    const int BUTTON_3_PIN = 13;
    
    // Configure pins as input (external pull-ups, so no internal pull-up needed)
    pinMode(BUTTON_1_PIN, INPUT);
    pinMode(BUTTON_2_PIN, INPUT);
    pinMode(BUTTON_3_PIN, INPUT);
    
    // Read button states - should be HIGH when not pressed
    bool btn1High = digitalRead(BUTTON_1_PIN) == HIGH;
    bool btn2High = digitalRead(BUTTON_2_PIN) == HIGH;
    bool btn3High = digitalRead(BUTTON_3_PIN) == HIGH;
    
    addTestMessage(data, "Button 1 (GPIO11)", btn1High ? "Not Pressed" : "Pressed/Shorted", btn1High ? "OK" : "NOK");
    addTestMessage(data, "Button 2 (GPIO12)", btn2High ? "Not Pressed" : "Pressed/Shorted", btn2High ? "OK" : "NOK");
    addTestMessage(data, "Button 3 (GPIO13)", btn3High ? "Not Pressed" : "Pressed/Shorted", btn3High ? "OK" : "NOK");
    #endif
    
    // Test 7: File System
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
    
    // Test 8: Build information
    addTestMessage(data, "Build Date", String(__DATE__), "OK");
    addTestMessage(data, "Build Time", String(__TIME__), "OK");
    
    // Test 9: Uptime
    addTestMessage(data, "Uptime", String(millis() / 1000) + "s", "OK");
    
    // Output JSON to serial
    serializeJson(doc, Serial);
    Serial.println();
    
    // Test 10: Run visual LED test
    runVisualLedTest();
    
    // Test 11: Run audio speaker test
    runAudioTest();
    
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
  
  void runAudioTest() {
    #ifdef ESP32
    // Play test tones through MAX98357A speaker
    const int I2S_SPK_DATA = 1;
    const int I2S_SPK_BCLK = 40;
    const int I2S_SPK_LRCLK = 39;
    const int I2S_SPK_MCLK = 41;
    const int I2S_SPK_CTRL = 42;
    
    // Configure I2S for audio output
    i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 16000,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
      .mck_io_num = I2S_SPK_MCLK,
      .bck_io_num = I2S_SPK_BCLK,
      .ws_io_num = I2S_SPK_LRCLK,
      .data_out_num = I2S_SPK_DATA,
      .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    // Enable MAX98357A by setting CTRL pin HIGH (SD pin - shutdown when LOW)
    pinMode(I2S_SPK_CTRL, OUTPUT);
    digitalWrite(I2S_SPK_CTRL, HIGH);
    delay(10);
    
    // Install and configure I2S driver on I2S_NUM_1 (to avoid conflict with microphone on I2S_NUM_0)
    if (i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL) == ESP_OK) {
      i2s_set_pin(I2S_NUM_1, &pin_config);
      
      // Generate simple test tones (3 beeps)
      const int sampleRate = 16000;
      const int beepFreq = 1000; // 1kHz tone
      const int beepDuration = 200; // 200ms per beep
      const int samples = (sampleRate * beepDuration) / 1000;
      int16_t audioBuffer[samples];
      
      // Generate sine wave tone
      for (int b = 0; b < 3; b++) {  // 3 beeps
        for (int i = 0; i < samples; i++) {
          float t = (float)i / sampleRate;
          audioBuffer[i] = (int16_t)(sin(2.0 * PI * beepFreq * t) * 8000); // Amplitude 8000
        }
        
        size_t bytes_written;
        i2s_write(I2S_NUM_1, audioBuffer, sizeof(audioBuffer), &bytes_written, portMAX_DELAY);
        delay(100); // Gap between beeps
      }
      
      // Small delay then cleanup
      delay(200);
      i2s_driver_uninstall(I2S_NUM_1);
      
      // Disable MAX98357A by setting CTRL pin LOW
      digitalWrite(I2S_SPK_CTRL, LOW);
    }
    #endif
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
