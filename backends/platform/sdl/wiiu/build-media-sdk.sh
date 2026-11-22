#!/usr/bin/env bash
# Add media libraries in an independent SDK; keep the base/current unchanged.
set -euo pipefail
: "${DEVKITPRO:?Activate the stabilized Pointer/USB SDK first}"
: "${WIIU_MEDIA_SDK_DEST:?Choose a new absolute SDK profile directory}"
: "${WIIU_MEDIA_SOURCES:?Choose an absolute shared source/build directory}"
wiiu_media_base="$(cd "$DEVKITPRO" && pwd -P)"
wiiu_media_script="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
[[ "$WIIU_MEDIA_SDK_DEST" = /* && "$WIIU_MEDIA_SOURCES" = /* ]] || exit 1
[[ ! -e "$WIIU_MEDIA_SDK_DEST" ]] || { echo 'SDK destination must be absent' >&2; exit 1; }
[[ -f "$WUT_ROOT/usr/lib/libfat.a" && -f "$WUT_ROOT/usr/lib/libmocha.a" ]] || exit 1
powerpc-eabi-strings "$DEVKITPRO/portlibs/wiiu/lib/libSDL2.a" | grep 'WiiU pointer: adaptive-stabilized IR' > /dev/null
cp -a --reflink=auto "$wiiu_media_base" "$WIIU_MEDIA_SDK_DEST"
export DEVKITPRO="$WIIU_MEDIA_SDK_DEST" DEVKITPPC="$WIIU_MEDIA_SDK_DEST/devkitPPC" WUT_ROOT="$WIIU_MEDIA_SDK_DEST/wut"
export PATH="$DEVKITPRO/tools/bin:$DEVKITPPC/bin:$DEVKITPRO/portlibs/wiiu/bin:$DEVKITPRO/portlibs/ppc/bin:$PATH"
export PKG_CONFIG_LIBDIR="$DEVKITPRO/portlibs/wiiu/lib/pkgconfig:$DEVKITPRO/portlibs/ppc/lib/pkgconfig" PKG_CONFIG_PATH=
python3 - "$WIIU_MEDIA_SOURCES" "$wiiu_media_script/media-sources.json" "$wiiu_media_base" "$DEVKITPRO" <<'PY'
from pathlib import Path
import hashlib,json,re,sys,tarfile,urllib.request,zipfile
root,lock,base,dest=map(Path,sys.argv[1:])
(root/'archives').mkdir(parents=True,exist_ok=True)
for pattern in ('*.la','*.pc'):
    for p in dest.rglob(pattern):
        s=p.read_text().replace(str(base),str(dest))
        s=re.sub(r'/[^\s;"\x27]+/sdk/versions/[^/\s;"\x27]+(?=/)',str(dest),s)
        p.write_text(s)
for name,spec in json.loads(lock.read_text()).items():
    archive=root/'archives'/name
    if not archive.exists():
        urllib.request.urlretrieve(spec['url'],archive)
    if hashlib.sha256(archive.read_bytes()).hexdigest()!=spec['sha256']:
        raise SystemExit('Source hash mismatch: '+name)
    if name.endswith('.zip'):
        with zipfile.ZipFile(archive) as z: z.extractall(root)
    else:
        with tarfile.open(archive) as t: t.extractall(root,filter='data')
PY
patch -d "$WIIU_MEDIA_SOURCES/flac-1.5.0" -p1 --batch --forward < "$wiiu_media_script/flac-newlib-int32.patch"
patch -d "$WIIU_MEDIA_SOURCES" -p1 --batch --forward < "$wiiu_media_script/media-zlib-module.patch"
patch -d "$WIIU_MEDIA_SOURCES/fluidsynth-2.5.6" -p1 --batch --forward < "$wiiu_media_script/fluidsynth-wiiu.patch"
cp "$wiiu_media_script"/fluid_sys_wiiu.{c,h} "$WIIU_MEDIA_SOURCES/fluidsynth-2.5.6/src/utils/"
wiiu_media_prefix="$DEVKITPRO/portlibs/ppc"
media_cmake() {
    local lib="$1"; shift
    powerpc-eabi-cmake -S "$WIIU_MEDIA_SOURCES/$lib" -B "$WIIU_MEDIA_SOURCES/build-$lib" \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=OFF \
        -DCMAKE_INSTALL_PREFIX="$wiiu_media_prefix" -DBUILD_SHARED_LIBS=OFF "$@"
    if [[ "$lib" = fluidsynth-* ]]; then
        cmake --build "$WIIU_MEDIA_SOURCES/build-$lib" --target libfluidsynth -j"${JOBS:-2}"
        cmake --install "$WIIU_MEDIA_SOURCES/build-$lib" --component fluidsynth_development
    else
        cmake --build "$WIIU_MEDIA_SOURCES/build-$lib" -j"${JOBS:-2}"
        cmake --install "$WIIU_MEDIA_SOURCES/build-$lib"
    fi
}
media_cmake flac-1.5.0 -DBUILD_CXXLIBS=OFF -DBUILD_PROGRAMS=OFF -DBUILD_EXAMPLES=OFF \
    -DBUILD_TESTING=OFF -DBUILD_DOCS=OFF -DINSTALL_MANPAGES=OFF -DENABLE_MULTITHREADING=OFF \
    -DWITH_STACK_PROTECTOR=OFF
media_cmake libpng-1.6.59 -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DPNG_TOOLS=OFF
media_cmake freetype-2.14.1 -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON \
    -DFT_DISABLE_BROTLI=ON -DFT_REQUIRE_ZLIB=ON -DFT_REQUIRE_PNG=ON
export CC=powerpc-eabi-gcc CXX=powerpc-eabi-g++ AR=powerpc-eabi-ar RANLIB=powerpc-eabi-ranlib
export CFLAGS="-O2 -mcpu=750 -meabi -mhard-float -ffunction-sections -fdata-sections"
export CPPFLAGS="-I$wiiu_media_prefix/include" LDFLAGS="-L$wiiu_media_prefix/lib"
export PKG_CONFIG=powerpc-eabi-pkg-config
for lib in libtheora-1.2.0 libmpeg2-0.5.1; do
    mkdir -p "$WIIU_MEDIA_SOURCES/build-$lib"
    (
        cd "$WIIU_MEDIA_SOURCES/build-$lib"
        options=(--disable-examples --disable-encode --disable-asm)
        if [[ "$lib" = libmpeg2-* ]]; then
            # Espresso has no AltiVec; disable all runtime accelerator probes.
            options=(--disable-sdl --disable-accel-detect)
        fi
        "$WIIU_MEDIA_SOURCES/$lib/configure" --host=powerpc-eabi \
            --prefix="$wiiu_media_prefix" --disable-shared --enable-static "${options[@]}"
        if [[ "$lib" = libmpeg2-* ]]; then
            # Only the decoder/headers; the obsolete command-line tools are
            # unrelated to ScummVM and use pre-C23 function declarations.
            make -C libmpeg2 -j"${JOBS:-2}"
            make -C libmpeg2 install
            make -C include install
        else
            make -j"${JOBS:-2}"
            make install
        fi
    )
done
media_cmake fluidsynth-2.5.6 -Dosal=wiiu -DCMAKE_CXX_STANDARD=17 \
    -DGCEM_INCLUDE_DIR="$WIIU_MEDIA_SOURCES/gcem-012ae73c6d0a2cb09ffe86475f5c6fba3926e200/include" \
    -Denable-alsa=OFF -Denable-aufile=OFF -Denable-dbus=OFF -Denable-ipv6=OFF \
    -Denable-jack=OFF -Denable-ladspa=OFF -Denable-libinstpatch=OFF -Denable-libsndfile=OFF \
    -Denable-midishare=OFF -Denable-network=OFF -Denable-oss=OFF -Denable-dsound=OFF \
    -Denable-wasapi=OFF -Denable-waveout=OFF -Denable-winmidi=OFF -Denable-sdl3=OFF \
    -Denable-pulseaudio=OFF -Denable-pipewire=OFF -Denable-readline=OFF \
    -Denable-threads=OFF -Denable-openmp=OFF -Denable-native-dls=OFF
for spec in 'flac-1.5.0 COPYING.Xiph' 'libpng-1.6.59 LICENSE' \
    'freetype-2.14.1 docs/FTL.TXT' 'freetype-2.14.1 LICENSE.TXT' \
    'libtheora-1.2.0 COPYING' 'libmpeg2-0.5.1 COPYING' 'fluidsynth-2.5.6 LICENSE' \
    'gcem-012ae73c6d0a2cb09ffe86475f5c6fba3926e200 LICENSE'; do
    read -r lib notice <<< "$spec"
    install -Dm644 "$WIIU_MEDIA_SOURCES/$lib/$notice" "$wiiu_media_prefix/licenses/$lib/$(basename "$notice")"
done
python3 - "$DEVKITPRO" "$wiiu_media_base" "$wiiu_media_script" <<'PY'
from pathlib import Path
import hashlib,json,os,re,sys
dest,base,backend=map(Path,sys.argv[1:])
for p in dest.rglob('*.pc'):
    p.write_text(re.sub(r'/[^\s;"\x27]+/sdk/versions/[^/\s;"\x27]+(?=/)',str(dest),p.read_text()))
m=json.loads((base/'MANIFEST.json').read_text())
m.update(profile=dest.name,build_date='2026-11-22')
m['versions'].update({'libFLAC':'1.5.0','libpng':'1.6.59','FreeType':'2.14.1',
    'libtheora':'1.2.0 decode only','libmpeg2':'0.5.1 portable C','FluidSynth':'2.5.6 SDL2 OSAL, no audio/MIDI drivers',
    'GCEM':'012ae73c6d0a2cb09ffe86475f5c6fba3926e200'})
m['provenance']['media']={'base_profile':str(base),'sources':json.loads((backend/'media-sources.json').read_text()),
    'recipe':'backends/platform/sdl/wiiu/build-media-sdk.sh',
    'local_files':{name:hashlib.sha256((backend/name).read_bytes()).hexdigest() for name in
        ('build-media-sdk.sh','flac-newlib-int32.patch','media-zlib-module.patch','fluidsynth-wiiu.patch',
         'fluid_sys_wiiu.c','fluid_sys_wiiu.h')}}
m['files']={str(p.relative_to(dest)):hashlib.sha256(p.read_bytes()).hexdigest()
    for p in sorted(dest.rglob('*')) if p.is_file() and not p.is_symlink() and p!=dest/'MANIFEST.json'}
m['symlinks']={str(p.relative_to(dest)):os.readlink(p) for p in sorted(dest.rglob('*')) if p.is_symlink()}
(dest/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n')
PY
