#!/usr/bin/env bash
# Uses the prepared SDL with the hardware-validated zero-timeout fix.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-io-fix-20261012}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_IO_FIX_TEST"
bash "$wiiu_script_dir/build-timer-fix.sh" "$@"
grep -q -- '-DWIIU_IO_FIX_TEST' "$WIIU_BUILD_DIR/config.mk"
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/IO-FIX-20261012.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/IO-FIX-20261012.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/IO-FIX-20261012.md" "$wiiu_script_dir/build-io-fix.sh" \
        "$wiiu_script_dir/meta-rpx-io-fix.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-io-fix.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
