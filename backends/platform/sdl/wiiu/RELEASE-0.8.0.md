# ScummVM Wii U 0.8.0

I prepared this community Wii U/Aroma release on November 18, 2026, based on
**ScummVM 2026.4.0**. It brings the combined engine profile, stabilized Wii
Remote Pointer and FAT32 USB game loading into the normal release build.

## Changes since 0.7.1

- **Full Wii Remote IR Pointer support** with fine movement and adaptive
  stabilization to reduce stationary jitter, A/B mouse clicks, held-button
  dragging, optional Nunchuk movement and GamePad touch/stick handover.
  Remote **2 = Escape/back**; Nunchuk **C = slow analog movement**.
- **Run games from FAT32 USB storage.** Add Game → Go up to `/` → `usb:/`.
  The browser now handles WUT `fs:` aliases and returns correctly to the
  SD/USB drive list. Configuration, data, logs and default saves stay on SD.
  An existing USB save directory can be selected explicitly.
- **15 integrated engine families:** SCUMM (v0–v8), MM/Xeen, SCI/SCI32,
  Mohawk (Myst/MystME/Riven), Blade Runner, Groovie/Groovie2, Tinsel,
  Sword1, Sword2, AGOS, Gob, Kyra/EOB, Darkseed, Sherlock and SAGA/IHNM.
- Discworld 1/2, Broken Sword 1/2, Elvira 1/2, Simon 1/2, Gobliiins trilogy,
  Woodruff, Kyrandia Books 1–3, EOB I/II, Dark Seed, Sherlock's Scalpel/Rose
  Tattoo and I Have No Mouth, and I Must Scream are in the enabled scope.
  Rebel Assault I/II use the existing SCUMM engine. **EOB III is unsupported
  by this base.** Integration is not a full-playthrough compatibility claim.
- Faster Kyrandia 3 AUD loading and corrected IHNM opening-skip controls
  (GamePad Y / Remote 2), retaining normal mouse/panel input.
- MP3, Ogg Vorbis, JPEG, MT-32 emulation, native keyboard, renderer cleanup,
  Curse/QFG4 IO improvements and Groovie pixel conversion remain included.
  Candidate-specific diagnostics are disabled; normal logging remains.

## Downloads and installation

- **ScummVM-wiiu-0.8.0-aroma.zip** — recommended for Aroma. Extract its
  contents to the SD root and launch ScummVM from the Wii U Menu.
- **ScummVM-wiiu-0.8.0-rpx-experimental.zip** — the same executable for
  testing with an existing RPX-capable Homebrew Launcher/loader. Non-Aroma
  compatibility remains unverified. This is not a WUP/channel installer.
- **SHA256SUMS.txt**, **BUILD-INFO.txt** — package integrity and exact source,
  SDK/profile and verification details.

Choose one ZIP, merge folders, replace the application and keep the complete
bundled `scummvm/data` directory, especially `mm.dat`, `kyra.dat` and
`darkseed.dat`. Preserve your configuration, saves and games. Aroma uses
`SD:/wiiu/apps/ScummVM.wuhb`; RPX uses `SD:/wiiu/apps/scummvm/ScummVM.rpx`.
The app name and upstream icon remain ScummVM. Games and MT-32 ROMs are not
included. GitHub's automatic source archives are not installation packages.

## Controls and USB setup

The [README](../../../../README.md) has the complete GamePad and Wii Remote +
Nunchuk button tables, engine/game list, Pointer and USB guides. It is included
as `scummvm/doc/README.md` in both ZIPs. Defaults vary with engine/keymap.

| Action | GamePad | Wii Remote + Nunchuk |
| --- | --- | --- |
| Point | Touch / left stick | Sensor Bar IR / Nunchuk stick |
| Left / right click | A / B | A / B |
| Game menu / F5 where supported | L | 1 |
| Escape/back; IHNM opening skip | Y | 2 |
| Skip dialogue where supported | X | Nunchuk Z |
| Slow analog pointer | Hold R | Hold Nunchuk C |
| ScummVM global menu | + | + |
| Native keyboard | − | − |

The Nunchuk is optional for IR; a powered Sensor Bar is required. C slows
analog movement, not absolute IR. World of Xeen uses X/Z for Enter and Y/2
for Escape. Keyboard OK accepts text but does not press Save.

Connect **one FAT32 USB drive before launch**, preferably with MBR partitions
and 512-byte logical sectors. Keep the SD card inserted. Cancel the Wii U
Menu's format prompt for a PC-readable drive. Go up to `/` in Add Game and
choose `usb:/`; `external01/` is SD. ScummVM does not format or migrate files.
NTFS, exFAT, GPT, encrypted Wii U storage, multiple drives and hot removal are
outside this configuration. No specific USB port/hub/model is established by
my report. Leave the drive connected until the app exits.

## Validation and limitations

I reported Simon 1, Broken Sword 1 and Goblins Quest 3 successful during the
combined-engine tests, followed by Kyrandia 3 and IHNM after their fixes. I
then reported the stabilized Remote Pointer excellent. On November 18 I
reported the USB browser fix worked perfectly. My November 17 log identifies
**6e1b8130**, confirms a FAT USB mount, Blade Runner Windows/English startup
and clean shutdown. It does not print the game data path or prove USB save
persistence. USB save/load/overwrite/relaunch still needs targeted console
validation. These reports cover specific scenarios, not every integrated game.

The final 0.8.0 source retains that runtime implementation with release
metadata, build/staging and documentation changes, and diagnostics disabled.
The new final binary has not had a separate console/Cemu run. Local full
cross-build and package validation are recorded in BUILD-INFO.txt. The normal
release uses the pinned nine-patch SDL + libfat/libmocha profile; its source
recipes, patches, credits and licenses accompany the installation packages.

HE/MM1/AGOS2/Lands of Lore/Rebel2 PSX/TinyGL, networking and OpenGL are off.
FLAC, MPEG-2, Opus and Theora are disabled. Sword SMK/DXA cutscenes are included.
Other untested games/editions, specifically identified Vorbis assets,
Pro-only keyboard/HOME and non-Aroma RPX still need hardware reports.

Report issues at https://github.com/Rrhapsod/scummvm-wiiu/issues with the exact
package, game edition, paths, steps and `SD:/scummvm/scummvm.log`. Copy the
log before relaunching. For USB issues include model, power and partition
layout. Keep backups of saves.

I maintain this unofficial port as Rrhapsod, building on ScummVM, SDL2,
devkitPro, WUT, libfat, libmocha and Aroma community work. Included credits
and licenses apply.
