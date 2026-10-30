#!/usr/bin/env bash
# Rebuild SDL in a new USB-capable SDK profile; never modify the base/current.
set -euo pipefail
: "${DEVKITPRO:?Activate the USB SDK described in USB-SDK.md first}"
: "${WIIU_POINTER_SDK_DEST:?Choose a new absolute SDK profile directory}"
: "${WIIU_POINTER_SOURCES:?Choose a new absolute shared source/build directory}"
: "${WIIU_POINTER_SDL_REPO:?Path to a devkitPro/SDL Git checkout containing a8f1e43a}"
wiiu_pointer_base="$(cd "$DEVKITPRO" && pwd -P)"
wiiu_pointer_script="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
[[ "$WIIU_POINTER_SDK_DEST" = /* && "$WIIU_POINTER_SOURCES" = /* ]] || { echo 'Use absolute destinations' >&2; exit 1; }
[[ ! -e "$WIIU_POINTER_SDK_DEST" && ! -e "$WIIU_POINTER_SOURCES" ]] || { echo 'Destinations must be absent' >&2; exit 1; }
[[ -f "$wiiu_pointer_base/wut/usr/lib/libfat.a" && -f "$wiiu_pointer_base/wut/usr/lib/libmocha.a" ]] || { echo 'Base must include the USB dependencies' >&2; exit 1; }
wiiu_pointer_revision=a8f1e43a4a9d70cbae397433e1a3dd40113679aa
git -C "$WIIU_POINTER_SDL_REPO" cat-file -e "$wiiu_pointer_revision^{commit}"
cp -a --reflink=auto "$wiiu_pointer_base" "$WIIU_POINTER_SDK_DEST"
mkdir -p "$WIIU_POINTER_SOURCES/SDL"
git -C "$WIIU_POINTER_SDL_REPO" archive "$wiiu_pointer_revision" | tar -x -C "$WIIU_POINTER_SOURCES/SDL"
for wiiu_pointer_patch in sdl2-swkbd-input sdl2-swkbd-lifecycle sdl2-renderer-lifecycle \
    sdl2-timer-zero-timeout sdl2-swkbd-controller sdl2-rgba565-blit sdl2-wiimote-pointer sdl2-wiimote-precision; do
    patch -d "$WIIU_POINTER_SOURCES/SDL" -p1 --batch --forward < "$wiiu_pointer_script/$wiiu_pointer_patch.patch"
done
python3 - "$wiiu_pointer_base" "$WIIU_POINTER_SDK_DEST" "$WIIU_POINTER_SOURCES/SDL" <<'PY'
from pathlib import Path
import sys
base,dest,source=map(Path,sys.argv[1:])
for p in dest.rglob('*.la'):
    p.write_text(p.read_text().replace(str(base),str(dest)))
# Existing WUT compatibility adjustment, separate from the eight SDL patches.
p=source/'src/joystick/wiiu/SDL_wiiujoystick.c'
s=p.read_text()
assert s.count('int32_t err;') == 1
p.write_text(s.replace('int32_t err;','KPADError err;'))
PY
export DEVKITPRO="$(cd "$WIIU_POINTER_SDK_DEST" && pwd -P)"
export DEVKITPPC="$DEVKITPRO/devkitPPC" WUT_ROOT="$DEVKITPRO/wut"
export PATH="$DEVKITPPC/bin:$DEVKITPRO/tools/bin:$PATH"
powerpc-eabi-cmake -S "$WIIU_POINTER_SOURCES/SDL" -B "$WIIU_POINTER_SOURCES/build" \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$DEVKITPRO/portlibs/wiiu" \
    -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST=OFF
cmake --build "$WIIU_POINTER_SOURCES/build" -j"${JOBS:-2}"
cmake --install "$WIIU_POINTER_SOURCES/build"
python3 - "$DEVKITPRO" "$wiiu_pointer_base" "$wiiu_pointer_script" <<'PY'
from pathlib import Path
import datetime,hashlib,json,os,sys
dest,base,backend=map(Path,sys.argv[1:])
m=json.loads((base/'MANIFEST.json').read_text())
m.update(profile=dest.name,build_date=datetime.date.today().isoformat())
m['versions']['SDL2']='2.32.7 + eight local patches (pixel-precision IR)'
m['provenance']['base_profile']=str(base)
m['provenance']['pointer_precision']={
    'SDL2_repository':'https://github.com/devkitPro/SDL.git',
    'SDL2_commit':'a8f1e43a4a9d70cbae397433e1a3dd40113679aa',
    'patches':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(backend.glob('sdl2-*.patch'))},
    'compatibility':'KPADReadEx error type KPADError, as in the base profile',
    'recipe':'backends/platform/sdl/wiiu/build-pointer-sdk.sh'}
m['files']={str(p.relative_to(dest)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(dest.rglob('*')) if p.is_file() and not p.is_symlink() and p!=dest/'MANIFEST.json'}
m['symlinks']={str(p.relative_to(dest)):os.readlink(p) for p in sorted(dest.rglob('*')) if p.is_symlink()}
(dest/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n')
print('Prepared independent SDK:',dest,'; shared default unchanged')
PY
