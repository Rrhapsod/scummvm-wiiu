# Audio timer / SCI resource diagnostic build — October 12, 2026

This is a second diagnostic build, not a confirmed repair or a stable release.
My October 10 logs show Curse opening all four original audio bundles, with
audio enabled, but queuing only six silent PCM buffers. Quest for Glory IV
CD/Windows stops inside resource initialization, before game graphics setup.
The logs narrow the investigation but do not establish either root cause.
I have not tested this new build on my Wii U yet.

## Install and test

1. Back up the installed executable, configuration and saves. Keep the October
   10 package for rollback. Use the Aroma ZIP for the existing Aroma setup.
2. Extract the entire ZIP to the SD root, including `scummvm/data/mm.dat`.
   Replace the existing executable instead of adding a duplicate entry.
3. No INI edits are required. The new startup line identifies **compatibility
   diagnostics 20261012**. Logs still append to
   **`SD:/scummvm/scummvm-compat-diagnostic.log`**, not `scummvm.log`.
4. Test **Curse alone**: finish or skip the intro, then play for about 30 seconds
   without changing audio settings. Note whether music, speech and effects are
   audible. Return to the launcher and Quit normally. Copy the log to the PC
   as `scummvm-compat-diagnostic-curse-20261012.log`.
5. From a fresh application launch, start **Quest for Glory IV CD/Windows**.
   If it hangs, stop testing and collect the log after recovery. Avoid repeated
   forced resets. Save this copy as `scummvm-compat-diagnostic-quest4-20261012.log`.
6. Send both copies and describe any change in behavior. If Quest starts, also
   report that result; tracing can change timing and does not prove a repair.

The log appends sessions, so the Quest copy may contain the earlier Curse
session too. That is expected. Archive the file periodically to limit its size.
If the new log is absent or empty after a normal launcher-only Quit, stop and
report that before repeating a test that hangs.

## What changed

- Lock-free 32-bit counters at SDL timer, digital iMUSE and wave callback
  entry/exit, including wave mutex acquisition and skipped iMUSE callbacks.
  Callbacks only increment counters: no diagnostic SD writes, clock reads or
  extra locks. Main-thread snapshots report cumulative process totals; values
  may advance during a snapshot and are not a transactional capture.
- Audio snapshots additionally report queue capacity, track pause and the
  music-transition skip flag. Existing 16-snapshot/two-second limits remain.
- SCI resource initialization markers bracket format detection, source scans,
  audio maps, script chunks and SCI version detection. Initial source/resource
  details identify filenames, resource IDs and load/patch completion. Detail
  output is capped at 256 messages; phase markers continue after that limit.
  Detailed resource tracing stops when initialization returns and is disabled
  during fallback detection; later audio-thread resource loads are not logged.

Playback, decoding, timers, resource loading order and game files are unchanged.
The SDL SDK is unchanged; the previous renderer fix, keyboard, MP3/MT-32 and
October 9 engine set remain included. No games, saves, configuration or ROMs
are bundled. RPX outside Aroma remains experimental and unverified.

Logs retain the earlier append + libc flush + WUT-backed `fsync` behavior.
This improves persistence but cannot guarantee survival of a forced reset.
Forced power-off can damage pending SD writes. Tracing changes timing and adds
startup overhead; do not publish this diagnostic package as a stable release.

## Rebuild and host checks

Activate the prepared SDK, then run:

```sh
bash backends/platform/sdl/wiiu/build-compat-diagnostic.sh
```

Default output: `../build-scummvm-wiiu-compat-diagnostic-20261012/`.
Earlier build/release directories are preserved.

Run host checks outside the cross-compiler environment:

```sh
python3 backends/platform/sdl/wiiu/tests/audio-diagnostics/run.py
python3 backends/platform/sdl/wiiu/tests/audio-probe/run.py
python3 backends/platform/sdl/wiiu/tests/sci-diagnostics/run.py
bash backends/platform/sdl/wiiu/tests/log/run.sh
```

Host tests and cross-compilation do not replace real-console testing.
