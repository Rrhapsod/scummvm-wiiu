#!/usr/bin/env python3
"""Actual KPAD held-state reconciliation with Pro/Classic/Remote mocks."""
from pathlib import Path
import subprocess
import sys
import tempfile

source = Path(sys.argv[1]).read_text()
start = source.index('\t\tuint32_t held = kpad.hold;')
end = source.index('\n\n\t\tswitch (ext)', start)
actual = source[start:end]
prefix = r'''
#include <cassert>
#include <cstdint>
#include <cstdio>
using Uint8 = uint8_t;
using SDL_bool = bool;
const bool SDL_TRUE = true, SDL_FALSE = false;
enum { SDL_RELEASED, SDL_PRESSED };
enum { WPAD_EXT_CORE, WPAD_EXT_MPLUS, WPAD_EXT_PRO_CONTROLLER, WPAD_EXT_CLASSIC, WPAD_EXT_MPLUS_CLASSIC, WPAD_EXT_NUNCHUK, WPAD_EXT_MPLUS_NUNCHUK };
const uint32_t wiimote_button_map[] = {1, 2};
const uint32_t pro_button_map[] = {4, 8};
const uint32_t classic_button_map[] = {16, 32};
const uint32_t nunchuk_button_map[] = {1, 1 << 16};
struct KPADStatus { uint32_t hold = 0; struct { uint32_t hold = 0; } pro, classic, nunchuk; };
struct SDL_Joystick { int nbuttons = 2, naxes = 4; int buttons[2] = {}; int axes[4] = {1, 1, 1, 1}; };
bool keyboard = false;
bool SDL_WiiUSetSWKBDKPAD(int, const void *) { return keyboard; }
int WIIU_WPAD_CHAN(int d) { return d; }
void SDL_PrivateJoystickButton(SDL_Joystick *j, Uint8 i, int s) { j->buttons[i] = s; }
void SDL_PrivateJoystickAxis(SDL_Joystick *j, Uint8 i, int s) { j->axes[i] = s; }
void WIIU_ReleaseRemotePointer(int) {}
void WIIU_UpdateRemotePointer(int, const KPADStatus *, uint32_t) {}
void update(SDL_Joystick *joystick, KPADStatus kpad, int ext, int channel) {
    static uint32_t swkbd_buttons[4] = {};
    static SDL_bool swkbd_was_open[4] = {};
    int wiiu_device = channel;
'''
checks = r'''
}
int main() {
    int channel = 0;
    for (int ext : {WPAD_EXT_PRO_CONTROLLER, WPAD_EXT_CLASSIC, WPAD_EXT_CORE, WPAD_EXT_NUNCHUK}) {
        SDL_Joystick j;
        KPADStatus down;
        down.hold = 1; down.pro.hold = 4; down.classic.hold = 16;
        keyboard = false; update(&j, down, ext, channel); assert(j.buttons[0] == SDL_PRESSED);
        keyboard = true; update(&j, down, ext, channel);
        assert(!j.buttons[0] && !j.axes[0] && !j.axes[3]);
        update(&j, {}, ext, channel); // original click released inside keyboard
        keyboard = false; update(&j, down, ext, channel); // OK consumed by independent keyboard poll
        assert(!j.buttons[0]);
        update(&j, down, ext, channel); assert(!j.buttons[0]);
        update(&j, {}, ext, channel); update(&j, down, ext, channel); assert(j.buttons[0]);
        update(&j, {}, ext, channel); assert(!j.buttons[0]);
        ++channel;
    }
    puts("PASS: actual Pro/Classic/Remote/Nunchuk button reconciliation, axis release, no OK leakage, independent channel masks (host mocks)");
}
'''
with tempfile.TemporaryDirectory(prefix='swkbd-joystick-') as temp:
    file = Path(temp) / 'test.cpp'
    file.write_text('#include <initializer_list>\n' + prefix + actual + checks)
    binary = file.with_suffix('')
    subprocess.run(['g++', '-std=c++11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-no-pie', str(file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
