# ScummVM for Wii U / Aroma — v0.7.1

![ScummVM icon](dists/psp2/icon0.png)

A community-developed, native ScummVM port for **Nintendo Wii U running Aroma**.
Launch **ScummVM** directly from the Wii U Menu and play using the GamePad
touchscreen or the analog stick and buttons. **No RetroArch required.**

This is an **unofficial, experimental port**, not an official ScummVM release.
Version **0.7.1** includes **SCUMM v0–v8, MM/Xeen, SCI/SCI32, Mohawk,
Blade Runner and Groovie/Groovie2**, not every engine supported by desktop
ScummVM. Humongous Entertainment and MM1 remain disabled. Games and MT-32 ROMs
are not included.

The community port version is **0.7.1**, based on **ScummVM 2026.4.0**.
See the [0.7.1 release notes](backends/platform/sdl/wiiu/RELEASE-0.7.1.md)
for the changes, packages and validation details.

This release adds **Wii Remote IR pointing and Nunchuk support**, including
simultaneous GamePad input. **Remote 2 = Escape/back; Nunchuk C = slow analog
pointer.** I reported a successful console test of the 2026.4.0 predecessor
before preparing 0.7.1. The complete control guide is below.

[Downloads](https://github.com/Rrhapsod/scummvm-wiiu/releases) ·
[Report a Wii U issue](https://github.com/Rrhapsod/scummvm-wiiu/issues) ·
[Upstream ScummVM README](README.scummvm.md)

## Features

- Native Wii U application packaged as `ScummVM.wuhb` for Aroma.
- ScummVM name and icon on the Wii U Menu.
- Standard ScummVM launcher, with video output on the TV and GamePad.
- **Touchscreen pointer control**, including tap-to-click.
- **Analog-stick pointer control** with physical left/right mouse buttons.
- **Wii Remote IR pointer**, A/B mouse clicks, Nunchuk movement and precision mode.
- Simultaneous GamePad/Remote input and controller hotplug.
- **Native Wii U on-screen keyboard** for text entry and save descriptions.
- Audio playback through the existing SDL mixer backend.
- MP3 decoding (libmad) and integrated MT-32 emulation, both tested on my Wii U.
- Ogg Vorbis decoding for compatible game audio resources.
- Existing-text editing in supported launcher/ScummVM and original SCUMM fields.
- Faster Curse of Monkey Island transitions and Quest for Glory IV resource loading.
- Faster Groovie pixel conversion; my latest 11th Hour movie/audio test improved.
- Game data browsing, configuration and save files on the SD card.
- SDL2-based backend; no Libretro frontend or RetroArch dependency.
- Corrected renderer texture cleanup for repeated game/launcher transitions;
  I confirmed that this fix resolved the repeated-exit freeze on my Wii U.

## Requirements

- For the WUHB package: a Wii U with Aroma already configured.
- For RPX testing: an existing Wii U Homebrew Launcher/loader with RPX support
  (see the experimental package instructions below).
- A Wii U GamePad, or a paired Wii Remote with a powered Sensor Bar for IR
  pointing; a Nunchuk is optional for analog movement and extra buttons.
- An SD card accessible to the selected environment.
- Your own game data files for a game supported by an included engine.

I reported successful Wii Remote pointing, button-remapping and upstream-update
tests on Aroma. Pro-only native keyboard/HOME and other controller combinations
still need targeted tests.
Keep backups of your saves when testing an experimental build.

## Choose a download

| Package | Intended use | Validation |
| --- | --- | --- |
| Aroma ZIP (`ScummVM.wuhb`) | Launch from the Wii U Menu with Aroma | The 2026.4.0 predecessor passed my console test; 0.7.1 changes version metadata and documentation |
| RPX experimental ZIP (`ScummVM.rpx`) | Community testing with a non-Aroma Wii U Homebrew Launcher/loader that supports RPX | **Not yet tested outside Aroma** |

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

## Controls

These are the defaults for the launcher and the usual SCUMM adventure keymap.
Other engines and saved custom mappings can change the in-game actions.
Buttons are listed by their **physical Nintendo labels**. The native Wii U
keyboard follows its own on-screen prompts while it is open.

### Wii U GamePad

| Input | Default action |
| --- | --- |
| Touchscreen | Position/move the pointer directly |
| Short tap | Left mouse click |
| Left analog stick | Move the pointer |
| **A** | Left mouse click; hold while moving to drag |
| **B** | Right mouse click |
| **X** | Skip the current dialogue line (period key) where supported |
| **Y** | Escape: skip/cancel/back where supported; close launcher dialogs |
| **D-pad ↑ ↓ ← →** | Navigate launcher widgets; game-specific directional controls |
| **L** | Game menu / F5 where supported |
| Hold **R** | Slow the analog pointer for precise movement |
| **ZL / ZR** | No general adventure default; available as left/right triggers for remapping or engine-specific bindings |
| Right analog stick | No general adventure default; available for remapping or engine-specific bindings |
| Left/right stick clicks (**L3 / R3**) | No general adventure default; available for remapping |
| **+** | ScummVM global main menu |
| **−** | Open the native on-screen keyboard |
| **HOME** | Wii U system HOME interface / application lifecycle |
| **TV / POWER** | Wii U system controls; no ScummVM action assigned |

For a right-click while using touch, press **B**. In launcher dialogs, **A**
clicks and **Y** closes/cancels. **B is not a universal Back button.** To exit
ScummVM normally, choose the launcher's **Quit** button.

### Wii Remote + Nunchuk

Pair the Remote with the Wii U before launching. For IR pointing, use a powered
Sensor Bar and keep the Remote aimed at it with a clear line of sight. A Remote
without a Nunchuk can still point, click, open menus and use its own buttons.
Attach a Nunchuk for analog movement, dialogue skip and precision mode.

| Input | Default action | GamePad equivalent |
| --- | --- | --- |
| Aim Wii Remote at Sensor Bar | Position/move the IR pointer directly | Touchscreen positioning |
| **A** | Left mouse click; hold while pointing/moving to drag | A |
| **B** (rear trigger) | Right mouse click | B |
| **D-pad ↑ ↓ ← →** | Navigate launcher widgets; game-specific directional controls | D-pad |
| **1** | Game menu / F5 where supported | L |
| **2** | Escape: skip/cancel/back where supported; close launcher dialogs | Y |
| **+** | ScummVM global main menu | + |
| **−** | Open the native on-screen keyboard | − |
| Nunchuk analog stick | Move the pointer | Left analog stick |
| Nunchuk **Z** | Skip the current dialogue line where supported | X |
| Hold Nunchuk **C** | Slow the Nunchuk analog pointer for precise movement | Hold R |
| **HOME** | Wii U system HOME interface / application lifecycle | HOME |
| **POWER** | Wii U system control; no ScummVM action assigned | POWER |

**C slows analog movement, not absolute IR pointing.** If you prefer the stick,
point the Remote away from the Sensor Bar; touch/stick input can take over the
pointer. Reacquiring IR resumes pointing. GamePad and Remote can remain
connected together, and the Nunchuk is optional for IR mouse control.

### Game-specific controls and remapping

In **World of Xeen**, the D-pad moves/turns, **GamePad X / Nunchuk Z** sends
Enter, and **GamePad Y / Remote 2** sends Escape. Escape at the title screen
can quit; use X/Z or click **Start Game** with A/touch/IR instead.

Use the **Keymaps** settings in Global Options or a game's options to inspect
and change supported bindings. Remote **1**, **2**, and Nunchuk **Z/C** share
the logical GamePad **L**, **Y**, **X/R** inputs respectively, so remapping one
also affects its equivalent. Remote **A/B** are delivered as mouse clicks;
changing joystick A/B bindings does not change those physical mouse buttons.
Some engines reserve or override inputs. The GamePad-only inputs marked with
no general default above can already have an action in a particular engine.

## Native on-screen keyboard

The native Wii U keyboard opens when a supported field requests text input.
If a game's own input field does not open it automatically, select that field
and press **−** on the GamePad or Wii Remote. GamePad touch input is supported. This version also improves
native input routing for Pro/Classic/Remote controllers and a GamePad connected
later; I still need hardware confirmation of the reported Pro-only/HOME case.

- **OK** submits the text and dismisses the keyboard. It does **not** press
  Enter or activate the game's Save button.
- Wait for all characters to appear, then choose **Save** in the game dialog.
  Text is delivered gradually to accommodate older SCUMM input routines.
- **Cancel** dismisses only the keyboard, without inserting text or sending
  Escape to the underlying dialog.
- Supported ScummVM fields and original SCUMM save dialogs pass their existing
  text to the keyboard. Confirming edits replaces the changed text rather than
  simply appending; Cancel or an unchanged submission leaves it alone. I tested
  editing successfully in Sam & Max. Arbitrary fields in other engines may
  still lack this context and use insertion-only behavior.
- Original game dialogs may restrict name length and supported characters.
  Plain letters and numbers are recommended for the first save test.

## MP3 audio playback

MP3 support remains included in 0.7.1. Earlier SCUMM-only packages did not
include the MP3 decoder.

MP3 decoding is automatic when a supported game engine reads audio in a
supported MP3-based format. **There is no MP3 switch to enable**, and the
**Music device** setting does not select the MP3 decoder.

1. Keep the game's audio files in the locations and with the filenames required
   by that game's ScummVM data-file instructions. Some engines use compressed
   audio containers rather than ordinary `.mp3` files.
2. Add the game normally and start it. If it already has compatible MP3 audio,
   no conversion or additional setup is needed.
3. If you want to compress original audio, back up the game files and use the
   appropriate tool from [ScummVM Tools](https://github.com/scummvm/scummvm-tools),
   selecting MP3 output where supported. Follow that tool's encoder requirements
   and game-specific instructions. Conversion is optional and is done on a
   computer, not by the Wii U port.

**Renaming an audio file's extension does not convert it to MP3.** Do not replace
game files with arbitrary MP3s: supported formats, names and layouts depend on
the engine. Uncompressed audio does not need to be converted just to play.

To verify decoding, use a separate test copy with the intended MP3 assets and
keep equivalent uncompressed originals outside that copy, following the game's
file requirements. Otherwise an engine may prefer the original audio. Check
the relevant speech, music or effects; hearing a different audio source does
not confirm MP3 playback. Keep the originals as a backup.

## Ogg Vorbis audio playback

Version 0.7.1 includes **Ogg Vorbis** decoding. It is automatic when an
included engine reads a supported Vorbis resource; there is no Ogg toggle.

1. Keep the game's required filenames and directory layout. Some engines use
   ordinary `.ogg` tracks; others use compressed speech containers such as
   `.sog`. An `.ogg` extension alone does not identify a supported codec.
2. Add and launch the game normally. Existing compatible Vorbis resources do
   not need conversion to MP3.
3. To compress original audio, keep a backup and use the appropriate ScummVM
   Tools utility with Vorbis output where supported. Do not rename `.sou`,
   `.so3`, `.mp3` or other files to `.ogg`/`.sog`: renaming is not conversion.
4. For a decoding test, use a separate game copy with the intended compressed
   resources and keep alternative originals outside it, following that game's
   data requirements. Otherwise the engine may prefer the original audio.

This adds Vorbis, not Opus or Theora. FLAC and MPEG-2 are also not enabled.
Host decoding checks passed; I have not yet recorded a confirmed Wii U test
using specifically identified Vorbis assets.

## MT-32 music emulation

MT-32 emulation reproduces a Roland music module for games with compatible
music. It is optional, does not improve every game's audio, and does not replace
recorded speech or CD/MP3 soundtracks. Use a build with MT-32 support included.

The tested MT-32 setup requires **both** files, named exactly:

```text
MT32_CONTROL.ROM
MT32_PCM.ROM
```

These are the MT-32's ROM data, not game files or a SoundFont. They are **not
included** with this port or ScummVM; you must supply your own matching pair.

1. Copy both ROM files to `SD:/scummvm/data/`, the port's default **Extra path**.
2. If you previously changed that setting, open **Global Options → Paths** and
   set **Extra path** to `/vol/external01/scummvm/data`, or use the directory
   where you placed the ROMs. Check that the game's own path settings do not
   override it with a different directory.
3. Select a compatible game in the launcher and open **Game Options → Audio**.
4. Enable **Override global audio settings**, then select **MT-32 emulator**
   as the **Music device**. Configure it per game first, rather than changing
   the global default for every game.
5. Save the settings and start the game. Restart the game after changing its
   music device.

You do **not** need to enable **True Roland MT-32** manually when using the
emulator. Selecting that checkbox alone is not a substitute for selecting
**MT-32 emulator** as the music device.

If ScummVM reports missing or invalid ROMs, check both filenames, the matching
ROM pair and the Extra path. If music stutters, compare with another device
supported by the game; MT-32 emulation requires more processing power.

See the [official audio guide](https://docs.scummvm.org/en/latest/advanced_topics/understand_audio.html)
for further background. The instructions above describe the MT-32 ROM pair
I used successfully in my tests.

## Hardware testing

I successfully tested the following games on my Wii U:

| Game | My test results |
| --- | --- |
| Sam & Max Hit the Road | Gameplay, saving, audio and controls |
| Indiana Jones and the Last Crusade | Gameplay, saving, audio and controls |
| Indiana Jones and the Fate of Atlantis | Repeated launch/return with the renderer fix |
| Riven: The Sequel to Myst | Ran without problems in my tests |
| Blade Runner | Ran without problems in my tests |
| The 11th Hour | Ran without problems in my tests |
| The Curse of Monkey Island | Gameplay and all audio; scene/menu delays resolved in my tests |
| Might and Magic: World of Xeen | Playable; game-specific keyboard mappings apply |
| Quest for Glory IV | DOS and Windows editions opened and were playable; startup greatly improved |

I also tested the native keyboard successfully on September 26, 2026.
My earlier SDK tests covered TV/GamePad video, touch,
buttons, audio, SD access and HOME background/return behavior.

On October 1, 2026, I successfully tested both MP3 decoding and MT-32 emulation
on my Wii U using the expanded build. For MT-32, I used
`MT32_CONTROL.ROM` and `MT32_PCM.ROM`. This confirms those tested audio
scenarios, not every game or edition.

On October 3, I confirmed that the renderer memory fix resolved the
repeated-exit freeze. My log shows two Fate of Atlantis and two Sam & Max
launch/return cycles, keyboard use and shutdown cleanup ending with successful
log closure. I also tested Riven, Blade Runner and The 11th Hour without
problems. That renderer fix remains included.

Since 0.6.0 I also tested Curse, World of Xeen and QFG4. The timer fix restored
Curse audio, and the IO changes removed the noticeable Curse/QFG4 delays in my
tests. Existing-text editing worked in Sam & Max. On October 18, my 11th Hour
movie/audio test looked and sounded good with the RGBA conversion optimization.
The diagnostic log shows roughly 29–31 ms drawing per frame instead of about
48 ms, two game launches and normal shutdown. It still records late frames
and some empty audio-queue observations, so I do not claim constant 30 fps or
perfect playback across all Groovie games.

The game-specific results above refer to earlier builds. On October 22, 2026,
I reported that the ScummVM 2026.4.0 update test was a success, after the earlier
Remote and button-remapping tests. That report identifies the predecessor
binary `90b6e646`; I supplied no new log or per-game/scenario breakdown.
Release 0.7.1 retains that runtime implementation and SDK, changing the port
version metadata and documentation. Its newly packaged binary has not had a
separate console run. Detailed diagnostic tracing remains disabled.

These are tests of specific scenarios, **not full-game completion reports or
a guarantee that every game/version works**. Additional community testing is
welcome. I also confirmed that the ScummVM menu icon worked on September 28,
2026. **I have not tested the separate RPX package outside Aroma.** Please do
not describe it as confirmed Tiramisu or legacy support yet.

## Current limitations

- The expanded build targets **Myst (original / Masterpiece), Riven (original),
  Blade Runner (original), The 7th Guest and The 11th Hour**. Their engines
  are included. I tested Riven, Blade Runner and The 11th Hour successfully;
  I have not yet tested Myst/Myst Masterpiece or The 7th Guest on my Wii U.
  Modern remakes and enhanced/remastered editions are not promised.
- SCUMM v7/v8, MM/Xeen and SCI32 are now enabled, but HE/MM1 and other unlisted
  engines are not. The complete desktop compatibility list is not a Wii U guarantee.
- MP3 decoding and MT-32 have passed my console tests, but
  compatibility/performance across all games is not guaranteed.
  Vorbis is included but still needs a specifically identified console test.
  FLAC, MPEG-2, Opus and Theora are not included.
- Clandestiny, Tender Loving Care and Uncle Henry's Playhouse community reports
  remain unresolved/unverified; the 11th Hour result does not validate them.
- Version 0.7.1 retains the renderer memory fix validated for the reported
  repeated-exit freeze. This does not guarantee that every game/edition or
  possible freeze is resolved. Back up saves and report new failures.
- Networking and OpenGL/shaders are not enabled.
- Other controllers and untested games or game editions may behave differently.
  Pro Controller keyboard/HOME changes are included but not hardware-confirmed.
- Native keyboard input follows the receiving game's character and length
  restrictions; existing-text context is not implemented for every engine.

## Release and historical test builds

Read the [0.7.1 release notes](backends/platform/sdl/wiiu/RELEASE-0.7.1.md)
before replacing your existing executable. Keep the earlier ZIP for rollback.
The [experimental build guide](backends/platform/sdl/wiiu/EXPERIMENTAL.md)
documents the historical September 30 profile and dependency preparation.

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

For release 0.7.1, including the expanded engines, MP3/Vorbis and MT-32 without
diagnostic tracing, use the release script after preparing the dependencies
and all seven SDL patches listed in the
[backend notes](backends/platform/sdl/wiiu/README.WIIU):

```sh
bash backends/platform/sdl/wiiu/build-release.sh
```

The following is the historical **minimal SCUMM-only profile**, with MT-32
explicitly disabled; it is not the expanded release profile. With a compatible
Wii U SDK prepared and its environment activated, build outside the source tree:

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
requirements and patch order, including keyboard lifecycle/controller routing,
renderer cleanup, timer zero-timeout handling, RGBA conversion and Wii Remote
IR pointer input. Rebuild/install
SDL after applying them, then relink ScummVM. An arbitrary
unpatched SDL/WUT installation is not equivalent to the locally tested SDK.

## Credits and license

Based on [ScummVM](https://www.scummvm.org/), with the work of the ScummVM
contributors, [SDL](https://www.libsdl.org/), [devkitPro](https://devkitpro.org/),
[WUT](https://github.com/devkitPro/wut) and the
[Aroma community](https://github.com/wiiu-env).

I'm [Rrhapsod](https://github.com/Rrhapsod), and I maintain this fork.
The ScummVM name and icon are credited to the upstream project; their use here
does not imply endorsement or official support.

ScummVM is free software under the GNU General Public License, version 3 or
later. See [COPYING](COPYING), [COPYRIGHT](COPYRIGHT) and [AUTHORS](AUTHORS).
Dependencies and bundled assets retain their respective licenses and credits.
The original project documentation is preserved in
[README.scummvm.md](README.scummvm.md).
