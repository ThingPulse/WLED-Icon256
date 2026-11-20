# Icon256 release process

## Version identity

The Git tag is the release version source. `tools/icon256_release.py prepare`
validates a `v`-prefixed semantic version and updates `package.json` and the root
package-lock versions **inside the disposable CI checkout** before `npm ci` and
compilation. Upstream metadata and web build scripts already read package.json,
so no upstream source patch is required. Ordinary developer builds keep the
repository's package version. The release tool rejects binaries whose embedded
version differs from the tag; renaming an old binary is not sufficient.

Example prerelease: `v17.0.0-icon256.1`. Any prerelease suffix is recognized,
including alpha/dev labels, rather than only beta and rc. Version strings must fit
the firmware's 48-byte metadata field. Tags and repository names are validated,
and workflow shell commands receive GitHub values through environment variables.

## Hardware compatibility

The tracked `platformio_icon256.ini` configures fresh CI builds without relying
on a developer's ignored `platformio_override.ini`:

| Target | Embedded release identity | OTA artifact |
| --- | --- | --- |
| icon256 | ESP32_S3_Icon256 | WLED_VERSION_ESP32_S3_Icon256.bin |
| s3-rainbow | ESP32_S3_Rainbow | Not distributed by the Icon256 release workflow |

Icon256 retains its existing identity so currently installed devices remain
compatible. Rainbow's old builds incorrectly used the Icon256 identity. Those
legacy Rainbow devices need a separate migration (for example USB flashing);
never bypass the compatibility check globally or offer them Icon256 updates.
The standard WLED name check cannot distinguish two historical builds that both
claimed the same identity. The new identifiers prevent that ambiguity going
forward.

The release validator requires ESP32-S3, a 4 MB flash declaration, a complete
hashed ESP application image, and exact Icon256 metadata. It checks the segment
checksum and appended ESP SHA-256, then generates a SHA-256 over the complete
file. The image must fit both 0x180000-byte application slots in
`tools/WLED_ESP32_4MB_1MB_FS.csv`. This is an OTA application image at offset
0x10000, not a factory image for flashing from address zero. Signed images are
not accepted by this packaging version.

## Workflow and review boundary

A pushed `v*` tag or manual workflow run for an existing tag builds and verifies
assets, uploads them as an Actions artifact, verifies them again after download,
and creates a draft release using the runner's GitHub CLI. Only the draft job has
`contents: write`. Existing releases are not overwritten. There are no branch
updates, force pushes, or automatic public releases.

The commit named by the tag must contain the new release script and workflow.
Download the draft's assets while signed in. Verify the manifest, inspect release
notes, and test actual OTA installation plus reboot/settings preservation on an
Icon256 before publishing. A successful build or package check does not substitute
for that OTA test. Draft URLs are not publicly downloadable update URLs.

## Manifest schema 1

`manifest.json` contains:

- `schema_version`, `version`, `tag`, `repository`, `channel` (stable/prerelease),
  `source_commit`, `built_at` (UTC build time), and `release_notes` URL.
- `firmware.target`, `chip`, `flash_size`, `release_name`, `format`, `offset`,
  `max_size`, `size`, `sha256`, `filename`, and immutable release download `url`.

`SHA256SUMS` accompanies the binary. Checksums detect corruption/mismatches; they
are not independent signatures or a replacement for trusting the HTTPS release
source. The future updater must check identity, channel, version, size, hash and
WLED's own OTA compatibility rules before installation.

## Update channels

After a stable draft is explicitly published and designated the latest release,
the stable discovery URL is:

`https://github.com/ThingPulse/WLED-Icon256/releases/latest/download/manifest.json`

For opted-in prerelease users, discover published releases using the GitHub
releases API and select a compatible prerelease by semantic version. Do not use
an array's first entry as a version comparison. Drafts and incomplete releases
must never be offered. Each selected release's manifest and firmware URLs are
tag-specific. Stable installations must ignore prereleases unless the user opts
in. Handle unavailable endpoints, GitHub rate limiting and no published releases
as normal no-update/error states, without blocking the display.

The former `main/version.json` and `beta/version-beta.json` scheme is retired.
`REMOTE_OTA_VERSION_URL` was unused and has been removed. The simplified UI's
update notification/download/install feature remains a separate implementation.

## Local validation

```sh
python3 -m unittest discover -s test/icon256_release -v
python3 tools/icon256_release.py package \
  --tag v17.0.0-devV5 --repository ThingPulse/WLED-Icon256 \
  --commit "$(git rev-parse HEAD)" \
  --firmware .pio/build/icon256/firmware.bin --out build_output/draft-release
python3 tools/icon256_release.py verify build_output/draft-release/manifest.json
```

The tag in the example must match the built firmware's embedded version. To test
another release version, use a separate checkout and run `prepare` there before
building; do not change a developer checkout's package version merely to rename
an existing binary.

## OTA name mismatch in early development builds

An upstream metadata helper copied a short release name but terminated the
buffer at its capacity rather than at the copied length. Uninitialized bytes
could be appended to the name during OTA validation. The one-line correction in
`wled_metadata.cpp` is covered by `test/ota_metadata/test_strings.py`, which runs
the actual helper with compiler-initialized poison bytes in automatic storage.

Devices already running the affected checker may reject a correctly named
image. Repair that checker once over USB, preserving the filesystem, then test
OTA with validation enabled. Do not disable hardware validation as a general
workaround. The first Icon256 draft includes the correction for future updates.
