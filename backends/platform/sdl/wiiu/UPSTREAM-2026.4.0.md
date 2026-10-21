# ScummVM Wii U — upstream 2026.4.0 update

I updated the native Wii U/Aroma port to the official ScummVM **2026.4.0** tag,
commit `7d2709da5136866589f7602f5f5a7b17de03420e`, released October 11, 2026.
This merges the release source, translations and engine data. My community
port version remains **0.7.0**, separate from the upstream version.

## Included behavior

- Wii Remote IR pointing with a Sensor Bar, A/B left/right mouse clicks,
  simultaneous GamePad input and controller hotplug remain available.
- **Remote 2** shares GamePad Y: Escape/back through the active keymap.
  **Nunchuk C** shares GamePad R: slow analog pointer movement.
- Native keyboard editing, touch, audio, renderer cleanup, HOME lifecycle,
  Curse/QFG4 IO improvements and Groovie pixel conversion are retained.
- The upstream event cleanup fix is included: queued keyboard/mouse events
  are purged through the event manager instead of polling in engine destruction.
- I retained the existing engine profile: SCUMM v0–v8 (no HE), MM/Xeen (no MM1),
  SCI/SCI32, Mohawk/Myst/MystME/Riven, Blade Runner and Groovie/Groovie2.
  Updating the base does not enable every newly supported desktop engine.
- MP3, Ogg Vorbis, JPEG and MT-32 emulation remain enabled. Detailed diagnostic
  instrumentation is disabled; ordinary ScummVM logging remains available.

## Installation

Use **ScummVM-wiiu-2026.4.0-aroma.zip** with Aroma. Extract its contents to the
SD root, merging folders and replacing matching application/data files.
Keep the complete bundled `scummvm/data` directory, including `mm.dat`.
Preserve the existing configuration, saves and game folders. The application
remains `SD:/wiiu/apps/ScummVM.wuhb`, with the ScummVM name and icon.

The separate **ScummVM-wiiu-2026.4.0-rpx-experimental.zip** contains the same
executable for RPX loader testing. Non-Aroma compatibility remains unverified.
Choose one installation package. No games, ROMs, saves or configuration files
are included. The older packages remain separate and unchanged.

## Validation

I cross-built this update in a fresh directory using the shared devkitPPC r51 /
GCC 16.2.0 SDK, WUT 1.9.1 and SDL2 2.32.7 with all seven existing local patches.
Exact source revision, profile, artifact hashes and package verification are
recorded in the accompanying `BUILD-INFO.txt` and `SHA256SUMS.txt`.

I reported that the preceding October 21 button-remapping/r51 candidate worked.
That report belongs to binary `d37ee3f4`, not this upstream update. I still need
to test the exact 2026.4.0 package on my Wii U: launcher/input, game startup and
audio, save-name editing, return/reopen and HOME/exit. Existing game reports
are scenario-specific; they do not establish full 2026.4.0 compatibility.

For controls and audio setup, see the bundled README. Report issues with the
game edition, reproduction steps and `SD:/scummvm/scummvm.log`; copy the log
before another launch overwrites it.

Based on ScummVM, SDL2, devkitPro, WUT and the Aroma community. I maintain this
unofficial port as Rrhapsod; bundled licenses and credits apply.
