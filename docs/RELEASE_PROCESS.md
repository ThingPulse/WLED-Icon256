# WLED Icon256 Release Process

This document describes the complete release process for WLED Icon256, including beta testing and production releases.

## Overview

The release system uses a **two-tier approach**:
1. **Beta Releases** - For Friends & Family (F&F) testing
2. **Production Releases** - For public distribution

This ensures thorough testing before public release, with minimal configuration required on devices.

## How It Works

### Branch-Based Update System

Instead of using version-specific URLs, the system uses **stable branch-based URLs**:

- **Beta Branch**: `https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/beta/version-beta.json`
  - Updated automatically when beta releases are created
  - Used by F&F testers

- **Main Branch**: `https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/main/version.json`
  - Updated automatically when production releases are created
  - Used by production devices

**Key Benefit**: Devices configure their update URL once at build time or via `-D` flag. They automatically receive new versions as the branches update.

## Release Workflow

### 1. Creating a Beta Release

Beta releases are for F&F testing before public release.

```bash
# Create and push a beta tag
git tag v0.16.1-beta
git push origin v0.16.1-beta
```

The GitHub Actions workflow will:
1. ✅ Build firmware with the version number
2. ✅ Run tests to ensure quality
3. ✅ Create a GitHub pre-release (marked as beta)
4. ✅ Generate `version-beta.json` with download URL
5. ✅ Update the `beta` branch with the new version file

**Beta testers** will automatically see update notifications on their devices because they're configured to check `https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/beta/version-beta.json`.

### 2. Creating a Production Release

Once beta testing is complete, create a production release:

```bash
# Create and push a production tag (no suffix)
git tag v0.16.1
git push origin v0.16.1
```

The GitHub Actions workflow will:
1. ✅ Build firmware with the version number
2. ✅ Run tests to ensure quality
3. ✅ Create a GitHub release (public)
4. ✅ Generate `version.json` with download URL
5. ✅ Update the `main` branch with the new version file

**Production devices** will automatically see update notifications because they're configured to check `https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/main/version.json`.

## Version Numbering

Follow [Semantic Versioning](https://semver.org/):

- **Beta**: `v0.16.1-beta`, `v0.16.2-beta`
- **Production**: `v0.16.1`, `v0.16.2`

**Rules**:
- Beta releases should have `-beta` suffix
- Production releases have NO suffix
- Always increment version numbers
- Never reuse version numbers

## Configuration Examples

### For Beta Testers (F&F)

Build with beta environment:
```ini
[env:icon256_beta]
extends = env:icon256
build_flags = 
  ${env:icon256.build_flags}
  -D REMOTE_OTA_VERSION_URL=\"https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/beta/version-beta.json\"
```

Or flash with:
```bash
pio run -e icon256_beta --target upload
```

### For Production Users

Build with production environment:
```ini
[env:icon256]
build_flags = 
  -D REMOTE_OTA_VERSION_URL=\"https://raw.githubusercontent.com/ThingPulse/WLED-Icon256/main/version.json\"
```

Or flash with:
```bash
pio run -e icon256 --target upload
```

## Testing the Release Process

### Manual Testing Checklist

Before your first real release, test the workflow:

1. **Create a test beta release**:
   ```bash
   git tag v0.16.0-beta-test
   git push origin v0.16.0-beta-test
   ```

2. **Verify the workflow**:
   - Check GitHub Actions completes successfully
   - Verify release appears in GitHub Releases (as pre-release)
   - Check that `beta` branch was created/updated
   - Verify `version-beta.json` exists in beta branch

3. **Test from a device**:
   - Flash firmware with `icon256_beta` environment
   - Wait 5 minutes for auto-check or trigger manual check via API
   - Verify update notification appears in web UI
   - Click "Update Now" and verify firmware updates

4. **Create a test production release**:
   ```bash
   git tag v0.16.0-test
   git push origin v0.16.0-test
   ```

5. **Verify production workflow**:
   - Check GitHub Actions completes successfully
   - Verify release appears in GitHub Releases (public)
   - Check that `main` branch was updated
   - Verify `version.json` exists in main branch

6. **Clean up test releases**:
   ```bash
   # Delete test tags
   git tag -d v0.16.0-beta-test v0.16.0-test
   git push origin :refs/tags/v0.16.0-beta-test
   git push origin :refs/tags/v0.16.0-test
   
   # Delete test releases from GitHub UI
   ```

## Troubleshooting

### Workflow Fails

**Problem**: GitHub Actions workflow fails

**Solutions**:
- Check workflow logs in GitHub Actions tab
- Verify `npm test` passes locally
- Verify `pio run -e icon256` completes successfully locally
- Ensure all required files are committed

### Version File Not Updated

**Problem**: `beta` or `main` branch doesn't have version file

**Solutions**:
- Check workflow completed all jobs (including branch update jobs)
- Verify GitHub Actions has write permissions to repository
- Check for git conflicts in branch update step

### Devices Don't See Updates

**Problem**: Devices don't show update notifications

**Solutions**:
- Verify device firmware includes `USERMOD_REMOTE_OTA`
- Check device has correct `REMOTE_OTA_VERSION_URL` compiled in
- Manually trigger check via API: `http://device-ip/json/state` with `{"rmOTA_check": true}`
- Verify version file URL is accessible (check in browser)
- Check device logs for HTTP errors

### Wrong Version in Firmware

**Problem**: Firmware shows wrong version number in filename

**Solutions**:
- Update the `sed` pattern in workflow's "Rename firmware" step
- Currently expects `WLED_0.16.0-alpha` - adjust to match your base version
- Alternatively, update version in `wled00/wled.h` before release

## Best Practices

1. **Always test beta first**: Create beta releases for new features
2. **Wait for feedback**: Give beta testers time to report issues
3. **Don't skip versions**: Always increment version numbers
4. **Document changes**: Include meaningful release notes
5. **Keep branches clean**: Only workflow should update beta/main branches
6. **Monitor workflow**: Check GitHub Actions after each release
7. **Test on real hardware**: Always test OTA updates on actual devices

## Version File Format

Both `version.json` and `version-beta.json` use this format:

```json
{
  "version": "0.16.1",
  "url": "https://github.com/ThingPulse/WLED-Icon256/releases/download/v0.16.1/WLED_0.16.1_ESP32_S3_Icon256.bin",
  "release_notes": "https://github.com/ThingPulse/WLED-Icon256/releases/tag/v0.16.1",
  "published_at": "2024-01-15T10:30:00Z",
  "is_beta": false
}
```

The Remote OTA usermod compares `version` against the device's current version (from `versionString` in WLED).

## Manual Rollback

If a release has critical issues:

1. **For beta**: Push previous version file to `beta` branch
2. **For production**: Push previous version file to `main` branch

```bash
# Example: Rollback beta to v0.16.0
git checkout beta
# Edit version-beta.json to point to v0.16.0
git add version-beta.json
git commit -m "Rollback to v0.16.0 due to critical bug"
git push origin beta
```

Devices will see the older version as "available" again if their current version is newer.

## Security Notes

- Version check URLs use HTTPS via GitHub's CDN
- Firmware downloads use HTTPS via GitHub Releases
- The Remote OTA usermod verifies ESP32's built-in signature checking
- Never use HTTP URLs for production

## Further Reading

- [Remote OTA Usermod README](../usermods/usermod_v2_remote_ota/readme.md)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [WLED Documentation](https://kno.wled.ge/)
