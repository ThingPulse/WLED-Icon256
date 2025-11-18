# Quick Release Guide

Fast reference for creating releases.

## Beta Release (F&F Testing)

```bash
git tag v0.16.1-beta
git push origin v0.16.1-beta
```

✅ Automatically:
- Builds firmware
- Creates pre-release on GitHub
- Updates `beta` branch with `version-beta.json`
- Beta testers get update notification

## Production Release

```bash
git tag v0.16.1
git push origin v0.16.1
```

✅ Automatically:
- Builds firmware  
- Creates public release on GitHub
- Updates `main` branch with `version.json`
- Production users get update notification

## Version Numbering

- Beta: `v0.16.1-beta`
- Production: `v0.16.1`
- Always increment, never reuse

## Device Configuration

**Beta Testers**:
```ini
-D REMOTE_OTA_VERSION_URL=\"https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/beta/version-beta.json\"
```

**Production Users**:
```ini
-D REMOTE_OTA_VERSION_URL=\"https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/main/version.json\"
```

## Workflow

1. Develop & test locally
2. Create beta release → F&F testing
3. Fix issues if found
4. Create production release → Public

## Check Status

- GitHub Actions: `https://github.com/ThingPulse/WLED-Icon256/actions`
- Releases: `https://github.com/ThingPulse/WLED-Icon256/releases`

## Troubleshooting

**Workflow fails?**
- Check `npm test` passes locally
- Check `pio run -e icon256` succeeds locally

**No update notification?**
- Verify device has `USERMOD_REMOTE_OTA` enabled
- Check version file URL in browser
- Trigger manual check: `{"rmOTA_check": true}` via API

## Full Documentation

See [docs/RELEASE_PROCESS.md](docs/RELEASE_PROCESS.md) for complete details.
