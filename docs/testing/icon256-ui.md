# Icon256 simplified UI: first implementation

The approved design reference remains `docs/ui-mockup/index.html`, unchanged.
Firmware UI sources live in `usermods/usermod_v2_icon256_wordclock/ui/`.
`build_ui.py` embeds those assets as a gzip PROGMEM array when PlatformIO builds
the word-clock usermod. No filesystem upload or CDN is required.

## Implemented

- Device root `/` and `/icon256` serve the simplified UI. `/?advanced=1` retains
  the upstream WLED UI; its assets and settings endpoints remain unchanged.
- Power, brightness, word clock, scrolling text, GEQ and Matrix rain use WLED's
  JSON state API and native effects. Entering GEQ enables AudioReactive.
- Per-mode appearance supports solid colors, five gradients and horizontal,
  vertical or diagonal direction. GEQ defaults to red/yellow/green from top to
  bottom; rain uses white spawn pixels and green trails.
- Tinting happens in the usermod's output overlay, leaving effect simulation
  buffers intact. Advanced disables the overlay so upstream color controls work.
- Appearance changes are coalesced and persisted after three seconds.
- SVG front plates are copied from the approved mock; previews use actual WLED
  WebSocket LED frames, including text, equalizer and rain. No simulated clock
  or audio is shown.
- Setup includes Wi-Fi scanning/manual SSID, device name, timezone, front plate,
  seconds visibility and German wording preferences. The five mock timezones
  map to WLED timezone IDs; an existing different timezone remains selectable.
- English/German interface preference is stored per browser. Front-plate language
  and clock settings are stored on the device. Existing settings PIN protection
  applies to configuration saves.

## Remaining phases / validation

- Fork-specific firmware availability checks and confirmed installation are not
  implemented here. The production UI displays only the installed version.
- Native iOS/Android web-frame verification and captive-portal Wi-Fi provisioning
  on a fresh device still require hardware/app testing.
- WLED supports one live LED WebSocket viewer at a time. A second live viewer can
  take over the preview stream without affecting control or the physical display.
- Brightness, message and selected mode retain WLED's normal state/preset startup
  behavior; this change does not overwrite users' boot presets.
- New Wi-Fi credentials are not exercised against the user's working network as
  part of automated testing. Blank password preserves the existing password.

## Checks

Run the normal `npm ci`, `npm run build`, `npm test`, and PlatformIO `icon256` and
`esp32dev` builds. Run `python3 test/icon256_wordclock/test_german.py` for wording
regressions.

The optional browser smoke test uses Playwright and its Chromium installation:

```sh
node tools/icon256-ui-smoke.cjs
```

If Playwright is installed outside the repository, set `PLAYWRIGHT_MODULE` to its
module path. `CHROMIUM_PATH` optionally selects an installed executable.
`SCREENSHOT_DIR` optionally saves mobile screenshots. The test uses isolated API
fixtures, never the physical device. It checks mobile/desktop overflow, German
labels, live SVG pixels, gradient/mode request payloads, text handling and setup
saves.

## Verification performed on the connected Icon256

Both firmware targets compiled; the 16 Node tests and 11,520 German sentence
frames passed. Browser fixture checks passed at mobile and desktop widths.
After flashing, real-device browser checks verified all four mode IDs, live SVG
pixels, a diagonal rainbow gradient (23 distinct displayed colors), and the
upstream advanced page. A setup-form seconds-visibility save succeeded while
preserving audio settings, network configuration and the NTP server. The original
clock display state and seconds visibility were restored after those checks.
