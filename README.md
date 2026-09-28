# ScummVM for Wii U / Aroma

![ScummVM icon](dists/psp2/icon0.png)

A community-developed, native ScummVM port for **Nintendo Wii U running Aroma**.
Launch **ScummVM** directly from the Wii U Menu and play using the GamePad
touchscreen or the analog stick and buttons. **No RetroArch required.**

This is an **unofficial, experimental port**, not an official ScummVM release.
The current Wii U package includes **only the SCUMM engine**, not every engine
supported by desktop ScummVM. Games are not included.

[Downloads](https://github.com/Rrhapsod/scummvm-wiiu/releases) ·
[Report a Wii U issue](https://github.com/Rrhapsod/scummvm-wiiu/issues) ·
[Upstream ScummVM README](README.scummvm.md)

## Features

- Native Wii U application packaged as `ScummVM.wuhb` for Aroma.
- ScummVM name and icon on the Wii U Menu.
- Standard ScummVM launcher, with video output on the TV and GamePad.
- **Touchscreen pointer control**, including tap-to-click.
- **Analog-stick pointer control** with physical left/right mouse buttons.
- **Native Wii U on-screen keyboard** for text entry and save descriptions.
- Audio playback through the existing SDL mixer backend.
- Game data browsing, configuration and save files on the SD card.
- SDL2-based backend; no Libretro frontend or RetroArch dependency.

## Requirements

- For the WUHB package: a Wii U with Aroma already configured.
- For RPX testing: an existing Wii U Homebrew Launcher/loader with RPX support
  (see the experimental package instructions below).
- A Wii U GamePad and an SD card accessible to the selected environment.
- Your own game data files for a game supported by the included SCUMM engine.

Other controller types have not been validated for this port. Keep backups of
your saves when testing an experimental build.

## Choose a download

| Package | Intended use | Validation |
| --- | --- | --- |
| Aroma ZIP (`ScummVM.wuhb`) | Launch from the Wii U Menu with Aroma | User-tested on real hardware |
| `ScummVM-wiiu-rpx-experimental.zip` | Community testing with a non-Aroma Wii U Homebrew Launcher/loader that supports RPX | **Not yet tested outside Aroma** |

Both packages include the same executable and required data in different
launcher layouts. Choose one; you do not need both on the SD card. The RPX is
not a WUP installer, a Wii/vWii app, or a guarantee of compatibility with every
homebrew environment. For RPX installation and reporting instructions, read the
[RPX testing guide](backends/platform/sdl/wiiu/README.RPX.md).

## Installation (Aroma)

1. Open the [Releases page](https://github.com/Rrhapsod/scummvm-wiiu/releases)
   and download the Wii U installation ZIP attached to a release. GitHub's
   automatically generated **Source code** archives are not installation packages.
2. Extract the **contents** of the installation ZIP to the root of your SD card.
3. Merge existing directories when prompted. Do not delete your existing
   `scummvm` directory, configuration or saves.
4. Insert the SD card and launch **ScummVM** from the Wii U Menu.
5. Choose **Add Game**, browse to a folder containing your game data, and add it.
6. Select the game and choose **Start**.

The installed layout is:

```text
SD:/
├── wiiu/apps/ScummVM.wuhb
└── scummvm/
    ├── data/                 Themes, translations and engine data
    └── doc/                  Documentation and license files
```

On first launch, the application creates `scummvm/scummvm.ini`,
`scummvm/scummvm.log` and `scummvm/saves/` as needed. Game folders can be stored
elsewhere on the SD card. The file browser uses `/vol/external01/` for the SD
root; this is also the path format used in configuration files.

To update, back up your saves and merge the new package onto the SD card.
Replace the existing `wiiu/apps/ScummVM.wuhb` rather than keeping multiple
copies with different filenames.

## GamePad controls

These are the default physical GamePad button mappings for the tested SCUMM
adventure games. Game-specific behavior and custom remapping may change them.

| Input | In-game action |
| --- | --- |
| Touchscreen | Position or move the pointer directly |
| Short tap | Left mouse click |
| Left analog stick | Move the pointer |
| **A** | Left mouse click |
| **B** | Right mouse click |
| Hold **R** | Slow the analog pointer for precise movement |
| **L** | Game menu, equivalent to F5 where supported |
| **+** | ScummVM global main menu |
| **−** | Open the native on-screen keyboard |
| **X** | Skip the current line of dialogue where supported |
| **Y** | Skip or cancel, equivalent to Escape where supported |

In the ScummVM launcher and dialogs, **A** clicks, **Y** closes or cancels,
and the directional pad navigates supported widgets. **B is not a universal
Back button.** The native Wii U keyboard uses its own on-screen controls.

For a right-click while using touch, use **B**; this guide does not assume
multitouch support. To exit ScummVM, use the launcher's **Quit** button.

## Native on-screen keyboard

The Wii U keyboard opens on the GamePad when a supported field requests text
input. If a game's own input field does not open it automatically, select that
field and press **−**.

- **OK** submits the text and dismisses the keyboard. It does **not** press
  Enter or activate the game's Save button.
- Wait for all characters to appear, then choose **Save** in the game dialog.
  Text is delivered gradually to accommodate older SCUMM input routines.
- **Cancel** dismisses only the keyboard, without inserting text or sending
  Escape to the underlying dialog.
- The keyboard starts empty and inserts at the receiving field's cursor.
  It does not load the field's existing contents for editing.
- Original game dialogs may restrict name length and supported characters.
  Plain letters and numbers are recommended for the first save test.

## Hardware testing

The maintainer reports successful tests on a real Wii U with:

| Game | Reported results |
| --- | --- |
| Sam & Max Hit the Road | Gameplay, saving, audio and controls |
| Indiana Jones and the Last Crusade | Gameplay, saving, audio and controls |

The native keyboard integration was also reported working in hardware testing
on September 26, 2026. Earlier SDK tests covered TV/GamePad video, touch,
buttons, audio, SD access and HOME background/return behavior.

These are tests of specific scenarios, **not full-game completion reports or
a guarantee that every game/version works**. Additional community testing is
welcome. The ScummVM menu icon was also confirmed working by the maintainer
on September 28, 2026. **The separate RPX package has not been tested outside
Aroma.** Please do not describe it as confirmed Tiramisu or legacy support yet.

## Current limitations

- Only the **SCUMM engine** is enabled in the current installation package.
  The complete desktop ScummVM compatibility list does not describe this build.
- Optional MP3, Ogg Vorbis and FLAC decoders are not included. Game data that
  requires those codecs may have missing audio or fail to work as expected.
- MT-32 emulation, networking and OpenGL/shaders are not enabled.
- Other controllers and untested games or game editions may behave differently.
- Native keyboard input follows the receiving game's character and length
  restrictions; it is not a synchronized editor for an existing field.

## Reporting issues

Please report port-specific problems to
[this fork's issue tracker](https://github.com/Rrhapsod/scummvm-wiiu/issues)
first. Include:

- Release name and the ScummVM version shown in the launcher.
- Package type (WUHB or RPX), homebrew environment and launcher version.
- Game title, language, platform and edition, such as floppy or CD/talkie.
- Steps to reproduce the problem and whether you used touch or buttons.
- Relevant screenshots and `SD:/scummvm/scummvm.log`.

**Copy the log before launching again:** it is overwritten on the next run.
Review logs and screenshots for personal information before sharing them.
Do not upload game data files.

## Building

The port uses the existing ScummVM build system with devkitPPC, WUT,
devkitPro's Wii U SDL2 branch, target zlib and `wut-tools`. SDL is responsible
for native video, audio, input and Wii U application lifecycle handling.

With a compatible Wii U SDK prepared and its environment activated, build
outside the source tree:

```sh
mkdir build-wiiu
cd build-wiiu
../configure --host=wiiu --disable-all-engines \
  --enable-engine=scumm --disable-detection-full --disable-mt32emu
make -j2 wiiu_release
```

To generate both SD staging trees, use `make -j2 wiiu_release_all` instead.
The Aroma layout is in `wiiu_release/`; the experimental HBL layout is in
`wiiu_rpx_release/`, with `wiiu/apps/scummvm/ScummVM.rpx` and `meta.xml`.
These targets stage files; ZIP archives must be created separately. Neither
layout contains user configuration, saves or games.

The SD installation tree is generated in `wiiu_release/`. The filename is
`ScummVM.wuhb`, and both Wii U display names are `ScummVM`. The icon reuses the
existing 128×128 ScummVM asset at `dists/psp2/icon0.png`.

See [Wii U backend notes](backends/platform/sdl/wiiu/README.WIIU) for SDK
requirements and the accompanying SDL keyboard input patch. An arbitrary
unpatched SDL/WUT installation is not equivalent to the locally tested SDK.

## Credits and license

Based on [ScummVM](https://www.scummvm.org/), with the work of the ScummVM
contributors, [SDL](https://www.libsdl.org/), [devkitPro](https://devkitpro.org/),
[WUT](https://github.com/devkitPro/wut) and the
[Aroma community](https://github.com/wiiu-env).

This fork is maintained by [Rrhapsod](https://github.com/Rrhapsod).
The ScummVM name and icon are credited to the upstream project; their use here
does not imply endorsement or official support.

ScummVM is free software under the GNU General Public License, version 3 or
later. See [COPYING](COPYING), [COPYRIGHT](COPYRIGHT) and [AUTHORS](AUTHORS).
Dependencies and bundled assets retain their respective licenses and credits.
The original project documentation is preserved in
[README.scummvm.md](README.scummvm.md).
