# Engine/media validation

These checks supplement the full Wii U cross-build. They do not execute games,
GX2, AX, ProcUI or physical SD/USB, and supply no Wii U performance result.

`fluidsynth.c` links the actual patched FluidSynth 2.5.6 sources with the SDL2
OS abstraction, a host SDL2 library and ASan/UBSan. Pass an SF2 path as its only
argument. I use the `sf2/VintageDreamsWaves-v2.sf2` fixture from the pinned
FluidSynth source archive locally; it is not copied to the installation ZIP.
The checks cover mutex-backed atomic increments on four threads, independent
TLS, conditions, join/delete and detached lifetime, file checks/time, and five
SF2 load/render/concurrent-MIDI/unload cycles at 48 kHz. PCM must contain audio.
Use the same disabled-driver/SDL OSAL options as `build-media-sdk.sh` when
building FluidSynth on the host. Include its generated config, public and
internal headers when compiling this test.

`ppc-codecs.c` links the actual devkitPPC FLAC/MPEG-2/Theora/Ogg archives into
a Linux/PPC test using the workspace's `sdk/tests/ppc-linux/cc`, then executes
through its `run` wrapper. FLAC runs six stereo 4096-frame memory-callback
roundtrips (16/24-bit, three block sizes), comparing every PCM sample. MPEG-2
is initialized/closed and Theora rejects an invalid header. The latter two
checks are API smoke tests, not successful video decoding.

Use `-ffunction-sections -fdata-sections -Wl,--gc-sections,--no-warn-mismatch`
and link `-lFLAC -lmpeg2 -ltheoradec -logg -lm`. `--no-warn-mismatch` permits
the SDK's 64-bit versus Linux's 128-bit long-double ELF attribute; these tests
pass no long-double values. `__getreent` aborts if a newlib stdio path is
entered: all codec IO uses memory callbacks. PNG is deliberately excluded
from this mixed-libc harness because its setjmp ABI belongs to newlib.

PNG/FreeType and real MPEG-2/Theora/SF2 game playback still need console tests
using the exact delivered candidate. Keep test outputs, build objects and
private inputs in the project's `work/local/`, outside the shared test tools.
