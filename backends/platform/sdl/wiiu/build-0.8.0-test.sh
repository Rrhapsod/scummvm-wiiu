#!/usr/bin/env bash
# All-in-one 0.8.0 engine candidate with adaptive-stabilized IR; bounded Kyra/SAGA diagnostics.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
: "${DEVKITPPC:?Activate the Wii U SDK first}"
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-0.8.0-test-20261105}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_ENGINES_TEST -DWIIU_KYRA_IO_DIAGNOSTICS -DWIIU_SAGA_INPUT_DIAGNOSTICS"
if [[ ! -f "$WUT_ROOT/usr/lib/libfat.a" || ! -f "$WUT_ROOT/usr/lib/libmocha.a" ]]; then
    echo "USB SDK dependencies missing; see USB-SDK.md and build-usb-sdk.sh." >&2
    exit 1
fi
for symbol in WIIU_SuspendRemotePointers Blit_RGBA8888_RGB565; do
    if ! powerpc-eabi-nm "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep "$symbol" > /dev/null; then
        echo "Rebuild/install SDL with all seven patches in README.WIIU ($symbol missing)." >&2
        exit 1
    fi
done
if ! powerpc-eabi-strings "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep 'WiiU pointer: adaptive-stabilized IR' > /dev/null; then
    echo "Rebuild SDL with the ninth patch; see POINTER-SDK.md." >&2
    exit 1
fi
mkdir -p "$WIIU_BUILD_DIR"
cd "$WIIU_BUILD_DIR"
"$wiiu_src/configure" --host=wiiu --disable-all-engines \
    --enable-engine=scumm,scumm_7_8,mm,xeen,sci,sci32,mohawk,myst,mystme,riven,bladerunner,groovie,groovie2,tinsel,sword1,sword2,agos,gob,kyra,eob,darkseed,sherlock,saga,ihnm \
    --disable-engine=he,mm1,agos2,lol,rebel2_psx --disable-tinygl \
    --disable-detection-full --enable-optimizations \
    --enable-mt32emu --with-mad-prefix="$DEVKITPRO/portlibs/ppc" \
    --with-jpeg-prefix="$DEVKITPRO/portlibs/ppc" --enable-ogg --enable-vorbis --disable-tremor \
    --with-ogg-prefix="$DEVKITPRO/portlibs/ppc" --with-vorbis-prefix="$DEVKITPRO/portlibs/ppc" "$@"
for flag in ENABLE_SCUMM ENABLE_SCUMM_7_8 ENABLE_MM ENABLE_XEEN ENABLE_SCI ENABLE_SCI32 ENABLE_MOHAWK ENABLE_MYST ENABLE_MYSTME ENABLE_RIVEN ENABLE_BLADERUNNER ENABLE_GROOVIE ENABLE_GROOVIE2 ENABLE_TINSEL ENABLE_SWORD1 ENABLE_SWORD2 ENABLE_AGOS ENABLE_GOB ENABLE_KYRA ENABLE_EOB ENABLE_DARKSEED ENABLE_SHERLOCK ENABLE_SAGA ENABLE_IHNM USE_INDEO3 USE_ZLIB USE_RGB_COLOR USE_HIGHRES USE_JPEG USE_MAD USE_MT32EMU USE_OGG USE_VORBIS; do
    grep -Eq "^${flag}[[:space:]]*=" config.mk || { echo "Required feature missing: $flag" >&2; exit 1; }
done
for flag in ENABLE_HE ENABLE_MM1 ENABLE_AGOS2 ENABLE_LOL ENABLE_REBEL2_PSX USE_TINYGL USE_OPENGL; do
    if grep -Eq "^${flag}[[:space:]]*=" config.mk; then
        echo "Unexpected feature enabled: $flag" >&2; exit 1
    fi
done
if grep -Eq 'WIIU_(LIFECYCLE_DIAGNOSTICS|COMPAT_DIAGNOSTICS|REMOTE_TEST|MEDIA_TEST|FMV_BLIT_TEST|INPUT_FMV_TEST)' config.mk; then
    echo "Unexpected tracing profile in engine candidate" >&2; exit 1
fi
python3 "$wiiu_script_dir/prepare-mm-data.py" "$WIIU_BUILD_DIR/engine-data/mm.dat"
make -j"${JOBS:-2}" wiiu_release_all \
    WIIU_RELEASE_NOTES="$wiiu_script_dir/ENGINES-TEST-0.8.0.md" \
    WIIU_RPX_META="$wiiu_script_dir/meta-rpx-0.8.0-test.xml"
for stage in wiiu_release wiiu_rpx_release; do
    cp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
    cmp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
    cp "$wiiu_script_dir/ENGINES-TEST-0.8.0.md" "$stage/TESTING.md"
    cp "$wiiu_script_dir/USB-SDK.md" "$stage/USB-SDK.md"
    cp "$wiiu_script_dir/POINTER-SDK.md" "$stage/POINTER-SDK.md"
    cp "$wiiu_script_dir/POINTER-SDK.md" "$wiiu_script_dir/build-pointer-sdk.sh" \
        "$wiiu_script_dir/sdl2-wiimote-precision.patch" "$wiiu_script_dir/sdl2-wiimote-stability.patch" "$stage/scummvm/doc/backends/platform/sdl/wiiu/"
    cp "$wiiu_script_dir/USB-SDK.md" "$wiiu_script_dir/build-usb-sdk.sh" \
        "$wiiu_script_dir/libfat-wiiu-usb.patch" "$wiiu_script_dir/libmocha-wiiu-usb.patch" \
        "$stage/scummvm/doc/backends/platform/sdl/wiiu/"
    cp "$wiiu_script_dir/TEST-RESULTS-0.8.0.md" "$stage/TEST-RESULTS.md"
    cp "$wiiu_script_dir/ENGINES-TEST-0.8.0.md" "$wiiu_script_dir/build-0.8.0-test.sh" \
        "$wiiu_script_dir/meta-rpx-0.8.0-test.xml" "$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
