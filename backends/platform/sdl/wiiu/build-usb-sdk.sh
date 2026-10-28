#!/usr/bin/env bash
# Add pinned FAT USB dependencies to an independent SDK profile; never switch current.
set -euo pipefail
: "${WIIU_USB_SDK_DEST:?Choose a new absolute SDK profile directory}"
: "${WIIU_USB_SOURCES:?Choose a new absolute shared source-cache directory}"
: "${DEVKITPRO:?Activate a complete base SDK first}"
wiiu_usb_base="$(cd "$DEVKITPRO" && pwd -P)"
wiiu_usb_script="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
[[ "$WIIU_USB_SDK_DEST" = /* && "$WIIU_USB_SOURCES" = /* ]] || { echo 'Use absolute paths' >&2; exit 1; }
[[ ! -e "$WIIU_USB_SDK_DEST" && ! -e "$WIIU_USB_SOURCES" ]] || { echo 'Both destinations must be absent' >&2; exit 1; }
cp -a --reflink=auto "$wiiu_usb_base" "$WIIU_USB_SDK_DEST"
mkdir -p "$WIIU_USB_SOURCES"
export DEVKITPRO="$(cd "$WIIU_USB_SDK_DEST" && pwd -P)"
export DEVKITPPC="$DEVKITPRO/devkitPPC" WUT_ROOT="$DEVKITPRO/wut"
export PATH="$DEVKITPPC/bin:$DEVKITPRO/tools/bin:$PATH"
python3 - "$wiiu_usb_base" "$DEVKITPRO" <<'PY'
from pathlib import Path
import sys
old,new=sys.argv[1:]
for p in Path(new).rglob('*.la'):
    s=p.read_text();p.write_text(s.replace(old,new))
PY
for wiiu_usb_lib in libmocha libfat; do
    git clone "https://github.com/wiiu-env/$wiiu_usb_lib.git" "$WIIU_USB_SOURCES/$wiiu_usb_lib"
    if [[ "$wiiu_usb_lib" = libmocha ]]; then
        wiiu_usb_revision=50fefdf8307a875c63bdbdcf6c973779d4ddac92
    else
        wiiu_usb_revision=6517ae03e07388c6b01d05baf245772bdf7b476e
    fi
    git -C "$WIIU_USB_SOURCES/$wiiu_usb_lib" checkout --detach "$wiiu_usb_revision"
    git -C "$WIIU_USB_SOURCES/$wiiu_usb_lib" apply "$wiiu_usb_script/$wiiu_usb_lib-wiiu-usb.patch"
done
make -C "$WIIU_USB_SOURCES/libmocha" -j"${JOBS:-2}"
mkdir -p "$WUT_ROOT/usr/include" "$WUT_ROOT/usr/lib"
cp -R "$WIIU_USB_SOURCES/libmocha/include/mocha" "$WUT_ROOT/usr/include/"
cp "$WIIU_USB_SOURCES/libmocha/lib/libmocha.a" "$WUT_ROOT/usr/lib/"
make -C "$WIIU_USB_SOURCES/libfat" -j"${JOBS:-2}" wiiu-release
cp "$WIIU_USB_SOURCES/libfat/include/fat.h" "$WIIU_USB_SOURCES/libfat/include/libfatversion.h" "$WUT_ROOT/usr/include/"
cp "$WIIU_USB_SOURCES/libfat/wiiu/lib/libfat.a" "$WUT_ROOT/usr/lib/"
python3 - "$DEVKITPRO" "$wiiu_usb_script" "$WIIU_USB_SOURCES" "$wiiu_usb_base" <<'PY'
from pathlib import Path
import datetime,hashlib,json,os,subprocess,sys
root,backend,sources,base=map(Path,sys.argv[1:])
m=json.loads((base/'MANIFEST.json').read_text()) if (base/'MANIFEST.json').exists() else {'versions':{},'provenance':{}}
m.update(profile=root.name,build_date=datetime.date.today().isoformat())
m['provenance']['base_profile']=str(base)
for name in ('libfat','libmocha'):
    m['versions'][name]='Pinned source + local Wii U USB patch (see USB-SDK.md)'
    m['provenance'][name]={'commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=sources/name,text=True).strip(),
                         'patch_sha256':hashlib.sha256((backend/(name+'-wiiu-usb.patch')).read_bytes()).hexdigest()}
m['files']={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(root.rglob('*')) if p.is_file() and not p.is_symlink() and p!=root/'MANIFEST.json'}
m['symlinks']={str(p.relative_to(root)):os.readlink(p) for p in sorted(root.rglob('*')) if p.is_symlink()}
(root/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n')
print('Prepared independent SDK:',root,'; shared default unchanged')
PY
