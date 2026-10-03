#!/usr/bin/env bash
# Reuses the expanded feature set; keeps its artifacts untouched.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-renderer-fix-20261002}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_LIFECYCLE_DIAGNOSTICS"
bash "$wiiu_script_dir/build-expanded.sh" "$@"
grep -q -- '-DWIIU_LIFECYCLE_DIAGNOSTICS' "$WIIU_BUILD_DIR/config.mk"
for wiiu_stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/DIAGNOSTIC.md" "$WIIU_BUILD_DIR/$wiiu_stage/DIAGNOSTIC.md"
    cp "$wiiu_script_dir/RENDERER_FIX.md" "$WIIU_BUILD_DIR/$wiiu_stage/RENDERER_FIX.md"
done
