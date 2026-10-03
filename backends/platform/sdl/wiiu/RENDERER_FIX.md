# Wii U renderer memory fix — test candidate, 2026-10-02

Historical candidate instructions. On October 3 I confirmed the repeated-exit
freeze was resolved; my log shows four successful game-return cycles,
keyboard use and logged shutdown cleanup. The fix is included in port v0.6.0;
see [release notes](RELEASE-0.6.0.md) for current status and installation.

This package retains automatic lifecycle tracing and the expanded engine,
MP3 and MT-32 support. It is not yet a hardware-confirmed freeze fix.

## Evidence and scope

My log from diagnostic build 337cc316 shows Sam & Max returning to
the launcher successfully, followed by Fate of Atlantis exiting about four
seconds after launch. Engine and MIDI cleanup completed. The log stops at
`VIDEO before SDL_CreateRenderer` while restoring the launcher.

The Wii U SDL renderer allocates a private window texture in MEM1. That
texture is embedded in its driver data, not linked into SDL's public texture
list. The renderer destructor did not release it. Window texture allocation
errors were also ignored, allowing the MEM1 allocation sentinel to be used
as if it were valid texture data. This is a concrete source-level leak and
error-handling defect, consistent with the log, not proof of the entire
hardware failure's cause.

The patch releases the private texture before renderer data is freed, waits
for GPU completion before releasing window storage, resets its ownership,
checks context allocation, and unwinds failed renderer creation. Replacement
allocation is transactional: a failed resize keeps the old texture intact.
Initial creation, resize and foreground-reacquisition callers check errors.
No audio, keyboard or engine teardown behavior is changed.

## Test on Aroma

1. Back up your current executable, configuration and saves.
2. Extract the Aroma candidate ZIP to the SD root, replacing
   `wiiu/apps/ScummVM.wuhb` and merging the bundled data/docs.
3. Check the log for `BUILD renderer-memory-fix candidate 20261002`.
4. Repeat the original sequence: launch Sam & Max, return to the ScummVM
   launcher, launch Fate of Atlantis, and return quickly. No keyboard or save
   is required. Also try repeated quick launch/return with the same game.
5. If successful, repeat for at least ten cycles, then test saving, keyboard
   confirm/cancel and Quit to the Wii U menu. Report which scenarios passed.
6. If it freezes, stop and preserve `SD:/scummvm/scummvm.log` before reopening
   ScummVM. Send the log even if the repeated-exit test succeeds.

No INI edits are needed. Extra logging changes timing; a successful short test
does not prove that every intermittent freeze is fixed. The original package
can be restored for rollback. RPX is a separate experimental non-Aroma package.

## Rebuild and host checks

Using devkitPro SDL revision a8f1e43a with the existing SDK compatibility fixes,
apply `sdl2-swkbd-input.patch`, `sdl2-swkbd-lifecycle.patch`, then
`sdl2-renderer-lifecycle.patch` from this directory. Rebuild and install SDL2
into the prepared Wii U SDK before running `build-diagnostic.sh` from the
ScummVM checkout. Do not reuse an older release output directory.

With a native host compiler:

```sh
python3 backends/platform/sdl/wiiu/tests/renderer/run.py /path/to/SDL/src/render/wiiu/SDL_render_wiiu.c
bash backends/platform/sdl/wiiu/tests/diagnostics/run.sh
bash backends/platform/sdl/wiiu/tests/run.sh
```

The renderer test extracts the actual patched lifecycle function bodies and
compiles them against mocks under ASan/UBSan. It exercises repeated ownership
cycles, failed allocations, replacement and cleanup. GPU/ProcUI timing is not
emulated; cross-compilation and mock tests cannot replace console testing.
