# FAT USB dependencies for the October 28, 2026 candidate

I added FAT USB mounting through libfat and libmocha. This is a native Wii U
path: it does not use the Wii/vWii libogc USB stack. The existing WUT SD mount,
SDL patches, ProcUI owner, input, graphics and audio setup are retained.

The first target is **one FAT32 USB drive connected before launch**, using
512-byte logical sectors and a conventional MBR partition table (or a FAT
volume without a partition table). NTFS, exFAT, GPT, multiple USB drives,
hot removal and Wii U formatted/encrypted storage are outside this candidate.
A WFS mount such as `/vol/storage_usb01` does not provide FAT32 access.

## Pinned sources and local changes

- [wiiu-env/libfat](https://github.com/wiiu-env/libfat), 1.1.5 Wii U fork,
  commit `6517ae03e07388c6b01d05baf245772bdf7b476e`.
  Apply `libfat-wiiu-usb.patch`: Mocha disc interface instead of IOSUHAX;
  recursive WUT partition mutex for audio/main-thread access; aligned cache;
  partial-allocation and devoptab-registration cleanup; mirrored FAT writes
  and correct active-FAT selection; unsigned little-endian shift.
  This is a locally modified library, not an unmodified upstream release.
- [wiiu-env/libmocha](https://github.com/wiiu-env/libmocha),
  commit `50fefdf8307a875c63bdbdcf6c973779d4ddac92`.
  Apply `libmocha-wiiu-usb.patch`: match newlib's 32-bit `long` handle typedefs
  and explicitly cast hexadecimal diagnostic arguments. No format-warning
  suppression or custom IOS kernel/ProcUI patch is added.
- Build both with the same WUT/devkitPPC profile as SDL/ScummVM. Install headers
  and static archives under that profile's `wut/usr/{include,lib}`.
  ScummVM links `-lfat -lmocha`; licenses are included in installation packages.

Locally I pin `sdk/versions/r51-wut1.9.1-sdl2.32.7-usb-20261028`, derived from
`r51-wut1.9.1-sdl2.32.7-local`. The shared `sdk/current` remains unchanged.
The compiler, WUT, SDL and codecs retain their previous bytes, apart from a
relocated libmad metadata prefix. See the profile manifest for dependency
source/patch/archive hashes. Previous release packages are preserved.

## Reproduce without modifying an active SDK

With the shared SDK environment activated, choose **new, absent** installation
and source-cache directories, then run:

```sh
WIIU_USB_SDK_DEST=/absolute/path/to/new-sdk-profile \
WIIU_USB_SOURCES=/absolute/path/to/shared-sdk/sources/usb-rebuild \
  bash backends/platform/sdl/wiiu/build-usb-sdk.sh
```

The helper copies the selected profile, clones the pinned library commits,
applies the local patches, builds and installs there, and writes a manifest.
It refuses existing destinations and does not switch the shared default.
For standalone SDKs, set `DEVKITPRO`, `DEVKITPPC` and `WUT_ROOT` to the resulting
profile before building ScummVM. In the shared workspace, set `WIIU_SDK_ROOT`
and re-source `sdk/env.sh`, then use a fresh out-of-tree build directory.

## Ownership and verification

`WiiUUSBStorage` in main mounts before the backend and stays alive until after
`g_system->destroy()`. It mounts only `usb:`, refuses an occupied device name,
and closes failed raw mounts. On exit it flushes libfat before closing the
raw USB/FSA handle and libmocha. It never calls `fatInitDefault`, unmounts SD,
formats storage, moves saves or redirects the configured save path.

`tests/usb/run.py` checks actual ownership code with failure/lifecycle mocks.
`tests/usb/fat-image.py LIBFAT_SOURCE LIBMOCHA_SOURCE SDK_PROFILE` compiles the
actual patched libfat with mock newlib/OS/block transport. It reads synthetic
files while saving/replacing files, then compares bytes with mtools and runs
`fsck.fat -n` on superfloppy and MBR FAT32 images. These tests access temporary
regular files only. They do not validate a USB bridge, power supply, Mocha IPC,
Wii U big-endian execution or physical save durability.

Hardware tests must cover SD-only startup, FAT32 enumeration, game startup,
new save/load, overwrite, Quit/relaunch/reload, HOME/return and SD regression.
Connect the drive before launch and leave it connected until ScummVM exits.
Use the candidate guide for user-facing installation and test steps.
