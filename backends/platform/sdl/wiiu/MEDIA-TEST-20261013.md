# Keyboard / Ogg Vorbis / Groovie test — October 13, 2026

This is a test candidate, not a stable release. I have not tested these exact
binaries on my Wii U yet. It retains the audio timer, renderer/keyboard
lifecycle and Curse/QFG4 loading fixes I already tested successfully.

## Keyboard editing

The native keyboard receives the existing field text in ScummVM GUI edit
fields/save lists and the original SCUMM save-name dialog used by Sam & Max.
The slot-number prefix is excluded. Confirming applies the changed suffix
using End/Backspace/text events instead of appending the entire result.
Cancel and unchanged confirmation leave the field alone. Confirming an empty
result removes the old name; it does not press Enter or save automatically.

Input remains paced for the original SCUMM dialogs. Replacing a long name can
take a few seconds after closing the keyboard; wait until the edit finishes
before saving. Existing game-specific character and length limits still apply.
The keyboard does not discover arbitrary text fields inside every engine;
unintegrated game dialogs keep the previous text-entry behavior.

## Ogg Vorbis audio

This profile links libogg 1.3.6 and libvorbis 1.3.7 into the executable. No
extra DLL/library files are needed on the SD. MP3 and MT-32 remain available.

Use a game's supported Vorbis-compressed resources or Ogg/Vorbis CD tracks
with the filenames and folder structure expected by its ScummVM engine.
Keep original data backed up. No conversion to MP3 is needed for supported
Vorbis resources. Renaming another format to .ogg does not convert it; .ogg
is a container extension and this change does not add Opus/Theora support.
Do not leave alternative encodings of the same track together during the
test, since another format may be selected and give a false positive.

Test intro/gameplay music and speech, seek/skip where supported, save/load and
return/reopen. Confirm using known Vorbis files; a working MP3 is not a Vorbis
test. Decoder availability does not make every engine accept arbitrary .ogg
filenames or external music replacements.

## Groovie investigation

The community reported choppy 11th Hour FMVs/audio, a Clandestiny crash after
the intro, extremely slow Tender Loving Care FMVs and an undetected Uncle
Henry's Playhouse. I only have The 7th Guest and The 11th Hour available to
test; the other cases have no reproducing files/logs yet. I am not claiming
these reports are fixed or that they share a single codec problem.

Changes in this candidate:

- Wii U resource streams use the existing ScummVM 64 KiB read buffer, reducing
  calls into stdio for byte-by-byte ROQ/VDX reads. Stream bounds/ownership and
  decoding are retained. No full-movie preload, frame skipping or custom
  audio/graphics backend was added. Performance still needs console timing.
- ROQ overlay flags are initialized before their first use. Stream ownership
  is initialized even when MPEG-2 is disabled, so file cleanup no longer reads
  an indeterminate flag. These are source-confirmed defects, not a demonstrated
  explanation or repair of the Clandestiny crash.
- Bounded main-thread video-file and slow-frame logs separate decode/read time
  from drawing time. Only 24 slow frames and the first 24 video loads are
  traced per player/script instance; logging itself affects timing.

Uncle Henry's Playhouse detection is already compiled in. Known PC releases
require `tpot.grv` together with `trt7g.gjd`; the known beta uses `tpt.gjd`.
Those are detection files, not a complete game-data list. The current detector
recognizes specific sizes/hashes and searches the selected directory plus
the configured MIDI/GROOVIE/MEDIA subdirectories (depth 2). Please send a file
listing and the exact PC ScummVM detection result/version for an undetected
copy. No guessed detection entry or permissive hash bypass has been added.

Tender Loving Care CD and DVD are different cases. This source labels DVD
entries unimplemented, and this candidate does not enable MPEG-2. Ask which
edition was tested; Ogg support does not add DVD-video support.

## Install and report

1. Back up saves/config/executable and retain the previous successful ZIP.
   Extract the complete Aroma ZIP to the SD root, replacing the executable.
   No INI edit is required. Non-Aroma RPX remains experimental/unverified.
2. Check the log identifies **media test 20261013**. Logging still appends to
   **`SD:/scummvm/scummvm-compat-diagnostic.log`**.
3. In Sam & Max, select a named save slot. Confirm that the native keyboard
   shows its name without the slot number; edit, erase, cancel, reopen and
   confirm unchanged. Test a new slot and ScummVM's own save dialog too.
4. Test known Ogg/Vorbis resources as described above. Retest MP3/MT-32,
   Curse audio/transitions and QFG4 startup as regressions.
5. Play the same 7th Guest/11th Hour movie sequence as before. Note audio
   interruptions, approximate smoothness and any delay when movies change.
   Return/reopen several times and Quit normally. Preserve a separate log.
6. For the other community reports, request a log from this build, exact
   edition/language, movie/scene and whether it freezes, exits or shows an
   error. After a genuine hang, copy the log once the console is fully off,
   before relaunching. Avoid repeated hard resets; fsync cannot guarantee
   persistence or prevent SD corruption after power loss.

No games, ROMs, saves, user configuration or logs are bundled.

## Rebuild

After activating the prepared SDK, build the pinned codecs once:

```sh
WIIU_CODEC_DIR=/absolute/path/to/codec-sources bash backends/platform/sdl/wiiu/build-vorbis-sdk.sh
bash backends/platform/sdl/wiiu/build-media-test.sh
```

The codec script verifies archive SHA-256 values against the inspected
devkitPro recipes and installs static libraries into the local SDK, without
sudo. The SDL archive and its prior patches are unchanged. Later local
autodetected builds may pick up the new codecs; existing binaries/ZIPs are
preserved. Default output: `../build-scummvm-wiiu-media-test-20261013/`.
