#!/usr/bin/env bash
# Wii U port 0.6.0: same expanded profile, without lifecycle instrumentation.
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-0.6.0}"
if [[ "${CXXFLAGS:-} ${CPPFLAGS:-} ${CFLAGS:-}" == *WIIU_LIFECYCLE_DIAGNOSTICS* ]]; then
    echo "Release builds must not enable WIIU_LIFECYCLE_DIAGNOSTICS" >&2
    exit 1
fi
bash "$wiiu_script_dir/build-expanded.sh" "$@"
if grep -q -- 'WIIU_LIFECYCLE_DIAGNOSTICS' "$WIIU_BUILD_DIR/config.mk"; then
    echo "Unexpected lifecycle diagnostic flag in release configuration" >&2
    exit 1
fi
