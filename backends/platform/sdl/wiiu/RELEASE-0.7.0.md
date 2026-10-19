# ScummVM for Wii U v0.7.0

An unofficial native Wii U port for Aroma. **No RetroArch required.**
0.7.0 is my Wii U port version, separate from the upstream ScummVM version.

## What's new since 0.6.0

- **More engines:** SCUMM v7/v8, MM/Xeen and SCI/SCI32 are now included. I tested
  The Curse of Monkey Island, World of Xeen, and Quest for Glory IV (DOS and
  Windows editions). Previously included SCUMM v0–v6, Myst/Riven, Blade Runner
  and Groovie engines remain available.
- **Curse audio and responsiveness:** an SDL zero-timeout fix restores timer
  progress; sequential bundle-table reads remove unnecessary seeks. Music,
  voices and effects work in my tests, without the earlier scene/menu delays.
- **Much faster QFG4 loading:** cached read-only resource-volume sizes avoid
  repeated SD queries. My diagnostic resource-init measurement dropped from
  about 156 seconds to 5 seconds; this is not total application startup time.
- **Improved Groovie movies:** buffered resource reads and faster RGBA pixel
  conversion reduce rendering cost. My latest 11th Hour test looked and
  sounded good. The log shows drawing around 29–31 ms/frame instead of about
  48 ms, though some frames remain late; this is not a constant-30-fps claim.
- **Ogg Vorbis decoding:** compatible Vorbis game audio no longer needs to be
  converted to MP3. MP3 decoding and MT-32 emulation remain included.
- **Edit existing text:** supported ScummVM fields and original SCUMM save
  dialogs prefill the native keyboard. Confirm edits the existing text rather
  than appending. I successfully tested this with Sam & Max.
- **Controller keyboard handling:** input routing now accounts for Pro/Classic/
  Remote controllers and a GamePad connected later, with native keyboard
  cleanup before HOME foreground release. I still need hardware confirmation
  of the reported Pro-only keyboard/HOME scenario.
- The renderer-memory fix for repeated game/launcher transitions is retained.
  Detailed diagnostic tracing is disabled in this release; normal logging remains.

## Downloads and installation

- **ScummVM-wiiu-0.7.0-aroma.zip** — recommended for Aroma. Extract the contents
  to the SD root and launch **ScummVM** from the Wii U Menu.
- **ScummVM-wiiu-0.7.0-rpx-experimental.zip** — for community testing with an
  existing non-Aroma Homebrew Launcher/loader supporting RPX. Compatibility
  outside Aroma is **not verified**. This is not a WUP/channel installer.
- **SHA256SUMS.txt** — checksums for the downloadable packages.

Choose one installation ZIP. Back up the executable, configuration and saves.
Merge folders; do not delete the existing `scummvm` directory. Keep the bundled
`scummvm/data` files, especially the updated Xeen `mm.dat`.

Aroma uses `SD:/wiiu/apps/ScummVM.wuhb`; RPX uses
`SD:/wiiu/apps/scummvm/ScummVM.rpx`. The name and icon remain **ScummVM**.
Games and MT-32 ROMs are not included. Source archives are not SD installation
packages; the release source includes the local SDL patches and build helpers.

## Touch and GamePad controls

Defaults for the tested SCUMM adventures; other engines/remappings can differ.

| Input | Action |
| --- | --- |
| Touchscreen / short tap | Position pointer / left click |
| Left analog stick | Move pointer |
| A / B | Left click / right click |
| Hold R | Slow pointer for precise movement |
| L | Game menu / F5 where supported |
| + | ScummVM global menu |
| Minus (−) | Open the native keyboard |
| X / Y | Skip dialogue / Escape where supported |

In launcher dialogs, A clicks and Y closes/cancels; B is not a universal Back
button. Use B for a right-click when playing with touch. Keyboard OK submits
the edit but does not press Save: wait for the text, then select Save. Cancel
closes only the keyboard. Text length/character limits still depend on the game.

In World of Xeen, the D-pad moves/turns, X sends Enter and Y sends Escape.
Escape at the title can quit; use X or click Start Game with A/touch.

## Generic audio setup

**MP3 and Ogg Vorbis:** decoding is automatic for supported engine resources.
Keep the game's required names and folder layout. Some engines use compressed
speech containers rather than standalone music tracks. Renaming extensions
does not convert audio. Optional compression should use the appropriate
ScummVM Tools utility on a backup copy; existing compatible Ogg resources do
not need conversion to MP3. Vorbis support does not include Opus or Theora.

**MT-32:** I use a matching pair of `MT32_CONTROL.ROM` and `MT32_PCM.ROM` in
`SD:/scummvm/data/`, or another directory selected as **Extra path**. For a
compatible game, open **Game Options → Audio**, override global audio settings,
choose **MT-32 emulator**, then restart the game. Global Options → Paths →
Extra path defaults to `/vol/external01/scummvm/data`. MT-32 is optional and
does not replace recorded speech or CD audio. See the README for the full guide.

## Testing and remaining limitations

I tested changes in diagnostic candidates on Aroma, including Sam & Max,
Indiana Jones and the Last Crusade, Fate of Atlantis launch/return cycles,
Riven, Blade Runner, Curse, World of Xeen, QFG4 and The 11th Hour. MP3, MT-32
and supported save-name editing passed my hardware tests. These are specific
scenarios, not full playthroughs or every-edition compatibility claims.

- My improved 11th Hour result does not resolve the unverified Clandestiny,
  Tender Loving Care or Uncle Henry's Playhouse community reports. Some late
  frames/empty audio-queue observations remain in the diagnostic log.
- Vorbis passed host decoding checks, but I have not recorded a console test
  with specifically identified Vorbis assets. Pro-only keyboard/HOME and
  non-Aroma RPX still need targeted hardware testing.
- Myst/Myst Masterpiece and The 7th Guest engines are included; I have not yet
  recorded representative console results for them.
- HE/MM1 and other unlisted engines, FLAC, MPEG-2, Opus, Theora, networking and
  OpenGL/shaders are not enabled. Modern remakes are not promised.
- Existing-text editing is not integrated with every engine's custom fields.

The final 0.7.0 binaries keep the tested changes with diagnostics disabled.
I still need a short smoke test of these exact release packages before
publication: movie/audio, menu return/reopen, save-name editing and Quit.

Report issues at https://github.com/Rrhapsod/scummvm-wiiu/issues with the game
edition, package/environment, reproduction steps and `SD:/scummvm/scummvm.log`.
Copy that log before relaunching: the normal release log is overwritten.

## Credits

Based on ScummVM, SDL2, devkitPro, WUT and the Aroma community. I'm Rrhapsod,
and I maintain this unofficial Wii U port. See the bundled licenses and credits.
