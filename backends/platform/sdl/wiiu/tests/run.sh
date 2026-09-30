#!/usr/bin/env bash
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
src_dir="$(cd "$test_dir/../../../../.." && pwd)"
test_tmp="$(mktemp -d)"
"${CXX:-g++}" -std=c++11 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -I"$test_dir/include" -I"$test_dir" -I"$src_dir" \
    "$test_dir/test.cpp" "$src_dir/backends/platform/sdl/wiiu/wiiu-events.cpp" \
    -o "$test_tmp/test-keyboard"
"$test_tmp/test-keyboard"
