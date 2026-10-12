# Curse audio / Quest for Glory IV diagnostic build — October 10, 2026

Historical test instructions for the October 10 artifacts. For the current
build helper and next test, see `COMPAT-DIAGNOSTIC-20261012.md`. Earlier
packages remain unchanged.

This is a diagnostic build, not a confirmed repair or a stable release.
I tested the October 9 candidate: Curse ran without sound; World of Xeen was
playable; Quest for Glory IV CD/Windows went black at startup and the ordinary
log was empty after power-off. I have not tested this diagnostic build yet.

The supplied Curse files have four original-format LB83 audio bundles, with
directory entries inside the file bounds. The supplied INI has `mute=false`.
These checks do not prove that every audio payload or the SD copy is intact.
No supplied game files or INI settings were modified or bundled.

## Install and collect evidence

1. Back up the installed executable, configuration and saves. Keep the October 9
   ZIP for rollback. Choose Aroma unless deliberately testing an RPX loader.
2. Extract the entire ZIP to the SD root, including `scummvm/data/mm.dat`.
   Replace the existing executable; do not install duplicate launcher entries.
3. Start ScummVM, then use **Quit** without opening a game. Check that
   **`SD:/scummvm/scummvm-compat-diagnostic.log`** is nonempty. If absent or
   empty, stop and report it before trying a game that hangs.
4. Test **Curse alone**: note sound during the intro, then reach the interactive
   game (finish or skip the intro) and play for roughly 30 seconds without
   changing audio settings. Note sound on both TV and GamePad, return to the
   launcher and quit normally. Copy the diagnostic log to your computer.
5. Test **Quest for Glory IV CD/Windows** from a fresh launch. If it hangs,
   preserve the diagnostic log after recovery. Do not repeatedly force-reset
   the console. Send the log even if the final line looks unrelated.

This build uses the separate diagnostic filename even if the INI specifies
another log path. No INI edits or debuglevel change are required. It appends
new sessions instead of truncating the previous one. Session headers and
`BUILD` markers separate launches; the ordinary `scummvm.log` is left alone.
Archive the diagnostic file periodically: it grows across launches.

Every log flush also calls POSIX `fsync`, mapped to `FSAFlushFile` by WUT.
This improves the chance that markers survive a forced reset, but does not
guarantee persistence through a console/SD failure. A power cut can still
damage pending data. Logging changes timing and can reduce performance.

## What is instrumented

- Existing engine/renderer lifecycle transitions.
- SCI resource setup, game object, kernel, game/graphics/sound initialization
  and entry into game execution.
- Curse bundle-open results and digital iMUSE startup settings.
- Up to 16 audio snapshots, about two seconds apart during the main game loop:
  pause/mute state, volume, queue progress and whether queued PCM has nonzero
  bytes. Intro movies may delay these main-loop snapshots.
- Audio callbacks only update counters; they do not write these diagnostic
  messages. Counter snapshots are copied under the resource mutex and logged
  after releasing it. Normal-latency PCM counters do not cover separate
  low-latency streams; keep the supplied `dimuse_low_latency_mode=false`.

Playback, decoding, SCI startup order and game data are unchanged. The previous
SDL renderer fix, keyboard, MP3/MT-32 and all October 9 engines remain enabled.
Xeen controls are unchanged: **Y is Escape**, which exits from its title
background. Use A/click for Start Game. Enable **Return to the launcher when
leaving a game** in Global Options if you prefer not to close ScummVM.
RPX outside Aroma remains unverified.

## Rebuild

Activate the prepared SDK with the existing SDL patches, then run:

```sh
bash backends/platform/sdl/wiiu/build-compat-diagnostic.sh
```

Default output: `../build-scummvm-wiiu-compat-diagnostic-20261010/`.
The prior compatibility and release directories are not rebuilt.

Additional host checks (mocks/host filesystem, not real SD power-loss tests):

```sh
bash backends/platform/sdl/wiiu/tests/log/run.sh
python3 backends/platform/sdl/wiiu/tests/audio-diagnostics/run.py
```
