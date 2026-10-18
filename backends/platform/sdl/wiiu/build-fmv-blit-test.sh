#!/usr/bin/env bash
# Requires SDL rebuilt with sdl2-rgba565-blit.patch and all previous patches.
set -euo pipefail
: "${DEVKITPRO:?Activate the Wii U SDK first}"
if ! powerpc-eabi-nm "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep 'Blit_RGBA8888_RGB565' > /dev/null; then
    echo "Rebuild/install SDL with sdl2-rgba565-blit.patch before this candidate." >&2
    exit 1
fi
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-fmv-blit-20261018-r2}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_FMV_BLIT_TEST"
bash "$wiiu_script_dir/build-input-fmv-test.sh" "$@"
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/FMV-BLIT-20261018-R2.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/FMV-BLIT-20261018-R2.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/FMV-BLIT-20261018-R2.md" "$wiiu_script_dir/build-fmv-blit-test.sh" \
        "$wiiu_script_dir/sdl2-rgba565-blit.patch" "$wiiu_script_dir/meta-rpx-fmv-blit.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-fmv-blit.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
