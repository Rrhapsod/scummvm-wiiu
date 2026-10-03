#!/usr/bin/env bash
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
src_dir="$(cd "$test_dir/../../../../../.." && pwd)"
test_tmp="$(mktemp -d)"
for mode in enabled disabled nonwiiu; do
    flags=()
    case "$mode" in
        enabled) flags=(-DWIIU -DWIIU_LIFECYCLE_DIAGNOSTICS);;
        disabled) flags=(-DWIIU);;
        nonwiiu) flags=(-DWIIU_LIFECYCLE_DIAGNOSTICS);;
    esac
    "${CXX:-g++}" -std=c++11 -Wall -Wextra -Werror -g \
        -fsanitize=address,undefined -fno-omit-frame-pointer "${flags[@]}" \
        -I"$test_dir/include" -I"$src_dir" \
        "$test_dir/test.cpp" "$src_dir/backends/platform/sdl/wiiu/wiiu-diagnostics.cpp" \
        -o "$test_tmp/test-$mode"
    "$test_tmp/test-$mode"
done
