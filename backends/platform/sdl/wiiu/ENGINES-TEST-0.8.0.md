# ScummVM Wii U — 0.8.0 engine candidate, October 26, 2026

I requested one combined build to test the next group of engines before a
0.8.0 release. This candidate keeps the upstream **2026.4.0** base, the complete
0.7.1 engine/codec profile, GamePad touch, native keyboard, Wii Remote IR and
Nunchuk controls. **Inclusion is not a Wii U compatibility result.**

## Requested games in this build

| Engine in this build | Game / ScummVM ID | Wii U status |
| --- | --- | --- |
| Tinsel | Discworld — `tinsel:dw` | Pending my test |
| Tinsel | Discworld II: Missing Presumed...!? — `tinsel:dw2` | Pending my test |
| Sword1 | Broken Sword: The Shadow of the Templars — `sword1:sword1` | Pending my test |
| Sword2 | Broken Sword II: The Smoking Mirror — `sword2:sword2` | Pending my test |
| AGOS | Elvira: Mistress of the Dark — `agos:elvira1` | Pending my test |
| AGOS | Elvira II: The Jaws of Cerberus — `agos:elvira2` | Pending my test |
| AGOS | Simon the Sorcerer — `agos:simon1` | Pending my test |
| AGOS | Simon the Sorcerer II — `agos:simon2` | Pending my test |
| Gob | Gobliiins — `gob:gob1` | Pending my test |
| Gob | Gobliins 2: The Prince Buffoon — `gob:gob2` | Pending my test |
| Gob | Goblins Quest 3 — `gob:gob3` | Pending my test |
| Gob | The Bizarre Adventures of Woodruff and the Schnibble — `gob:woodruff` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 1 — `kyra:kyra1` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 2: Hand of Fate — `kyra:kyra2` | Pending my test |
| Kyra | The Legend of Kyrandia, Book 3: Malcolm's Revenge — `kyra:kyra3` | Pending my test |
| Kyra/EOB | Eye of the Beholder — `kyra:eob` | Pending my test |
| Kyra/EOB | Eye of the Beholder II: The Legend of Darkmoon — `kyra:eob2` | Pending my test |
| Darkseed | Dark Seed — `darkseed:darkseed` | Pending my test |
| SCUMM v7/v8 | Star Wars: Rebel Assault — `scumm:rebel1` | Pending my test |
| SCUMM v7/v8 | Star Wars: Rebel Assault II: The Hidden Empire — `scumm:rebel2` | Pending my test |
| Sherlock | The Lost Files of Sherlock Holmes: The Case of the Serrated Scalpel — `sherlock:scalpel` | Pending my test |
| Sherlock | The Lost Files of Sherlock Holmes: The Case of the Rose Tattoo — `sherlock:rosetattoo` | Pending my test |

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

Use **ScummVM-wiiu-0.8.0-candidate-20261026-aroma.zip** with Aroma. Extract the
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

## Build and evidence

Activate the shared Wii U SDK and run `build-0.8.0-test.sh`. It uses a fresh
`build-scummvm-wiiu-0.8.0-test-20261026` directory, enables all requested existing
engines statically, checks required features and restores generated Xeen data
after staging. WIIU_ENGINES_TEST identifies the candidate only; detailed
performance/IR diagnostics are off. The shared r51/WUT/SDL SDK and its seven
patches are unchanged. BUILD-INFO and SHA256SUMS record exact build provenance.

My earlier success report belongs to 2026.4.0 binary `90b6e646`; it does not
validate these newly enabled engines or this larger binary. All 22 listed game
tests and the combined build's Wii U runtime remain pending until I test them.
The final 0.8.0 release is a later step after hardware results and any fixes.
