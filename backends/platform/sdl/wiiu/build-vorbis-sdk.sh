#!/usr/bin/env bash
# Pinned Xiph sources; hashes match the inspected devkitPro PPC recipes.
set -euo pipefail
: "${DEVKITPRO:?Activate the prepared Wii U SDK first}"
wiiu_codec_dir="${WIIU_CODEC_DIR:?Set WIIU_CODEC_DIR to the codec source/build directory}"
mkdir -p "$wiiu_codec_dir"
cd "$wiiu_codec_dir"
for spec in \
    'ogg libogg 1.3.6 83e6704730683d004d20e21b8f7f55dcb3383cdf84c0daedf30bde175f774638' \
    'vorbis libvorbis 1.3.7 0e982409a9c3fc82ee06e08205b1355e5c6aa4c36bca58146ef399621b0ce5ab'; do
    read -r category library version digest <<< "$spec"
    archive="$library-$version.tar.gz"
    if [[ ! -f "$archive" ]]; then
        curl --fail --location --retry 2 "https://downloads.xiph.org/releases/$category/$archive" -o "$archive"
    fi
    [[ "$(sha256sum "$archive" | cut -d ' ' -f 1)" == "$digest" ]]
    if [[ ! -d "$library-$version" ]]; then
        tar -xzf "$archive"
    fi
    options=()
    if [[ "$library" == libogg ]]; then
        options=(-DBUILD_TESTING=OFF -DINSTALL_DOCS=OFF)
    else
        options=(-DOGG_INCLUDE_DIR="$DEVKITPRO/portlibs/ppc/include" -DOGG_LIBRARY="$DEVKITPRO/portlibs/ppc/lib/libogg.a")
    fi
    powerpc-eabi-cmake -S "$library-$version" -B "$library-$version/build-wiiu" \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$DEVKITPRO/portlibs/ppc" -DBUILD_SHARED_LIBS=OFF "${options[@]}"
    cmake --build "$library-$version/build-wiiu" -j"${JOBS:-2}"
    cmake --install "$library-$version/build-wiiu"
    install -Dm644 "$library-$version/COPYING" "$DEVKITPRO/portlibs/ppc/licenses/$library/COPYING"
done
