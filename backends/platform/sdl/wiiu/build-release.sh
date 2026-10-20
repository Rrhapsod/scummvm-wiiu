#!/usr/bin/env bash
# Wii U port 0.7.0: compatibility/media profile without diagnostic instrumentation.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
: "${DEVKITPPC:?Activate the Wii U SDK first}"
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-0.7.0}"
if printf '%s\n' "${CXXFLAGS:-} ${CPPFLAGS:-} ${CFLAGS:-}" | grep -E 'WIIU_[A-Z_]*(DIAGNOSTICS|TEST)' > /dev/null; then
    echo "Release builds must not enable Wii U diagnostics/test profiles" >&2
    exit 1
fi
for symbol in Blit_RGBA8888_RGB565 WIIU_SuspendRemotePointers; do
    if ! powerpc-eabi-nm "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep "$symbol" > /dev/null; then
        echo "Rebuild SDL with the seven patches listed in README.WIIU first ($symbol missing)" >&2
        exit 1
    fi
done
mkdir -p "$WIIU_BUILD_DIR"
cd "$WIIU_BUILD_DIR"
"$wiiu_src/configure" --host=wiiu --disable-all-engines \
    --enable-engine=scumm,scumm_7_8,mm,xeen,sci,sci32,mohawk,myst,mystme,riven,bladerunner,groovie,groovie2 \
    --disable-engine=he,mm1 --disable-detection-full --enable-optimizations \
    --enable-mt32emu --with-mad-prefix="$DEVKITPRO/portlibs/ppc" \
    --with-jpeg-prefix="$DEVKITPRO/portlibs/ppc" --enable-ogg --enable-vorbis --disable-tremor \
    --with-ogg-prefix="$DEVKITPRO/portlibs/ppc" --with-vorbis-prefix="$DEVKITPRO/portlibs/ppc" "$@"
if grep -E 'WIIU_[A-Z_]*(DIAGNOSTICS|TEST)' config.mk > /dev/null; then
    echo "Unexpected diagnostics/test flag in release configuration" >&2
    exit 1
fi
for flag in ENABLE_SCUMM ENABLE_SCUMM_7_8 ENABLE_MM ENABLE_XEEN ENABLE_SCI ENABLE_SCI32 ENABLE_MOHAWK ENABLE_MYST ENABLE_MYSTME ENABLE_RIVEN ENABLE_BLADERUNNER ENABLE_GROOVIE ENABLE_GROOVIE2 USE_RGB_COLOR USE_HIGHRES USE_JPEG USE_MAD USE_MT32EMU USE_OGG USE_VORBIS; do
    grep -Eq "^${flag}[[:space:]]*=" config.mk || { echo "Required feature missing: $flag" >&2; exit 1; }
done
python3 "$wiiu_script_dir/prepare-mm-data.py" "$WIIU_BUILD_DIR/engine-data/mm.dat"
make -j"${JOBS:-2}" wiiu_release_all
for stage in wiiu_release wiiu_rpx_release; do
    cp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
    cmp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
done
