# ScummVM Wii U — 0.8.0 engine candidate, October 30, 2026

I requested one combined build to test the next group of engines before a
0.8.0 release. This candidate keeps the upstream **2026.4.0** base, the complete
0.7.1 engine/codec profile, GamePad touch, native keyboard, Wii Remote IR and
Nunchuk controls. **Inclusion is not a Wii U compatibility result.**

## Requested games in this build

| Engine in this build | Game / ScummVM ID | Wii U status |
| --- | --- | --- |
| Tinsel | Discworld — `tinsel:dw` | Pending my test |
| Tinsel | Discworld II: Missing Presumed...!? — `tinsel:dw2` | Pending my test |
| Sword1 | Broken Sword: The Shadow of the Templars — `sword1:sword1` | My Oct 27 predecessor test passed (GOG/Windows/English); new build pending |
| Sword2 | Broken Sword II: The Smoking Mirror — `sword2:sword2` | Pending my test |
| AGOS | Elvira: Mistress of the Dark — `agos:elvira1` | Pending my test |
| AGOS | Elvira II: The Jaws of Cerberus — `agos:elvira2` | Pending my test |
| AGOS | Simon the Sorcerer — `agos:simon1` | My Oct 27 predecessor test passed (CD/Windows/English); new build pending |
| AGOS | Simon the Sorcerer II — `agos:simon2` | Pending my test |
| Gob | Gobliiins — `gob:gob1` | Pending my test |
| Gob | Gobliins 2: The Prince Buffoon — `gob:gob2` | Pending my test |
| Gob | Goblins Quest 3 — `gob:gob3` | My Oct 27 predecessor test passed (CD v1.02/DOS/English); new build pending |
| Gob | The Bizarre Adventures of Woodruff and the Schnibble — `gob:woodruff` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 1 — `kyra:kyra1` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 2: Hand of Fate — `kyra:kyra2` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 3: Malcolm's Revenge — `kyra:kyra3` | My 7cd5beaa test worked perfectly (DOS/English); same IO implementation retained; new binary pending |
| Kyra/EOB | Eye of the Beholder — `kyra:eob` | Pending my test |
| Kyra/EOB | Eye of the Beholder II: The Legend of Darkmoon — `kyra:eob2` | Pending my test |
| Darkseed | Dark Seed — `darkseed:darkseed` | Pending my test |
| SCUMM v7/v8 | Star Wars: Rebel Assault — `scumm:rebel1` | Pending my test |
| SCUMM v7/v8 | Star Wars: Rebel Assault II: The Hidden Empire — `scumm:rebel2` | Pending my test |
| Sherlock | The Lost Files of Sherlock Holmes: The Case of the Serrated Scalpel — `sherlock:scalpel` | Pending my test |
| Sherlock | The Lost Files of Sherlock Holmes: The Case of the Rose Tattoo — `sherlock:rosetattoo` | Pending my test |
| SAGA/IHNM | I Have No Mouth, and I Must Scream — `saga:ihnm` | My 8d242742 test worked perfectly (DOS/English); input fix retained; new binary pending |

**Eye of the Beholder III is not implemented in this ScummVM base.** Enabling
Kyra/EOB supplies I and II only. It is excluded rather than advertised as
working. The [upstream 2026.4.0 compatibility list](https://www.scummvm.org/compatibility/2026.4.0/)
and this checkout's Kyra detection table list I/II, not III.

Rebel Assault uses SCUMM v7/v8, already enabled in 0.7.1; there is no separate
`rebel` configure engine. Both Sherlock titles use the single `sherlock`
engine. AGOS2 is unnecessary for the four requested AGOS titles. Extra
subengines HE, MM1, AGOS2, Lands of Lore and Rebel Assault II PlayStation remain
disabled. TinyGL remains off: this candidate targets Discworld 1/2, not Noir.
Use the original PC versions for the first tests; remakes/remasters and the
Rebel Assault II PlayStation version are outside this candidate's scope.

## Installation and required data

Use **ScummVM-wiiu-0.8.0-candidate-20261030-aroma.zip** with Aroma. Extract the
complete ZIP to the SD root, merge folders and replace `wiiu/apps/ScummVM.wuhb`.
Keep the previous 0.7.1 executable/package and preserve configuration, saves
and game folders. No games or MT-32 ROMs are included.

Keep all of `scummvm/data`, especially **kyra.dat**, **darkseed.dat** and the
source-generated **mm.dat**. These are bundled ScummVM engine resources, not
commercial game files. The default Extra path is `/vol/external01/scummvm/data`;
check a custom game/global Extra path if an engine reports missing resources.

Broken Sword supports original Smacker (`.smk`) and DXA (`.dxa`) cutscenes with
the existing decoders/zlib. Old MPEG-2 (`.mp2`) replacement videos still require
MPEG-2 support, which this SDK/profile does not include. Supply the appropriate
original or supported cutscene files; renaming files does not convert them.
MP3, Ogg Vorbis, JPEG and MT-32 emulation remain enabled. Other codecs/editions
may have their own requirements.

A separate RPX ZIP is generated from the same executable for loader testing;
non-Aroma compatibility is unverified. Choose one installation layout.

## Controls and joint test checklist

The bundled README retains the complete physical GamePad and Remote + Nunchuk
tables. Standard adventure bindings remain **2 = Y/Escape**, **C = R/slow
analog pointer**, 1 = L, Z = X, Plus = menu, Minus = keyboard. Engines may use
different keys/actions: inspect their Keymaps rather than assuming F5/Escape
or dialogue skip is universal. Native keyboard follows its own prompts.

Rebel Assault has dedicated action/aim/gamepad keymaps; Escape/Y and Plus can
have game-specific menu/cover behavior. Test GamePad/stick and Remote IR input
separately, including firing and release after switching input. For EOB, test
D-pad movement/turning, mouse inventory/combat, party generation and saving;
0.7.1's Xeen X/Z Enter bindings do not automatically apply to EOB.

For each listed game, record edition/platform/language, then:

1. Add Game detection, engine ID and startup.
2. Intro/movie and music/speech/effects, where present.
3. Pointer/left-right clicks, GamePad controls, Remote/Nunchuk controls and
   game-specific keyboard input. Check Minus and existing save-name editing.
4. A new save and load (use a spare slot), menu return and a second launch.
5. HOME/return and clean Quit after the basics work.

Copy `SD:/scummvm/scummvm.log` immediately after a failure; the next launch
replaces it. Include the candidate version/revision, exact scene and input used.
Use the included TEST-RESULTS.md table for a combined report. Also reopen one
previously working 0.7.1 game to check for regressions.

## Wii Remote pointer precision

My new report describes a grid-like cursor when moving the Remote slowly.
The previous SDL helper discarded small movements until they accumulated to
4 display pixels at 720p or 6 at 1080p, including while dragging. This
candidate reports every changed display pixel while the Remote owns the
cursor. The acquisition threshold after touch/stick use remains to prevent
idle IR noise from taking over. Identical pixel coordinates remain silent;
there is no extra averaging delay. Game cursor resolution still applies.

1. Move slowly horizontally, vertically and diagonally over the launcher,
   including small buttons, then repeat in a previously working game.
2. Hold A to drag slowly. Check B/right-click and release both buttons after
   moving off the Sensor Bar and after disconnecting the Remote.
3. Leave the Remote still, use touch and both GamePad/Nunchuk sticks, then
   deliberately point again. Check that idle IR does not pull the cursor back.
4. Open/cancel the native keyboard with Minus; test HOME/return and Quit.

Compare using the same display mode, distance and Sensor Bar placement.
Record 480p/720p/1080p, Remote model, any stationary jitter and the exact
candidate revision. Host mock ramps show one-pixel steps; smoothness with real
IR and a physical console still needs my test. See [POINTER-SDK.md](POINTER-SDK.md).

## I Have No Mouth: confirmed predecessor input fix

My October 29 session identifies **8d242742**, and I reported IHNM worked
perfectly. The log records an accepted opening skip, character-selection and
chapter clicks, then clean shutdown. This is a successful scenario report,
not a full playthrough. The SAGA implementation is unchanged in this candidate.

- **Y / Remote 2:** skip the opening/cutaway. The initial logo/title screens
  also accept A/B or a touch/IR click in the Wii U build.
- **X / Nunchuk Z:** skip speech; **L / Remote 1:** original options panel
  where allowed. Plus retains the global menu and Minus the native keyboard.
- **A/B / Remote A/B / touch/IR:** mouse controls in character selection and
  gameplay. The cursor remains deliberately hidden during videos.

The October 29 fix added the missing controller bindings, kept the skip map
active during cutaways and selected panel/text maps before polling, including
the first input. The separate logo/title poll now accepts mapped actions.
Keyboard bindings/IDs, exclusive maps and text entry remain. Actual-code host
ASan/UBSan tests cover this routing with mocked event transport. Bounded SAGA
input/cursor diagnostics remain enabled; copy scummvm.log after a failure.

## Kyrandia 3 loading

I reported playable Malcolm's Revenge with roughly four-second stalls on scene
changes in binary `67e2ae12`. The supplied October 27 log identifies the game
and normal teardown, but contains no scene/IO timings. The new Wii U AUD input
buffer targets the many tiny seeks in the audio-duration scan. It uses 64 KiB
per active AUD stream; no full speech archive is preloaded and game pacing,
script waits and music fades are unchanged.

Host tests of the actual decoder/Common buffer preserve PCM samples, duration,
seek/rewind behavior and ownership. With 2,000 synthetic 2,048-byte chunks, the
initial scan changes from 4,004 reads/2,001 seeks to 63 reads/63 seeks. These
are operation counts, not measured SD speed or proof of fixing the reported
stalls. This candidate logs up to 24 AUD initializations and 24 Kyra3 scene
transitions, including the explicit fade wait separately from scripts/entry.
My subsequent test of `7cd5beaa` worked perfectly. Its log records large AUD
initializations at 487/499 ms and three scene transitions; total scene duration
also includes entry animations/walking, so it is not a disk-stall measurement.
This candidate retains the exact Kyra/USB implementations from that binary.

## USB games and saves (first hardware candidate)

Use **one FAT32 drive**, with 512-byte logical sectors and an MBR partition
layout, connected before launching ScummVM. A FAT32 volume without a partition
table is also covered by image tests. NTFS, exFAT, GPT, Wii U formatted/encrypted
drives, multiple USB drives and live removal are outside this candidate.
**Do not reformat a drive containing data just to try this build.** If the Wii U
Menu offers to format this PC-readable FAT32 drive, cancel that prompt.

1. On a computer, copy a game's own files into a folder such as
   `USB:/scummvm/games/ihnm/`. Create `USB:/scummvm/saves/` if you want USB saves.
2. Keep the full installation ZIP on SD. The WUHB, configuration, log and bundled
   engine resources still use SD. ScummVM mounts only the USB FAT filesystem;
   it does not format storage or move any existing files/saves.
3. Choose **Add Game**, go up to the drive list and open **usb:**. Select the
   game folder (`usb:/scummvm/games/ihnm/`). SD remains `/vol/external01/`.
4. Saves stay on SD by default. For USB saves choose the existing USB saves
   folder under **Global Options → Paths → Save Path**, or the individual
   game's **Game Options → Paths → Save Path**. A per-game path overrides the
   global setting. Copy old saves yourself if you want them available there.
5. Create a spare save, load it, overwrite that spare slot, then Quit cleanly.
   Relaunch and load it again. Check that the files are present on USB from
   the computer. Leave the drive connected until ScummVM has exited.

If `usb:` does not appear, check `SD:/scummvm/scummvm.log` for `Wii U USB:`.
The log distinguishes unavailable Mocha API, an occupied device name, failure
to mount FAT and a mounted drive. No USB is a supported SD-only startup case.
A saved USB path with no drive present should report an unavailable path;
it does not silently move saves to SD. Hardware/bridge/power compatibility and
save durability still require physical-console tests. Use a drive with a
suitable power supply; changing the format cannot fix an unstable connection.

Record these USB checks in TEST-RESULTS.md, together with drive model, partition
layout and whether games and saves were on SD or USB. Also test HOME/return,
menu/reopen, SD-only startup and one previously working SD game.

## Build and evidence

Pin the independent pointer SDK profile in [POINTER-SDK.md](POINTER-SDK.md),
which retains the USB dependencies described in [USB-SDK.md](USB-SDK.md), then
run `build-0.8.0-test.sh`. It uses a fresh October 30 directory and restores
Xeen data after staging. No additional engine-data archive is needed for IHNM.

`WIIU_ENGINES_TEST` identifies this build. Bounded Kyra loading and SAGA input/
cursor diagnostics remain enabled; detailed renderer/audio/IR tracing is off.
The r51 compiler, WUT, codecs and USB libraries are retained; SDL adds only
incremental patch 8 to the existing seven patches. The shared SDK default and
older binaries are unchanged. BUILD-INFO and SHA256SUMS record provenance.
My Simon 1/Sword1/Gob3 reports apply to `67e2ae12`, Kyra3 success to `7cd5beaa`
and IHNM success to `8d242742`. This new pointer binary and physical USB remain
untested; final 0.8.0 still needs the broader requested game tests.
