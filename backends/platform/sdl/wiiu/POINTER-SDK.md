# Pixel-precision Wii Remote candidate — October 30, 2026

The existing IR helper applied a display-width-dependent deadband to every
movement, including drags: 4 pixels at 1280-wide output and 6 at 1920-wide.
Small deliberate movements therefore accumulated before moving the cursor.
The eighth incremental SDL patch, `sdl2-wiimote-precision.patch`, separates
cursor ownership from motion precision. The active Remote now reports every
changed display pixel. The larger acquisition threshold and 350 ms yield
after touch/sticks remain, as do click ordering and held-button releases.
Identical pixel positions remain silent. No interpolation, delayed averaging,
extra KPAD reads or changes to game timing are introduced. Games still have
their own logical cursor resolution and frame rate.

Host ASan/UBSan tests compile the actual helper and event adapter. A synthetic
64-pixel slow ramp previously generated 16 movements in 4-pixel steps at
1280x720. The updated helper generates 64 one-pixel movements. Horizontal,
vertical and diagonal ramps in both directions, held drags and 320/854/1280/
1920-wide outputs pass, along with input arbitration, same-pixel stability,
click/release, invalid tracking, multiple Remotes and suspend/disconnect tests.
These are deterministic mock samples, not measured Sensor Bar tracking or
console frame rates. My console comparison of this new binary is pending.

## Independent SDK profile

I keep the existing shared default, USB profile and prepared binaries intact.
The new profile is `r51-wut1.9.1-sdl2.32.7-pointer-20261030`, based on the
independent USB SDK in [USB-SDK.md](USB-SDK.md). Only SDL is rebuilt; the r51
compiler, WUT, codecs and USB libraries are retained.

For a fresh reconstruction, activate a complete USB-capable base SDK. Choose
two absent absolute destinations and a devkitPro/SDL Git checkout containing
commit `a8f1e43a4a9d70cbae397433e1a3dd40113679aa`:

```sh
export WIIU_SDK_ROOT=/path/to/sdk/versions/r51-wut1.9.1-sdl2.32.7-usb-20261028
source /path/to/sdk/env.sh
export WIIU_POINTER_SDK_DEST=/path/to/sdk/versions/new-pointer-profile
export WIIU_POINTER_SOURCES=/path/to/sdk/sources/new-pointer-build
export WIIU_POINTER_SDL_REPO=/path/to/sdk/sources/SDL-devkitpro
JOBS=6 bash backends/platform/sdl/wiiu/build-pointer-sdk.sh
```

The recipe exports the pinned SDL commit without changing that checkout,
applies patches 1–7 from README.WIIU followed by patch 8, retains the existing
KPADError compatibility adjustment, relocates dependency metadata, builds
static SDL in a fresh directory and records manifest hashes/provenance.
It refuses existing destinations and does not switch `current`.

To build the combined candidate with the prepared local profile:

```sh
export WIIU_SDK_ROOT=/home/renan/Documentos/wiiu-dev/sdk/versions/r51-wut1.9.1-sdl2.32.7-pointer-20261030
source /home/renan/Documentos/wiiu-dev/sdk/env.sh
JOBS=6 bash backends/platform/sdl/wiiu/build-0.8.0-test.sh
```

Use a fresh ScummVM build directory. The candidate helper checks the SDL
precision marker as well as the existing required symbols. All engines,
codecs, SAGA opening-input and Kyra IO fixes, USB support and button mappings
are retained. The new SDL activation message contains `pixel-precision IR`.
Copy the console log before relaunch when reporting the hardware comparison.
