# WLED Self-Test Usermod

This usermod provides comprehensive diagnostic testing for WLED devices, outputting results in JSON format compatible with factory testing tools.

## Features

- **JSON Test Results**: Outputs test results in structured JSON format with name/value/result fields
- **Comprehensive Tests**: Memory, WiFi, LED configuration, file system, and build info
- **Visual LED Test**: Separate command to cycle through colors (RED/GREEN/BLUE/WHITE)
- **Pass/Fail Indicators**: Each test includes OK/NOK result status

## Commands

### Self-Test (JSON Output)
Runs all diagnostic tests and outputs results as JSON array.

**Serial Monitor:**
```json
{"ST":true}
```

**HTTP API:**
```bash
curl -X POST http://[device-ip]/json/state -H "Content-Type: application/json" -d '{"ST":true}'
```

**Example Output:**
```json
[
  {"name":"WLED Version","value":"0.14.0","result":"OK"},
  {"name":"Mac Address","value":"AA:BB:CC:DD:EE:FF","result":"OK"},
  {"name":"Chip Model","value":"ESP32-D0WDQ6","result":"OK"},
  {"name":"Free Heap","value":"234kb","result":"OK"},
  {"name":"LED Count","value":"256","result":"OK"},
  {"name":"WiFi Status","value":"Connected","result":"OK"},
  {"name":"WiFi RSSI","value":"-45dBm","result":"OK"}
]
```

### Visual LED Test
Cycles through colors without JSON output.

**Serial Monitor:**
```json
{"VT":true}
```

## Test Categories

### Hardware Tests
- **Mac Address**: Device MAC address
- **Chip Model**: ESP32/ESP8266 model (ESP32 only)
- **Chip Revision**: Silicon revision (ESP32 only)
- **Available Cores**: CPU core count (ESP32 only, NOK if != 2)
- **CPU Frequency**: Clock speed in MHz (NOK if < 160MHz)

### Memory Tests
- **Free Heap**: Available RAM (NOK if < 50KB)
- **Heap Size**: Total RAM size (ESP32 only, NOK if < 100KB)
- **PSRAM Size**: External RAM size (ESP32 only, NOK if 0)
- **Free PSRAM**: Available external RAM (ESP32 only, NOK if 0)

### LED Configuration Tests
- **LED Count**: Total number of LEDs (NOK if 0)
- **Number of Busses**: LED bus count (NOK if 0)
- **Bus Details**: Type, length, and GPIO pins for each bus
- **Current Brightness**: Current brightness level (0-255)

### Network Tests
- **WiFi Status**: Connection state (NOK if not connected)
- **WiFi SSID**: Connected network name
- **WiFi IP**: Assigned IP address
- **WiFi RSSI**: Signal strength (NOK if < -80dBm)

### File System Tests
- **FS Total**: Total filesystem size (NOK if 0)
- **FS Used**: Used space in KB
- **FS Free**: Available space (NOK if < 10KB)

### Build Information
- **Build Date**: Compilation date
- **Build Time**: Compilation time
- **Uptime**: Seconds since boot

## Result Codes

- **OK**: Test passed
- **NOK**: Test failed or value out of expected range

## Installation

This usermod is automatically included in the icon256 build environment.

## Integration with Factory Tools

The JSON output format matches the factory testing sketch structure:
```cpp
{
  "name": "Test Name",
  "value": "Test Value",
  "result": "OK" | "NOK"
}
```

This allows easy integration with automated testing and quality control systems.
