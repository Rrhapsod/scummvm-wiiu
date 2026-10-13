#!/usr/bin/env bash
set -euo pipefail
wiiu_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
wiiu_src="$(cd "$wiiu_script_dir/../../../.." && pwd)"
export WIIU_BUILD_DIR="${WIIU_BUILD_DIR:-$wiiu_src/../build-scummvm-wiiu-media-test-20261013}"
export CXXFLAGS="${CXXFLAGS:-} -DWIIU_MEDIA_TEST"
bash "$wiiu_script_dir/build-io-fix.sh" --enable-ogg --enable-vorbis --disable-tremor \
    --with-ogg-prefix="$DEVKITPRO/portlibs/ppc" --with-vorbis-prefix="$DEVKITPRO/portlibs/ppc" "$@"
for flag in USE_OGG USE_VORBIS; do
    grep -Eq "^$flag[[:space:]]*=" "$WIIU_BUILD_DIR/config.mk"
done
for stage in wiiu_release wiiu_rpx_release; do
    cp "$wiiu_script_dir/MEDIA-TEST-20261013.md" "$WIIU_BUILD_DIR/$stage/TESTING.md"
    cp "$wiiu_script_dir/MEDIA-TEST-20261013.md" "$WIIU_BUILD_DIR/$stage/RELEASE-NOTES.md"
    cp "$wiiu_script_dir/MEDIA-TEST-20261013.md" "$wiiu_script_dir/build-media-test.sh" \
        "$wiiu_script_dir/build-vorbis-sdk.sh" "$wiiu_script_dir/meta-rpx-media-test.xml" \
        "$WIIU_BUILD_DIR/$stage/scummvm/doc/backends/platform/sdl/wiiu/"
done
cp "$wiiu_script_dir/meta-rpx-media-test.xml" "$WIIU_BUILD_DIR/wiiu_rpx_release/wiiu/apps/scummvm/meta.xml"
