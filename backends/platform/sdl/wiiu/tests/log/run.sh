#!/usr/bin/env bash
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
src_dir="$(cd "$test_dir/../../../../../.." && pwd)"
test_tmp="$(mktemp -d)"
"${CXX:-g++}" -std=c++11 -Wall -Wextra -Werror -g \
    -fsanitize=address,undefined -fno-omit-frame-pointer -no-pie \
    -I"$test_dir/include" -I"$src_dir" "$test_dir/test.cpp" \
    "$src_dir/backends/platform/sdl/wiiu/wiiu-log.cpp" -Wl,--wrap=fsync -o "$test_tmp/test"
"$test_tmp/test"
