#!/usr/bin/env bash
# Isolated 0.7.0 feature profile + bounded Wii Remote diagnostics, no release replacement.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
: "${DEVKITPPC:?Activate the Wii U SDK first}"
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-remote-20261021-escape}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_REMOTE_TEST"
for symbol in WIIU_SuspendRemotePointers Blit_RGBA8888_RGB565; do
    if ! powerpc-eabi-nm "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep "$symbol" > /dev/null; then
        echo "Rebuild/install SDL with all seven patches in README.WIIU ($symbol missing)." >&2
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
for flag in ENABLE_SCUMM ENABLE_SCUMM_7_8 ENABLE_MM ENABLE_XEEN ENABLE_SCI ENABLE_SCI32 ENABLE_MOHAWK ENABLE_MYST ENABLE_MYSTME ENABLE_RIVEN ENABLE_BLADERUNNER ENABLE_GROOVIE ENABLE_GROOVIE2 USE_RGB_COLOR USE_HIGHRES USE_JPEG USE_MAD USE_MT32EMU USE_OGG USE_VORBIS; do
    grep -Eq "^${flag}[[:space:]]*=" config.mk || { echo "Required feature missing: $flag" >&2; exit 1; }
done
python3 "$wiiu_script_dir/prepare-mm-data.py" "$WIIU_BUILD_DIR/engine-data/mm.dat"
make -j"${JOBS:-2}" wiiu_release_all \
    WIIU_RELEASE_NOTES="$wiiu_script_dir/REMOTE-TEST-20261021.md" \
    WIIU_RPX_META="$wiiu_script_dir/meta-rpx-remote.xml"
for stage in wiiu_release wiiu_rpx_release; do
    cp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
    cmp engine-data/mm.dat "$stage/scummvm/data/mm.dat"
    cp "$wiiu_script_dir/REMOTE-TEST-20261021.md" "$stage/TESTING.md"
    cp "$wiiu_script_dir/REMOTE-TEST-20261021.md" "$wiiu_script_dir/build-remote-test.sh" \
        "$wiiu_script_dir/meta-rpx-remote.xml" "$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
