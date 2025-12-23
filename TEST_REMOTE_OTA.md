# Quick Test Guide for Remote OTA Usermod

## Option 1: Test with Local Web Server (Recommended)

### Step 1: Build the firmware
```bash
pio run -e icon256
```
The firmware binary will be at: `.pio/build/icon256/firmware.bin`

### Step 2: Start a local HTTP server
```bash
# In the project root directory
cd .pio/build/icon256
python3 -m http.server 8000
```

### Step 3: Create a test version.json file
In another terminal, create a test version file in the same directory:
```bash
cd .pio/build/icon256
cat > version.json << 'EOF'
{
  "version": "0.16.0-test",
  "url": "http://YOUR_COMPUTER_IP:8000/firmware.bin"
}
EOF
```
Replace `YOUR_COMPUTER_IP` with your computer's local IP (e.g., 192.168.1.100)

### Step 4: Configure WLED to use the test server
1. Open WLED web interface
2. Go to **Settings** > **Usermods** > **RemoteOTA**
3. Set **Version Check URL** to: `http://YOUR_COMPUTER_IP:8000/version.json`
4. Set **Check Interval Hours** to: `0.01` (36 seconds for quick testing)
5. Save

### Step 5: Test the update check
Watch the serial monitor:
```bash
pio device monitor -e icon256
```

You should see:
```
Remote OTA: Checking for updates...
Remote OTA: Update available! Current: 0.16.0, Latest: 0.16.0-test
```

### Step 6: Test the update notification
1. Go to the WLED info page
2. You should see: **"Remote OTA: Update available: 0.16.0-test"** with a **"🔄 Update Now"** button

### Step 7: Verify the update mechanism (without actually updating)
Use curl to trigger an update check:
```bash
curl -X POST http://YOUR_WLED_IP/json/state -d '{"rmOTA_check":true}'
```

Check the status:
```bash
curl http://YOUR_WLED_IP/json/state | grep rmOTA
```

You should see:
```json
"rmOTA_available": true,
"rmOTA_version": "0.16.0-test",
"rmOTA_current": "0.16.0",
"rmOTA_updating": false,
"rmOTA_progress": 0
```

---

## Option 2: Test with GitHub (For Real Scenario)

### Step 1: Create a test branch
```bash
git checkout -b test-remote-ota
```

### Step 2: Update version in version.json on GitHub
1. Edit `version.json` in your repository
2. Change version to something different (e.g., "0.16.0-beta")
3. Commit and push

### Step 3: Configure WLED to use GitHub URL
Use the URL: `https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/test-remote-ota/version.json`

---

## Option 3: Quick Manual Test via JSON API

You can manually test the JSON API integration:

```bash
# Force an immediate check
curl -X POST http://YOUR_WLED_IP/json/state -d '{"rmOTA_check":true}'

# Check the status
curl http://YOUR_WLED_IP/json/state

# Trigger update (only if you want to actually update!)
# curl -X POST http://YOUR_WLED_IP/json/state -d '{"rmOTA_update":true}'
```

---

## What to Look For

✅ **Success indicators:**
- Serial output shows "Checking for updates..."
- Info page shows update notification when version differs
- JSON API returns correct `rmOTA_*` fields
- HTTP server shows GET requests for version.json

❌ **Common issues:**
- URL not accessible from device (check firewall/network)
- JSON format incorrect (validate with jsonlint.com)
- Version URL not set in config
- Usermod not enabled

---

## Quick Verification Checklist

- [ ] Serial output shows "Remote OTA: Checking for updates..."
- [ ] Info page displays RemoteOTA section
- [ ] JSON API includes `rmOTA_available` field
- [ ] Can trigger check via `{"rmOTA_check":true}`
- [ ] Update notification appears when versions differ
- [ ] HTTP server receives GET requests for version.json

---

## Reset Test Environment

If you need to reset and test again:

1. Stop the HTTP server (Ctrl+C)
2. Restart WLED device
3. Start HTTP server again
4. Trigger check via JSON API or wait for interval
