# Wii U expanded experimental build — September 30, 2026

**Test build, not a stable release. Cross-compilation is not console validation.**

## Included

- SCUMM v0–v6 (including the previously tested Sam & Max and Last Crusade).
- Mohawk, Myst, Myst Masterpiece Edition and Riven: The Sequel to Myst.
- Blade Runner (original 1997 game).
- Groovie and Groovie 2: The 7th Guest and The 11th Hour.
- Software RGB/16-bit graphics, JPEG, MP3 via libmad, integrated Munt MT-32.
- Internal QDM2 and Sorenson Video 1 components used by Myst ME media.
- Optimizations enabled (`-O2`), with debug logging/symbols retained in ELF.

This does not enable all ScummVM engines. SCUMM v7/v8 and HE remain disabled.
Modern remakes are not targets. Optional Vorbis, FLAC and MPEG-2 are not
included: some editions/encoded assets may require additional codecs.
The original Windows/DOS releases are the initial test targets. Do not assume
that detecting a game guarantees working audio/video or full-game completion.

## Freeze investigation

Community feedback reports intermittent freezes on Aroma in Fate of Atlantis
and The Secret of Monkey Island after saving, returning to the ScummVM launcher,
quitting, or confirming/cancelling the native keyboard. No failing log was
available. The cause has **not** been established on hardware.

This candidate hardens the existing SDL path:

- SDL emits a keyboard decision only once, and does not restart fade-out on
  repeated hide requests.
- SDL refuses to show a keyboard after failed creation or during a transition.
- Destroying a window finalizes the native keyboard, dropping its window
  reference and releasing its filesystem resources before process shutdown.
- SDL detaches the GX2 context before freeing the renderer's context memory
  while in the foreground. There is no new ScummVM graphics backend.
- ScummVM discards pending keyboard text when an engine finishes or quits;
  repeated close requests are harmless, and transition markers are logged.

Host tests exercise the actual Wii U event adapter using mocked SDL/Common,
including UTF-8, paced key pairs, cancellation, duplicate close, fade-out,
reopening and dropping partial submissions. They do **not** run the native
keyboard, GPU, audio driver, save system or console lifecycle.

## Install and regress first

1. Back up `SD:/scummvm/scummvm.ini`, `SD:/scummvm/saves/` and the old executable.
2. Choose **one** ZIP: Aroma WUHB or experimental RPX. Extract to the SD root,
   merging directories. Never delete the existing `scummvm` directory.
3. In Fate of Atlantis and The Secret of Monkey Island, use a **new** save slot.
   Name it using the keyboard, wait until all letters arrive, then save/reload.
4. Repeat keyboard OK and Cancel at least ten times, using touch and buttons.
   Include opening then cancelling without typing. Check controls afterward.
5. Save, return to the ScummVM launcher, start the game again, save, then Quit
   to the Wii U Menu. Repeat in separate sessions. Also compare with a session
   that never opens the keyboard, to help isolate the trigger.
6. Re-test Sam & Max and Last Crusade before testing the newly included games.
7. Test each new game separately: detection, intro/video, sound, controls,
   new save/load, return to launcher and Quit. Record edition/language/platform.

If it freezes, stop that test; do not repeatedly hard-reset important saves.
Copy `SD:/scummvm/scummvm.log` **before starting ScummVM again** (next launch
overwrites it). A hard reset may lose the last buffered messages. Report build,
game edition, exact steps, input method, loader and whether music continued.
Return to the previous executable if needed; preserve your backup configuration
and saves. Old packages remain available separately.

## MT-32

The emulator is included, but Roland ROM data is **not** distributed.
Supply your own lawfully obtained matching pair of `MT32_CONTROL.ROM` and
`MT32_PCM.ROM`, or a matching `CM32L_CONTROL.ROM` / `CM32L_PCM.ROM` pair.
Put them in `SD:/scummvm/data/` (the default Extra path), or set Extra path to
their directory. Select **MT-32 emulator** in the game's audio settings for a
game with MT-32 music. This does not upgrade games that have no MT-32 score.

MT-32 is CPU-intensive and has not been performance-tested here. Start with
one supported game; compare against AdLib if audio stutters. Do not make MT-32
the global default until you have tested it. Do not submit ROMs with reports.

MP3 support allows ScummVM to decode game audio stored in supported MP3-based
formats. No conversion of your game data is required or performed by this port.

## Build and dependency record

Use the existing Wii U SDK described in README.WIIU, with libjpeg-turbo 3.1.4.1
and libmad 0.15.1b installed in `$DEVKITPRO/portlibs/ppc`.
Apply both `sdl2-swkbd-input.patch` and `sdl2-swkbd-lifecycle.patch` to devkitPro
SDL `a8f1e43a` (wiiu-sdl2-2.32), and rebuild/install SDL before linking.
The lifecycle patch does not include the pre-existing KPADError compatibility
change; preserve that SDK fix as documented in README.WIIU.

After activating the SDK:

```sh
bash backends/platform/sdl/wiiu/build-expanded.sh
bash backends/platform/sdl/wiiu/tests/run.sh
```

The build uses a separate sibling `build-scummvm-wiiu-expanded` directory.
`WIIU_BUILD_DIR` can override it. Configuration checks fail if a requested
engine, RGB, JPEG, MP3 or MT-32 is missing. No original build or ZIP is removed.

Dependency archives (SHA-256 verified against devkitPro package recipes):

- libjpeg-turbo 3.1.4.1: `https://github.com/libjpeg-turbo/libjpeg-turbo/archive/refs/tags/3.1.4.1.tar.gz`
  — `a7da42b640377c2a9a9665e2c4b0ea60cd5599afb48c2521e6df0c9dc9d15a25`.
  CMake Release, static only, SIMD/tools/tests/thread-local disabled.
- libmad 0.15.1b: `https://downloads.sourceforge.net/mad/libmad-0.15.1b.tar.gz`
  — `bbfac3ed6bfbc2823d3775ebb931087371e142bb0e9bb1bee51a76a6e0078690`.
  devkitPro `ppc/libmad` frame-length safety patch applied. Static build with
  `-O2 -std=gnu17 -mcpu=750 -meabi -mhard-float`. On this host, autotools were
  unavailable: the shipped configure/Makefile.in were used, with CFLAGS
  overridden at make time to avoid obsolete `-fforce-mem` from old configure.

No games, Roland ROMs, user configuration, logs or saves belong in release ZIPs.
Do not publish this candidate as a confirmed fix before real-console retesting.
