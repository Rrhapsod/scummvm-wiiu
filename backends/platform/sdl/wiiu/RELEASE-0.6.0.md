# ScummVM for Wii U v0.6.0

An unofficial native Wii U port for Aroma. No RetroArch required.
**0.6.0 is the Wii U port version**, not the upstream ScummVM version.

## Highlights

- Fixes a graphics-memory leak when switching between games and the launcher.
  The private SDL window texture is now released correctly, and allocation
  failures are handled safely. I confirmed that this resolved the repeated
  launch/exit freeze on my Wii U.
- Expanded game engines: SCUMM v0–v6, Mohawk (Myst/Myst Masterpiece/Riven),
  Blade Runner, and Groovie/Groovie2 (The 7th Guest/The 11th Hour).
- I tested Riven, Blade Runner and The 11th Hour on my Wii U, and all three
  ran without problems.
- MP3 decoding and MT-32 emulation, both tested on my Wii U.
- GamePad touchscreen control, analog pointer control, native Wii U keyboard,
  TV/GamePad video, SD browsing, settings and saves.
- Detailed lifecycle diagnostics are disabled in these release binaries.
  Normal ScummVM logging remains available.

## Downloads and installation

- **ScummVM-wiiu-0.6.0-aroma.zip** — recommended for Aroma. Extract to the SD
  root and launch ScummVM from the Wii U Menu.
- **ScummVM-wiiu-0.6.0-rpx-experimental.zip** — for community testing with an
  existing non-Aroma Homebrew Launcher/loader supporting RPX. Compatibility
  outside Aroma is **not yet verified**; this is not a WUP channel installer.
- **ScummVM-wiiu-0.6.0-source.tar.gz** — source and local SDL patches, not an
  installation package.
- **SHA256SUMS.txt** — checksums for the release archives.

Choose one installation ZIP. Back up your current executable, configuration
and saves first. Merge folders; do not delete your existing `scummvm` directory.
The Aroma executable is `SD:/wiiu/apps/ScummVM.wuhb`; RPX goes in
`SD:/wiiu/apps/scummvm/ScummVM.rpx`. Keep the bundled `scummvm/data` files.
Games and Roland ROMs are not included. GitHub's automatic source downloads
are also not installation packages.

## GamePad controls

Defaults for the tested SCUMM games; other engines or custom mappings can differ.

| Input | Action |
| --- | --- |
| Touchscreen / short tap | Position pointer / left click |
| Left analog stick | Move pointer |
| A / B | Left click / right click |
| Hold R | Slow pointer for precision |
| L | Game menu / F5 where supported |
| + | ScummVM global menu |
| Minus (−) | Open native on-screen keyboard |
| X / Y | Skip dialogue / Escape where supported |

In launcher dialogs, A clicks and Y closes/cancels; B is not a universal Back
button. For touch plus right-click, use B. Keyboard OK inserts text but does
not press Save: wait for the text to appear, then select Save. Keyboard Cancel
dismisses the keyboard without cancelling the underlying game dialog.

## Audio setup

**MP3:** decoding is automatic for supported game audio formats. There is no
MP3 toggle. Keep the filenames/layout required by the game's data instructions;
renaming files to `.mp3` does not convert them. Compression with the appropriate
ScummVM Tools utility is optional and should be done on a backup copy.

**MT-32:** supply your own matching `MT32_CONTROL.ROM` and `MT32_PCM.ROM` in
`SD:/scummvm/data/`. Ensure Global Options → Paths → Extra path points to
`/vol/external01/scummvm/data` (or your chosen ROM directory). For a compatible
game, open Game Options → Audio, override global audio settings, select
**MT-32 emulator**, and restart the game. It is optional and does not replace
recorded speech or CD audio. See the bundled README for the full generic guide.

## Testing and limitations

- I previously tested gameplay, saving, audio and controls in Sam & Max Hit
  the Road and Indiana Jones and the Last Crusade on my Wii U.
- With the corrected diagnostic build, I completed two Fate of Atlantis and
  two Sam & Max launch/return cycles, used the keyboard and exited normally.
  I confirmed that the repeated-exit freeze was resolved.
- I also tested Riven, Blade Runner and The 11th Hour without problems.
  These are not full-game completion or every-edition compatibility claims.
- MP3, MT-32 and the native keyboard have also passed my hardware tests.
- Myst/Myst Masterpiece and The 7th Guest are included, but I have not yet
  tested them on my Wii U.
- SCUMM v7/v8/HE and other unlisted engines are not included. Neither are
  Vorbis, FLAC or MPEG-2; modern remakes/enhanced editions are not promised.
- Other controllers and non-Aroma environments remain unverified.

My tests above used the corrected diagnostic build and earlier builds.
The final 0.6.0 binaries use the same corrected SDL and engine/audio
profile with lifecycle tracing disabled; I still need to smoke-test these
final binaries on my console before publication. Source cross-compilation and host regression
tests are separate from hardware validation.

Report issues at https://github.com/Rrhapsod/scummvm-wiiu/issues with the game
edition, environment, reproduction steps and `SD:/scummvm/scummvm.log`.
Copy the log before relaunching ScummVM, as it is overwritten.

## Credits

Based on ScummVM, SDL2, devkitPro and WUT. I'm Rrhapsod, and I maintain this Wii U port.
This is a community release, not an official ScummVM project release.
See the bundled copyright and license notices. SDL patches ship with the source
and package documentation; ScummVM is licensed under GPL-3.0-or-later.
