#!/usr/bin/env bash
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
src_dir="$(cd "$test_dir/../../../../../.." && pwd)"
sdl_src="${1:?Usage: run.sh /path/to/patched/SDL}"
test_tmp="$(mktemp -d)"
trap 'rm -rf "$test_tmp"' EXIT
flags=(-std=c++11 -Wall -Wextra -Werror -g -fsanitize=address,undefined -fno-omit-frame-pointer -no-pie)
"${CXX:-g++}" "${flags[@]}" -DWIIU_REMOTE_TEST \
    -I"$test_dir/../include" -I"$test_dir/.." -I"$src_dir" \
    "$test_dir/controllers.cpp" "$src_dir/backends/platform/sdl/wiiu/wiiu-events.cpp" \
    -o "$test_tmp/controllers"
"$test_tmp/controllers"
python3 - "$sdl_src/src/joystick/wiiu/SDL_wiiujoystick.c" "$test_tmp/suspend.inc" <<'PY'
from pathlib import Path
import sys
source = Path(sys.argv[1]).read_text()
parts = []
for signature in ['void WIIU_SuspendRemotePointers(void)\n{', 'static int WIIU_JoystickGetCount(void)\n{']:
    start = source.index(signature)
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    parts.append(source[start:end])
Path(sys.argv[2]).write_text('\n'.join(parts))
PY
"${CXX:-g++}" "${flags[@]}" -I"$test_tmp" -I"$sdl_src/src/joystick/wiiu" \
    "$test_dir/pointer.cpp" -o "$test_tmp/pointer"
"$test_tmp/pointer"
