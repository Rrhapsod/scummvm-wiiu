#!/usr/bin/env bash
# Run after activating the prepared Wii U SDK. Never touches the baseline build.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
: "${DEVKITPPC:?Activate the Wii U SDK first}"
wiiu_src="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)"
wiiu_build="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-expanded}"
mkdir -p "$wiiu_build"
cd "$wiiu_build"
"$wiiu_src/configure" --host=wiiu --disable-all-engines \
    --enable-engine=scumm,mohawk,myst,mystme,riven,bladerunner,groovie,groovie2 \
    --disable-detection-full --enable-optimizations --enable-mt32emu --with-mad-prefix="$DEVKITPRO/portlibs/ppc" \
    --with-jpeg-prefix="$DEVKITPRO/portlibs/ppc" "$@"
# Autodetection must not silently drop a requested engine or dependency.
for wiiu_feature in ENABLE_SCUMM ENABLE_MOHAWK ENABLE_MYST ENABLE_MYSTME ENABLE_RIVEN ENABLE_BLADERUNNER ENABLE_GROOVIE ENABLE_GROOVIE2 USE_RGB_COLOR USE_JPEG USE_MAD USE_MT32EMU; do
    grep -Eq "^${wiiu_feature}[[:space:]]*=" config.mk || {
        echo "Required build feature missing: $wiiu_feature" >&2; exit 1;
    }
done
make -j"${JOBS:-2}" wiiu_release_all
