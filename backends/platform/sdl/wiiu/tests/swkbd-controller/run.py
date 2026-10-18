#!/usr/bin/env python3
"""Actual SDL input-selection functions with host mocks, not native UI emulation."""
from pathlib import Path
import subprocess
import sys
import tempfile

source = Path(sys.argv[1]).read_text()
def function(name):
    pos = source.index(name + "(")
    begin = source.rfind('\n', 0, pos) + 1
    end = source.index('{', pos) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[begin:end]

prefix = r'''
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <optional>
#define SDL_fabsf std::fabs
enum { WPAD_EXT_PRO_CONTROLLER, WPAD_EXT_CLASSIC, WPAD_EXT_MPLUS_CLASSIC, WPAD_EXT_CORE };
using WPADChan = int;
using WPADExtensionType = int;
enum { VPAD_CHAN_0, VPAD_READ_SUCCESS = 0, VPAD_READ_NO_SAMPLES = 1, KPAD_ERROR_OK = 0 };
using VPADReadError = int;
using KPADError = int;
using SDL_bool = bool;
const bool SDL_TRUE = true, SDL_FALSE = false;
struct Stick { float x = 0, y = 0; };
struct VPADStatus { unsigned trigger = 0; struct { bool touched = false; } tpNormal; };
struct KPADStatus {
    int extensionType = WPAD_EXT_CORE;
    unsigned trigger = 0;
    struct { unsigned trigger = 0; Stick leftStick; } pro, classic;
};
namespace nn { namespace swkbd {
enum class ControllerType { WiiRemote0, WiiRemote1, WiiRemote2, WiiRemote3, DrcGamepad };
enum class State { Hidden, FadeIn, Visible, FadeOut };
struct ControllerInfo { VPADStatus *vpad = nullptr; KPADStatus *kpad[4] = {}; };
State state = State::Hidden;
State GetStateInputForm() { return state; }
} }
namespace detail {
namespace create { bool created = false; }
nn::swkbd::ControllerType activeController = nn::swkbd::ControllerType::DrcGamepad;
nn::swkbd::ControllerInfo controllerInfo;
VPADStatus vpad;
std::array<KPADStatus, 4> kpad;
}
bool vAvailable = false;
bool kNoSamples = false;
std::array<bool, 4> kAvailable = {};
VPADStatus nextV;
std::array<KPADStatus, 4> nextK;
int vReads = 0, calibrated = 0;
std::array<int, 4> kReads = {};
int VPADRead(int, VPADStatus *s, int, int *error) {
    ++vReads; *error = vAvailable ? 0 : -1;
    if (vAvailable) *s = nextV;
    return vAvailable ? 1 : 0;
}
void VPADGetTPCalibratedPoint(int, void *, void *) { ++calibrated; }
int KPADReadEx(int c, KPADStatus *s, int, int *error) {
    if (kNoSamples) { *error = 1; return 0; }
    ++kReads[c]; *error = kAvailable[c] ? 0 : -1;
    if (kAvailable[c]) *s = nextK[c];
    return kAvailable[c] ? 1 : 0;
}
int WPADProbe(int c, int *extension) { *extension = nextK[c].extensionType; return kAvailable[c] ? 0 : -1; }
'''
checks = r'''
int main() {
    using nn::swkbd::ControllerType;
    using nn::swkbd::State;
    // No GamePad, only Pro on channel 3, not opened by SDL.
    kAvailable[3] = true;
    nextK[3].extensionType = WPAD_EXT_PRO_CONTROLLER;
    detail::collectKeyboardInput();
    assert(detail::activeController == ControllerType::WiiRemote3);
    assert(detail::controllerInfo.kpad[3] && !detail::controllerInfo.vpad);
    // A supplied opened-controller sample must not be read twice.
    int reads = kReads[3];
    detail::collectKeyboardInput(); assert(kReads[3] == reads);
    // A GamePad connected later can take over by touch.
    detail::controllerInfo = {}; vAvailable = true; nextV.tpNormal.touched = true;
    detail::collectKeyboardInput();
    assert(detail::activeController == ControllerType::DrcGamepad && calibrated);
    // Pro/Classic input can take over even while the GamePad stays connected.
    nextV.tpNormal.touched = false; nextK[3].pro.trigger = 1;
    detail::controllerInfo = {}; detail::collectKeyboardInput();
    assert(detail::activeController == ControllerType::WiiRemote3);
    nextK[3].pro.trigger = 0; nextK[3].extensionType = WPAD_EXT_CLASSIC;
    nextK[3].classic.leftStick.x = 0.8f;
    assert(detail::kpadActive(nextK[3]));
    kNoSamples = true; detail::controllerInfo = {};
    detail::collectKeyboardInput();
    assert(detail::activeController == ControllerType::WiiRemote3); // connected, no fresh sample
    kNoSamples = false;
    // No replay of stale pointers when a device has no sample/disconnects.
    detail::controllerInfo = {}; vAvailable = false; kAvailable = {};
    detail::collectKeyboardInput();
    assert(!detail::controllerInfo.vpad && !detail::controllerInfo.kpad[3]);
    // Remember the controller before creation; consume throughout animations.
    KPADStatus pro; pro.extensionType = WPAD_EXT_PRO_CONTROLLER; pro.pro.trigger = 1;
    assert(!SDL_WiiUSetSWKBDKPAD(2, &pro));
    assert(detail::activeController == ControllerType::WiiRemote2);
    detail::create::created = true;
    for (State s : {State::FadeIn, State::Visible, State::FadeOut}) {
        nn::swkbd::state = s;
        assert(SDL_WiiUSetSWKBDKPAD(2, &pro));
        VPADStatus v; v.tpNormal.touched = true;
        assert(SDL_WiiUSetSWKBDVPAD(&v));
        assert(detail::activeController == ControllerType::DrcGamepad);
    }
    nn::swkbd::state = State::Hidden;
    assert(!SDL_WiiUSetSWKBDKPAD(2, &pro));
    assert(!SDL_WiiUSetSWKBDKPAD(-1, nullptr) && !SDL_WiiUSetSWKBDKPAD(4, nullptr));
    puts("PASS: actual SDL controller selection, unopened Pro channel, late GamePad, sample ownership, no-sample/disconnect, Classic, fade suppression (host mocks)");
}
'''
assert 'nn::swkbd::SetControllerRemo(detail::activeController)' in function('WIIU_SWKBD_Calc')
assert 'arg.keyboardArg.configArg.controllerType = detail::activeController' in function('WIIU_SWKBD_ShowScreenKeyboard')
suspend = function('WIIU_SWKBD_Suspend')
assert suspend.index('SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT') < suspend.index('WIIU_SWKBD_Finalize();')
video = Path(sys.argv[1]).with_name('SDL_wiiuvideo.c').read_text()
release = video[video.index('static int WIIU_ForegroundReleased'):video.index('static Uint32 WIIU_GetWindowPixelFormat') if 'static Uint32 WIIU_GetWindowPixelFormat' in video else len(video)]
assert release.index('WIIU_SWKBD_Suspend();') < release.index('WIIU_GfxHeap_MEM1Destroy();')
actual = '\nnamespace detail {\n' + function('kpadActive') + function('collectKeyboardInput') + '\n}\n'
actual += function('SDL_WiiUSetSWKBDVPAD') + function('SDL_WiiUSetSWKBDKPAD')
with tempfile.TemporaryDirectory(prefix='swkbd-controller-') as temp:
    file = Path(temp) / 'test.cpp'
    file.write_text(prefix + actual + checks)
    binary = file.with_suffix('')
    subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-no-pie', str(file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
