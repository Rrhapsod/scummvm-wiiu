# Adaptive Wii Remote stabilization — November 5, 2026

For final **0.8.0**, use `build-release.sh` with the complete stabilized
Pointer + USB SDK profile described in README.WIIU. It retains these patches
with candidate diagnostics off. I subsequently confirmed the Pointer and USB
browser scenarios on my console; see [RELEASE-0.8.0.md](RELEASE-0.8.0.md) for
reports and limits. The dated candidate verification below is historical.

I tested the October 30 pixel-precision candidate and reported smoother motion
but excessive stationary jitter compared with the Wii U Menu. Patch 8 removed
the old 4–6-pixel active-motion deadband; it did not filter IR noise crossing
integer pixels. Patch 9, `sdl2-wiimote-stability.patch`, adds adaptive smoothing
before integer conversion, retaining patch 8 and the earlier fixes.

The filter follows the speed-based [1 Euro principle](https://gery.casiez.net/1euro/)
(Casiez, Roussel and Vogel, CHI 2012), independently implemented here. It is not
a reconstruction of Nintendo's menu code. Near rest it reduces noise; during
motion its cutoff rises to reduce tracking delay. There is no spatial dead
zone. One shared cutoff preserves diagonal direction. Initial tuning is
0.8 Hz minimum cutoff, beta 0.03 and 1 Hz derivative cutoff in 1280x720
reference units, independent of output resolution. It uses elapsed sample
time; repeated polls within one millisecond do not advance the filter.

Each Remote has its own state. Tracking/ownership loss, suspend/disconnect,
window resize and a sample gap over 250 ms reset the filter. Reacquisition
anchors immediately at the valid position. Clicks use the displayed filtered
cursor; A/B transitions are delivered immediately without snapping to a noisy
raw position. The existing 350 ms touch/stick yield, acquisition hysteresis,
drag ownership, Sensor Bar request and release paths are retained. No added
KPAD reads, event loops, renderer or engine timing changes.

## Host verification and limits

ASan/UBSan tests compile the actual SDL helper and ScummVM event adapter.
Synthetic stationary noise crossing multiple pixels at 8/16/33 ms cadences
reduces output RMS variation to 25–32% of input (68–75% reduction), at 854,
1280 and 1920-wide output. This metric removes constant output offset;
integer quantization is included. It is not a physical IR measurement.

Twenty-four timed slow ramps cover horizontal/vertical/diagonal motion in both
directions, hover and held drags, with 320/854/1280/1920-wide output. Maximum
step remains one pixel. At 15.625 display pixels/second, tracking trails by at
most 3.13 pixels including integer conversion and converges to the final
target. For a synthetic 300-pixel/second ramp, observed tracking delay after
startup stays under 32 ms at the three tested cadences. These are trajectory
tests, not end-to-end console latency or frame-rate measurements.

The tests also cover immediate click/release, same-tick repeated polls, invalid
tracking, resize, long gaps, touch/stick handover, multiple controllers,
suspend/disconnect and timestamp wrap. Native keyboard tests remain separate.
The new binary still needs my console comparison for stationary stability,
small targets, tracking feel and rapid direction changes.

## Independent SDK and reconstruction

The new profile is `r51-wut1.9.1-sdl2.32.7-stable-pointer-20261105`, based on
the preserved October 30 pointer profile (which includes the USB dependencies
in [USB-SDK.md](USB-SDK.md)). Only SDL is rebuilt; compiler, WUT, codecs and
USB libraries are retained. The shared `current` profile is unchanged.

Activate a complete USB-capable base, choose two absent absolute destinations
and provide a devkitPro/SDL checkout containing pinned commit
`a8f1e43a4a9d70cbae397433e1a3dd40113679aa`:

```sh
export WIIU_SDK_ROOT=/path/to/sdk/versions/r51-wut1.9.1-sdl2.32.7-pointer-20261030
source /path/to/sdk/env.sh
export WIIU_POINTER_SDK_DEST=/path/to/sdk/versions/new-stable-pointer-profile
export WIIU_POINTER_SOURCES=/path/to/sdk/sources/new-stable-pointer-build
export WIIU_POINTER_SDL_REPO=/path/to/sdk/sources/SDL-devkitpro
JOBS=6 bash backends/platform/sdl/wiiu/build-pointer-sdk.sh
```

The recipe exports pinned SDL, applies patches 1–9 in README.WIIU order,
retains KPADError compatibility, relocates metadata, builds static SDL in a
fresh directory and records manifest hashes. It refuses existing destinations
and does not modify the source checkout, the base profile or `current`.

To use the prepared local profile:

```sh
export WIIU_SDK_ROOT=/home/renan/Documentos/wiiu-dev/sdk/versions/r51-wut1.9.1-sdl2.32.7-stable-pointer-20261105
source /home/renan/Documentos/wiiu-dev/sdk/env.sh
JOBS=6 bash backends/platform/sdl/wiiu/build-0.8.0-test.sh
```

The candidate helper uses a fresh build directory and requires the SDL
`adaptive-stabilized IR` marker. Existing engines/codecs, SAGA input, Kyra IO,
USB support and controller mappings are retained. Older SDKs and binaries stay
available for comparison. Copy the console log before relaunch when reporting
the exact new revision and hardware result.
