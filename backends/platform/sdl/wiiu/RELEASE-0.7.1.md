# ScummVM for Wii U v0.7.1

My unofficial native Wii U port for **Aroma**, now based on **ScummVM 2026.4.0**.
The community port version is 0.7.1, separate from the upstream version.
No RetroArch is required.

## Changes since 0.7.0

- **Wii Remote IR pointer:** aim at a powered Sensor Bar, click with A/B and
  hold A to drag. A Nunchuk adds analog pointer movement and extra buttons.
- **Remote 2 = Escape/back**, sharing GamePad Y. **Nunchuk C = slow analog
  pointer**, sharing GamePad R. Remote 1 opens the game menu where supported;
  Nunchuk Z shares GamePad X for dialogue skip or a game-specific action.
- **Simultaneous controllers:** GamePad and Remote input can coexist, with
  hotplug handling and shared-button release tracking. Touch remains available.
- **Upstream 2026.4.0 source integration**, including updated translations and
  selected engine data, with the Wii U fixes preserved.
- **Updated build SDK:** devkitPPC r51 / GCC 16.2.0, WUT 1.9.1 and SDL2 2.32.7
  with all seven local keyboard/rendering/timer/IR patches.
- **Complete README control guide** for GamePad and Remote + Nunchuk, including
  default-free inputs, system buttons, keymap exceptions and Xeen bindings.

The existing engine/codec profile remains: SCUMM v0–v8 (no HE), MM/Xeen (no
MM1), SCI/SCI32, Mohawk/Myst/MystME/Riven, Blade Runner and Groovie/Groovie2;
JPEG, MP3, Ogg Vorbis and MT-32 emulation. Native keyboard editing, renderer
cleanup, Curse/QFG4 IO improvements and Groovie pixel conversion remain.
This update does not enable every engine supported by desktop ScummVM.

## Downloads and installation

- **ScummVM-wiiu-0.7.1-aroma.zip** — recommended for Aroma. Extract its contents
  to the SD root and launch **ScummVM** from the Wii U Menu.
- **ScummVM-wiiu-0.7.1-rpx-experimental.zip** — the same executable for testing
  with an existing RPX-capable Wii U Homebrew Launcher/loader. Compatibility
  outside Aroma is still unverified; this is not a WUP/channel installer.
- **SHA256SUMS.txt** — checksums for the installation packages and release files.

Choose one ZIP. Merge folders, replace the application and keep the complete
bundled `scummvm/data` directory, especially `mm.dat`. Preserve configuration,
saves and games. Aroma uses `SD:/wiiu/apps/ScummVM.wuhb`; RPX uses
`SD:/wiiu/apps/scummvm/ScummVM.rpx`. The app name and icon remain **ScummVM**.
Games and MT-32 ROMs are not included. GitHub source archives are not SD
installation packages.

## Controls

The complete physical-button tables are in
[the README](../../../../README.md#controls), also included as
`scummvm/doc/README.md` in each ZIP. Defaults vary with the engine/keymap.

| Action | GamePad | Wii Remote + Nunchuk |
| --- | --- | --- |
| Point | Touch / left stick | IR / Nunchuk stick |
| Left / right click | A / B | A / B |
| Game menu / F5 where supported | L | 1 |
| Escape/back where supported | Y | 2 |
| Skip dialogue where supported | X | Nunchuk Z |
| Slow analog pointer | Hold R | Hold Nunchuk C |
| ScummVM global menu | + | + |
| Native keyboard | − | − |

C slows the stick, not IR positioning. A powered Sensor Bar is needed for IR;
the Nunchuk is optional. World of Xeen uses X/Z for Enter and Y/2 for Escape.
The native keyboard follows its own on-screen prompts. Keyboard OK accepts the
text but does not press Save; wait for the full name, then select Save.

## Validation and limitations

On October 22, 2026, I reported a successful console test of the 2026.4.0 Aroma
predecessor, source `90b6e646`, after earlier Remote/remapping success reports.
I supplied no additional log or per-game/scenario breakdown. The 0.7.1 source
retains that runtime implementation and SDK; its changes are version metadata,
build/staging defaults and documentation. I have not separately run the newly
packaged 0.7.1 binary on my console. Exact source/profile/hashes are recorded in
`BUILD-INFO.txt` and `SHA256SUMS.txt`.

Earlier game results in the README are specific tests on earlier builds,
not full playthrough or every-edition guarantees. Pro-only keyboard/HOME,
specifically identified Vorbis assets, Myst/7th Guest and non-Aroma RPX still
need targeted console reports. Clandestiny/Tender Loving Care/Uncle Henry
community reports remain unresolved. Networking/OpenGL, HE/MM1, unlisted
engines, FLAC, MPEG-2, Opus and Theora are not enabled.

Report issues at https://github.com/Rrhapsod/scummvm-wiiu/issues with the game
edition, package/environment, reproduction steps and `SD:/scummvm/scummvm.log`.
Copy the log before relaunching; normal logging overwrites it.

Based on ScummVM, SDL2, devkitPro, WUT and Aroma community work. I maintain this
unofficial port as Rrhapsod; bundled licenses and credits apply.
