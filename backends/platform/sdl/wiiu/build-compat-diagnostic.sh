#!/usr/bin/env bash
# Opt-in diagnostics, preserving the October 9 compatibility candidate.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-compat-diagnostic-20261010}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_LIFECYCLE_DIAGNOSTICS -DWIIU_COMPAT_DIAGNOSTICS"
bash "$wiiu_script_dir/build-compatibility.sh" "$@"
for flag in WIIU_LIFECYCLE_DIAGNOSTICS WIIU_COMPAT_DIAGNOSTICS; do
    grep -q -- "-D$flag" "$WIIU_BUILD_DIR/config.mk"
done
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/COMPAT-DIAGNOSTIC-20261010.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/COMPAT-DIAGNOSTIC-20261010.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/COMPAT-DIAGNOSTIC-20261010.md" "$wiiu_script_dir/build-compat-diagnostic.sh" \
        "$wiiu_script_dir/meta-rpx-compat-diagnostic.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-compat-diagnostic.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
