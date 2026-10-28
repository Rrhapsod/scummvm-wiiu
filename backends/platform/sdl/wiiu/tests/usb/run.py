#!/usr/bin/env python3
"""Exercise actual USB owner with mocks; no USB hardware/driver claims."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
with tempfile.TemporaryDirectory(prefix='scummvm-usb-') as temp:
    p = Path(temp)
    (p/'mocha').mkdir(); (p/'sys').mkdir()
    (p/'fat.h').write_text('''#pragma once
struct DISC_INTERFACE { bool (*shutdown)(); };
extern DISC_INTERFACE Mocha_usb_disc_interface;
bool fatMount(const char *, const DISC_INTERFACE *, unsigned, unsigned, unsigned);
void fatUnmount(const char *);
''')
    (p/'mocha/mocha.h').write_text('''#pragma once
#define MOCHA_RESULT_SUCCESS 0
int Mocha_InitLibrary();
int Mocha_DeInitLibrary();
''')
    (p/'sys/iosupport.h').write_text('int FindDevice(const char *);\n')
    (p/'test.cpp').write_text(r'''
#include <cassert>
#include <cstring>
#include <vector>
#include "fat.h"
#include "backends/platform/sdl/wiiu/wiiu-usb.h"
static int device = -1, mocha = 0;
static bool mountOK = true;
static std::vector<int> calls;
int FindDevice(const char *s) { assert(!strcmp(s,"usb:")); calls.push_back(1); return device; }
int Mocha_InitLibrary() { calls.push_back(2); return mocha; }
int Mocha_DeInitLibrary() { calls.push_back(6); return 0; }
bool shutdown() { calls.push_back(5); return true; }
DISC_INTERFACE Mocha_usb_disc_interface = {shutdown};
bool fatMount(const char *s, const DISC_INTERFACE *d, unsigned start, unsigned pages, unsigned sectors) {
 assert(!strcmp(s,"usb") && d == &Mocha_usb_disc_interface);
 assert(start == 0 && pages * sectors * 512 == 131072); calls.push_back(3); return mountOK;
}
void fatUnmount(const char *s) { assert(!strcmp(s,"usb:")); calls.push_back(4); }
int main() {
 { WiiUUSBStorage usb; assert(!usb.root()); }
 assert(calls.empty());
 { device=4; WiiUUSBStorage usb; usb.mount(); usb.mount(); assert(!usb.root()); }
 assert((calls == std::vector<int>{1})); calls.clear(); device=-1;
 { mocha=-1; WiiUUSBStorage usb; usb.mount(); assert(!usb.root()); }
 assert((calls == std::vector<int>{1,2})); calls.clear(); mocha=0;
 { mountOK=false; WiiUUSBStorage usb; usb.mount(); usb.mount(); assert(!usb.root()); }
 assert((calls == std::vector<int>{1,2,3,5,6})); calls.clear(); mountOK=true;
 { WiiUUSBStorage usb; usb.mount(); usb.mount(); assert(!strcmp(usb.root(),"usb:/"));
   // Represents engine/save/mixer teardown while the mount still exists.
   calls.push_back(99);
 }
 assert((calls == std::vector<int>{1,2,3,99,4,5,6}));
}
''')
    out=p/'test'
    subprocess.run(['g++','-std=c++11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-no-pie',
                    '-I'+str(p),'-I'+str(root),str(p/'test.cpp'),
                    str(root/'backends/platform/sdl/wiiu/wiiu-usb.cpp'),'-o',str(out)], check=True)
    subprocess.run([str(out)],check=True)
main=(root/'backends/platform/sdl/wiiu/wiiu-main.cpp').read_text()
assert main.index('WiiUUSBStorage usb;') < main.index('new OSystem_WiiU(usb)') < main.index('g_system->destroy();')
backend=(root/'backends/platform/sdl/wiiu/wiiu.cpp').read_text()
assert 'if (_usb.root())\n\t\tfactory->addDrive(_usb.root());' in backend
assert 'new DefaultSaveFileManager(Common::Path(kDataRoot).join("saves"))' in backend
print('PASS: actual USB owner, unavailable API/device, name collision, partial failure, repeat mount, flush/close order and SD/save defaults (host mocks)')
