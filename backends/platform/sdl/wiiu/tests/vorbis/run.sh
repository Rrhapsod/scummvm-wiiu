#!/usr/bin/env bash
# Native build only. Pass the directory holding the verified Xiph sources.
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
codec_dir="$(cd -- "${1:?Pass the codec source directory}" && pwd)"
test_build="$(mktemp -d -t scummvm-vorbis-test.XXXXXXXX)"
printf 'Host codec test directory: %s\n' "$test_build"
cmake -S "$codec_dir/libogg-1.3.6" -B "$test_build/ogg" \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX="$test_build/installed"
cmake --build "$test_build/ogg" -j2
cmake --install "$test_build/ogg"
cmake -S "$codec_dir/libvorbis-1.3.7" -B "$test_build/vorbis" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
    -DOGG_INCLUDE_DIR="$test_build/installed/include" -DOGG_LIBRARY="$test_build/ogg/libogg.a"
cmake --build "$test_build/vorbis" -j2
cc -I"$codec_dir/libogg-1.3.6/include" -I"$test_build/ogg/include" \
    -I"$codec_dir/libvorbis-1.3.7/include" -I"$codec_dir/libvorbis-1.3.7/test" \
    "$test_dir/test.c" "$codec_dir/libvorbis-1.3.7/test/write_read.c" \
    "$test_build/vorbis/lib/libvorbisfile.a" "$test_build/vorbis/lib/libvorbisenc.a" \
    "$test_build/vorbis/lib/libvorbis.a" "$test_build/ogg/libogg.a" -lm -o "$test_build/test"
cd "$test_build"
./test
