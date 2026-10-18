#!/usr/bin/env bash
# Requires SDL rebuilt with sdl2-swkbd-controller.patch on the prior patches.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-input-fmv-20261018}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_INPUT_FMV_TEST"
bash "$wiiu_script_dir/build-media-test.sh" "$@"
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/INPUT-FMV-20261018.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/INPUT-FMV-20261018.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/INPUT-FMV-20261018.md" "$wiiu_script_dir/build-input-fmv-test.sh" \
        "$wiiu_script_dir/sdl2-swkbd-controller.patch" "$wiiu_script_dir/meta-rpx-input-fmv.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-input-fmv.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
