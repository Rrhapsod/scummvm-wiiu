# Wii U lifecycle diagnostic build — 2026-10-02

Update: the current build script produces the renderer-memory-fix candidate
described in `RENDERER_FIX.md`, with this tracing retained. The unchanged-SDK
description below refers to the original `337cc316` diagnostic build only.
The new startup marker is `BUILD renderer-memory-fix candidate 20261002`;
its default build directory is `build-scummvm-wiiu-renderer-fix-20261002`.

This is a test build, **not a confirmed freeze fix or a stable release**.
It investigates intermittent freezes when leaving a game, particularly when
the first return to the launcher succeeds but a quick second launch/exit freezes.
The issue has also been reported without keyboard use and without MT-32.

## Install and test

1. Back up `SD:/scummvm/scummvm.ini`, saves, and your existing executable.
2. For Aroma, extract the Aroma diagnostic ZIP to the SD root, replacing
   `wiiu/apps/ScummVM.wuhb` and merging the bundled `scummvm` data/docs folders.
   Do not install both distributions. The separate RPX package is experimental
   for non-Aroma Homebrew Launcher setups, not hardware-validated here.
3. Start ScummVM, launch a game, and return to the ScummVM launcher once.
4. In the **same ScummVM session**, launch the same game again and quickly
   return to the launcher. No saving or keyboard input is needed for this test.
5. If this succeeds, separately try the same sequence using Quit. Report which
   action froze (return to ScummVM or exit to the Wii U launcher).
6. Stop after the first freeze. Once the console is safely powered off, copy
   `SD:/scummvm/scummvm.log` **before starting ScummVM again**: a new session can
   overwrite it. Send the log, game/edition, exit action, and successful cycle
   count. If there is no freeze, send the completed session log instead.

No `.ini` edit or `debuglevel` setting is required. Check for the startup marker
`BUILD lifecycle diagnostic 20261002` to identify the diagnostic executable.
If a custom log path was previously configured, retrieve that file instead.

## What is recorded

`[WIIU-DIAG seq=... cycle=... ms=...]` messages identify each game cycle and
boundaries around engine shutdown, SCUMM music cleanup, mixer cleanup, pending
event draining, texture/renderer recreation, launcher setup, and backend quit.
Cycle 0 is initial launcher setup; cycle 1 is the first game launch.
The old `engineDone` message was only a backend hook, not confirmation that
the engine had been destroyed or that the launcher was ready.

Tracing is main-thread-only and uses the existing SDL file logger, which
flushes each message. A hard hang/power loss can still lose buffered SD data.
The last line narrows down where to investigate; it does not establish a root
cause. Final `SDL_Quit` runs after the normal log closes and is not traced.
Extra logging changes timing and may hide a race, so success is not proof of
a repair. Review logs for private paths before sharing them publicly.

## Features and rollback

The expanded engine/MP3/MT-32 profile and existing SDL SDK are retained.
Audio behavior, keyboard behavior, synchronization, and destruction order are
unchanged; only lifecycle tracing and an accurate hook message were added.
For audio setup, see the main README. MT-32 ROMs and game files are not bundled.
Restore your backed-up executable to return to the previous build.

## Reproduce the build

Activate the prepared Wii U SDK, then run:

```sh
bash backends/platform/sdl/wiiu/build-diagnostic.sh
```

The wrapper invokes `build-expanded.sh` with the compile-time flag
`-DWIIU_LIFECYCLE_DIAGNOSTICS`, using a separate sibling directory,
`build-scummvm-wiiu-diagnostic-20261002`. `WIIU_BUILD_DIR` can override it;
choose a fresh directory, not an existing release directory. Standard builds
do not enable the diagnostic calls. Host tests and cross-compilation are not
console validation; this build still needs the reproduction above on hardware.

Host-only regression checks (run with a native C++ compiler, before activating
the cross-compiler environment):

```sh
bash backends/platform/sdl/wiiu/tests/diagnostics/run.sh
bash backends/platform/sdl/wiiu/tests/run.sh
```

The diagnostic test covers automatic info-level messages, sequence/cycle IDs,
bounded formatting, a missing system, and disabled-build no-ops. Both scripts
use AddressSanitizer and UndefinedBehaviorSanitizer with mock backends.
