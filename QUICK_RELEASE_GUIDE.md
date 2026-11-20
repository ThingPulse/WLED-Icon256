# Icon256 releases

Releases are **drafts first**. The workflow never publishes automatically and
never rewrites `main` or `beta` to advertise an update.

1. Commit and review the firmware and release-tool changes.
2. Choose a new semantic version, for example `17.0.0-icon256.1` for a prerelease.
3. Tag that exact commit and push the tag:

   ```sh
   git tag v17.0.0-icon256.1
   git push origin v17.0.0-icon256.1
   ```

4. The Release Build workflow sets the disposable checkout's package version from
   the tag, builds `icon256`, verifies the embedded binary version and hardware,
   and creates a **draft GitHub release** with the OTA binary, `manifest.json`
   and `SHA256SUMS`.
5. Download the assets while signed in, verify them, and test the OTA update on an
   Icon256 with backed-up settings. Review the release notes before publishing.

```sh
python3 tools/icon256_release.py verify /path/to/downloads/manifest.json
```

Any semantic-version prerelease suffix (`alpha`, `beta`, `rc`, `dev`, `icon256.1`,
etc.) produces a prerelease, never a stable release. A tag without a suffix,
for example `v17.0.0`, produces a stable draft. Do not reuse tags.

Publishing remains a separate human action. Drafts are not update notifications.
The simplified UI does not yet check for or install remote firmware updates.

See [the release process](docs/RELEASE_PROCESS.md) for manifest fields, hardware
compatibility and update-channel URLs.
