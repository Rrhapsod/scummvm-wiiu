#!/usr/bin/env bash
# Requires SDL rebuilt with sdl2-timer-zero-timeout.patch; see TIMER-FIX-20261012.md.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-timer-fix-20261012}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_TIMER_FIX_TEST"
bash "$wiiu_script_dir/build-compat-diagnostic.sh" "$@"
grep -q -- '-DWIIU_TIMER_FIX_TEST' "$WIIU_BUILD_DIR/config.mk"
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/TIMER-FIX-20261012.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/TIMER-FIX-20261012.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/TIMER-FIX-20261012.md" "$wiiu_script_dir/build-timer-fix.sh" \
        "$wiiu_script_dir/sdl2-timer-zero-timeout.patch" "$wiiu_script_dir/meta-rpx-timer-fix.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-timer-fix.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
