#!/usr/bin/env bash
# Isolated post-0.6.0 compatibility candidate; does not rebuild release 0.6.0.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
: "${DEVKITPPC:?Activate the Wii U SDK first}"
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
wiiu_build="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-compat-20261009}"
mkdir -p "$wiiu_build"
cd "$wiiu_build"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_COMPATIBILITY_TEST"
"$wiiu_src/configure" --host=wiiu --disable-all-engines \
    --enable-engine=scumm,scumm_7_8,mm,xeen,sci,sci32,mohawk,myst,mystme,riven,bladerunner,groovie,groovie2 \
    --disable-engine=he,mm1 --disable-detection-full --enable-optimizations \
    --enable-mt32emu --with-mad-prefix="$DEVKITPRO/portlibs/ppc" \
    --with-jpeg-prefix="$DEVKITPRO/portlibs/ppc" "$@"
for wiiu_feature in ENABLE_SCUMM ENABLE_SCUMM_7_8 ENABLE_MM ENABLE_XEEN ENABLE_SCI ENABLE_SCI32 ENABLE_MOHAWK ENABLE_MYST ENABLE_MYSTME ENABLE_RIVEN ENABLE_BLADERUNNER ENABLE_GROOVIE ENABLE_GROOVIE2 USE_RGB_COLOR USE_HIGHRES USE_INDEO3 USE_TRUEMOTION1 USE_JPEG USE_MAD USE_MT32EMU; do
    grep -Eq "^${wiiu_feature}[[:space:]]*=" config.mk || {
        echo "Required build feature missing: $wiiu_feature" >&2; exit 1;
    }
done
python3 "$wiiu_script_dir/prepare-mm-data.py" "$wiiu_build/engine-data/mm.dat"
make -j"${JOBS:-2}" wiiu_release_all \
    WIIU_RELEASE_NOTES="$wiiu_script_dir/COMPATIBILITY-20261009.md" \
    WIIU_RPX_META="$wiiu_script_dir/meta-rpx-compat.xml"
for wiiu_stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_build/engine-data/mm.dat" "$wiiu_stage/scummvm/data/mm.dat"
    test -s "$wiiu_stage/scummvm/data/mm.dat"
    cmp "$wiiu_build/engine-data/mm.dat" "$wiiu_stage/scummvm/data/mm.dat"
    cp "$wiiu_script_dir/COMPATIBILITY-20261009.md" "$wiiu_stage/TESTING.md"
    cp "$wiiu_script_dir/COMPATIBILITY-20261009.md" "$wiiu_script_dir/build-compatibility.sh" \
        "$wiiu_script_dir/meta-rpx-compat.xml" "$wiiu_script_dir/prepare-mm-data.py" \
        "$wiiu_stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
