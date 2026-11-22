# Media SDK for the Wii U 0.9.0 candidate

I use an independent SDK based on the nine-patch SDL2/USB profile. The shared
`current` link and previous profiles/releases are preserved. Source archives,
build trees and evidence live in the shared SDK hub, outside application Git.

```sh
export WIIU_SDK_ROOT=/home/renan/Documentos/wiiu-dev/sdk/versions/r51-wut1.9.1-sdl2.32.7-stable-pointer-20261105
source /home/renan/Documentos/wiiu-dev/sdk/env.sh
export WIIU_MEDIA_SDK_DEST=/home/renan/Documentos/wiiu-dev/sdk/versions/r51-wut1.9.1-sdl2.32.7-media-20261122
export WIIU_MEDIA_SOURCES=/home/renan/Documentos/wiiu-dev/sdk/sources/scummvm-media-20261122
JOBS=6 bash backends/platform/sdl/wiiu/build-media-sdk.sh
```

The destination profile must not exist. Use a new source/build directory when
reproducing the recipe. Archives are pinned and SHA-256 checked against
`media-sources.json`. Versions are FLAC 1.5.0, PNG 1.6.59, FreeType 2.14.1,
Theora 1.2.0, MPEG-2 0.5.1 and FluidSynth 2.5.6. FluidSynth's build-only GCEM
headers are pinned to `012ae73c6d0a2cb09ffe86475f5c6fba3926e200`.

Local adaptations:

- FLAC's Rice residual pointers and escape sample use `FLAC__int32`
  consistently: newlib defines `int32_t` as `long`, distinct from `int`.
- PNG/FreeType use CMake's ZLIB/PNG find modules. The base SDK's static-only
  zlib config lists a nonexistent shared export; its bytes are preserved.
- FluidSynth uses the small `fluid_sys_wiiu.c/.h` SDL2 OS abstraction. Real
  mutexes/TLS protect synth calls and sample-cache state. SDL supplies time and
  threads; ScummVM supplies the audio mixer and MIDI timer. GLib, platform
  audio/MIDI drivers, network, native DLS, SF3/libsndfile and parallel voice
  synthesis are disabled. SF2 loading remains enabled.
- MPEG-2 acceleration probing is disabled: Espresso has no AltiVec. Theora is
  decoder-only, without assembly. No FFmpeg dependency is introduced.

The recipe installs notices in `portlibs/ppc/licenses/` and records installed
file hashes, versions and source origins in the new `MANIFEST.json`. Preserve
the source archives and these patches for corresponding-source reconstruction.

Activate the new profile before building ScummVM:

```sh
export WIIU_SDK_ROOT=/home/renan/Documentos/wiiu-dev/sdk/versions/r51-wut1.9.1-sdl2.32.7-media-20261122
source /home/renan/Documentos/wiiu-dev/sdk/env.sh
bash /home/renan/Documentos/wiiu-dev/sdk/doctor.sh
JOBS=6 bash backends/platform/sdl/wiiu/build-0.9.0-test.sh
```

The candidate helper defaults to a new out-of-tree build and verifies required
engine/codec flags. The normal `build-release.sh` retains the prepared 0.8.0
profile. See `ENGINES-TEST-0.9.0.md` for the Wintermute 3D limitation and console
test checklist. Cross-builds and host tests are separate from Wii U runtime.
