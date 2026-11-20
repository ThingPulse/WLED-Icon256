# Icon256 market assets

`icon256.png` is the existing ThingPulse product thumbnail copied from
`esp-app-market/src/assets/devices/icon256/icon256-thumb.png`.

`tools/icon256_app_market.py` combines the Icon256 bootloader, partition table,
OTA boot data, and application into a USB image, verifies the partition layout
and application metadata, then generates the market manifest and SHA-256 hashes.
The application starts at `0x10000`; the factory image is flashed at zero.

The existing release workflow packages and verifies both USB and OTA assets.
Only a published stable release is imported by the production app market.
Use a new tag containing these tooling changes; rerunning an old tag checks out
its old workflow and cannot produce these assets. The previously tested 17.0.1
factory build remains the manual market fallback until that first release.
