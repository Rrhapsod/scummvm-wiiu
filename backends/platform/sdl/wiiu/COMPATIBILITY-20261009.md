# Wii U compatibility test — October 9, 2026

This is a separate test build based on my 0.6.0 port, not a new stable release.
I am adding the existing upstream engines below for the next console test round.
I have not yet tested these three games with this build on my Wii U. Enabling
an engine does not establish performance, full-game compatibility or completion.

| Test target | Enabled engine |
| --- | --- |
| The Curse of Monkey Island | SCUMM v8 (the v7/v8 subengine is enabled) |
| Might and Magic: World of Xeen | MM with Xeen |
| Quest for Glory IV: Shadows of Darkness | SCI with SCI32 (full floppy and CD versions need SCI32) |

The earlier SCUMM v0–v6, Mohawk/Myst/Myst ME/Riven, Blade Runner and
Groovie/Groovie2 engines remain enabled, together with MP3, JPEG and MT-32.
The SDL renderer cleanup fix is retained. This is not the lifecycle diagnostic
build. HE and Might and Magic I are outside this test profile.

## Installation

1. Back up the installed executable, `SD:/scummvm/scummvm.ini` and
   `SD:/scummvm/saves/`. Keep the 0.6.0 package for rollback.
2. Choose the Aroma ZIP for the Wii U Menu, or the experimental RPX ZIP for
   an RPX-capable loader. I have not validated non-Aroma operation.
3. Extract the complete ZIP to the SD root, merging folders. Replace the old
   executable; do not keep duplicate launcher entries. Do not delete saves.
4. Ensure `SD:/scummvm/data/mm.dat` is present. This supplied engine data file
   is required by Xeen, is not a game, and does not replace your game files.
5. Add your own original game data through **Add Game**. Keep each game in
   its own directory and include data from all discs where applicable. Do not
   substitute a remake, video-only files or a modern game executable.

If an existing configuration overrides **Extra Path**, point it to the supplied
data directory (`/vol/external01/scummvm/data`) if engine data cannot be found.
No other INI change is required. The packaged README describes 0.6.0; this
document takes precedence for the additional engines and this build's status.
No games, saves, user configuration or MT-32 ROMs are included.

## Controls to verify

The touchscreen and analog pointer use the existing backend. For SCUMM,
the default mappings remain A = left click, B = right click, R = slow pointer,
L = game menu, + = ScummVM menu and − = native keyboard.
Do not assume that every engine uses the same game-specific buttons.

Xeen now has an additional remappable keymap: D-pad up/down moves forward/
backward, D-pad left/right turns, **X sends Enter** and **Y sends Escape**.
A/B remain left/right click. Use the pointer for party portraits and other
on-screen controls, including the sidestep arrows. After entering text with
the native keyboard, wait for delivery and press X to confirm in Xeen.
These bindings are implemented but still need console testing.

Use **Game Options → Keymaps** where available to inspect or change each
game's bindings. Xeen movement, turning, party selection and combat need
special attention; Quest for Glory IV also needs movement and combat tests.
The native keyboard is for text entry, not a replacement for held movement
keys. When naming a save, wait for text delivery and explicitly activate the
game's confirmation button; keyboard OK does not send Enter.

## Hardware checklist

- All three: detection, startup, intro, music/speech, touch and analog input.
- Curse: verb interface, inventory, dialogue, scene transitions and movies.
- Xeen: move/turn, select party members, combat, spell/item dialogs, and
  travel between Clouds and Darkside when reachable in the available save.
- Quest for Glory IV: character creation, navigation, dialogue, inventory and
  combat; include speech if using the CD edition.
- Each: save with a name, load it, return to the launcher, reopen and quickly
  return several times in the same session, then quit to the system menu.
- Recheck one previously working SCUMM game to catch regressions.

Report the game edition/language, package/loader, scenario and whether sound
or video stalls. On a freeze, stop and preserve `SD:/scummvm/scummvm.log`
before relaunching; the last message may precede the actual failure.
Keep a backup of saves before testing to limit data-loss risk.

## Rebuilding

Activate the prepared SDK (including the existing SDL keyboard and renderer
patches), then run from the repository root:

```sh
bash backends/platform/sdl/wiiu/build-compatibility.sh
```

The default output is `../build-scummvm-wiiu-compat-20261009/`; previous build
directories are untouched. `WIIU_BUILD_DIR` and `JOBS` can override the output
directory and parallel jobs. The script checks all requested engine/audio
features and stages matching Aroma and RPX distributions, including `mm.dat`.
The startup log explicitly identifies the compatibility test profile.

The bundled upstream `mm.dat` has Xeen data version 1.2, while this source
requires 1.3. `prepare-mm-data.py` uses a native C++ compiler to regenerate all
four languages' constants and key tables with the checkout's `create_xeen`
tool, verifies the required version, and preserves the other bundled data.
This does not merely relabel the old archive and needs no local game files.
Python 3 and a native `c++` are required (`WIIU_HOST_CXX` can override the
native compiler). The source archive's original `mm.dat` is not modified.

Host regression checks (mocks, not hardware validation):

```sh
bash backends/platform/sdl/wiiu/tests/run.sh
bash backends/platform/sdl/wiiu/tests/diagnostics/run.sh
python3 backends/platform/sdl/wiiu/tests/xeen/run.py
python3 backends/platform/sdl/wiiu/tests/renderer/run.py ../SDL-devkitpro/src/render/wiiu/SDL_render_wiiu.c
```
