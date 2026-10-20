// Compile the actual SDL pointer helper against deterministic API mocks.
#include <cassert>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <vector>
#include <limits>
using Uint32 = uint32_t;
using SDL_MouseID = Uint32;
using BOOL = int;
using SDL_bool = bool;
const bool SDL_TRUE = true, SDL_FALSE = false;
const int TRUE = 1, FALSE = 0;
const int WIIU_NUM_WPADS = 4;
const int MAX_CONTROLLERS = 5;
int nextDeviceIndex = 0;
int WIIU_GetNextDeviceIndex() { return nextDeviceIndex; }
enum WPADExtensionType { WPAD_EXT_CORE, WPAD_EXT_MPLUS, WPAD_EXT_NUNCHUK, WPAD_EXT_MPLUS_NUNCHUK, WPAD_EXT_CLASSIC, WPAD_EXT_PRO_CONTROLLER };
using KPADChan = int;
const Uint32 WPAD_BUTTON_A = 1, WPAD_BUTTON_B = 2;
const int SDL_WINDOW_SHOWN = 1, SDL_WINDOW_WIIU_GAMEPAD_ONLY = 2;
const int SDL_PRESSED = 1, SDL_RELEASED = 0, SDL_BUTTON_LEFT = 1, SDL_BUTTON_RIGHT = 3, SDL_USEREVENT = 10;
#define SDL_zero(x) std::memset(&(x), 0, sizeof(x))
#define SDL_max(a,b) ((a) > (b) ? (a) : (b))
#define SDL_abs(x) std::abs(x)
#define SDL_TICKS_PASSED(a,b) ((int32_t)((b) - (a)) <= 0)
struct KPADStatus { int posValid; struct { float x, y; } pos; };
struct SDL_Window { int flags, w, h; SDL_Window *next; };
struct SDL_VideoDevice { SDL_Window *windows; };
struct SDL_Event { int type; struct { int code; } user; };
struct MouseEvent { int type, channel, x, y, state, button; };
std::vector<MouseEvent> mouseEvents;
std::vector<int> trackingEvents;
SDL_Window window = {SDL_WINDOW_SHOWN, 1280, 720, nullptr};
SDL_VideoDevice video = {&window};
bool optedIn = false, sensorBar = false;
int enabled[4] = {}, disabled[4] = {};
Uint32 ticks = 0;
int mx = 0, my = 0;
SDL_VideoDevice *SDL_GetVideoDevice() { return &video; }
bool SDL_GetHintBoolean(const char *, bool) { return optedIn; }
void WPADSetSensorBar(BOOL v) { sensorBar = v; }
void KPADEnableDPD(int c) { ++enabled[c]; }
void KPADDisableDPD(int c) { ++disabled[c]; }
void SDL_Log(const char *, ...) {}
Uint32 SDL_GetTicks() { return ticks; }
int SDL_PushEvent(SDL_Event *e) { trackingEvents.push_back(e->user.code); return 1; }
int SDL_SendMouseMotion(SDL_Window *, Uint32 id, int, int x, int y) {
    mx = x; my = y; mouseEvents.push_back({0, (int)(id & 3), x, y, 0, 0}); return 1;
}
int SDL_SendMouseButton(SDL_Window *, Uint32 id, int state, int button) {
    mouseEvents.push_back({1, (int)(id & 3), mx, my, state, button}); return 1;
}
#include "SDL_wiiupointer.h"
#include "suspend.inc"

static KPADStatus sample(float x, float y, int valid = 1) { return {valid, {x, y}}; }
static void update(int c, float x, float y, Uint32 buttons = 0, int valid = 1) {
    const auto kpad = sample(x, y, valid); WIIU_UpdateRemotePointer(c, &kpad, buttons);
}
int main() {
    for (int i = 0; i < MAX_CONTROLLERS; ++i) { nextDeviceIndex = i; assert(WIIU_JoystickGetCount() == i); }
    nextDeviceIndex = -1; assert(WIIU_JoystickGetCount() == MAX_CONTROLLERS);
    WIIU_ConfigureRemotePointer(0, WPAD_EXT_CORE); assert(!enabled[0] && !sensorBar);
    optedIn = true;
    WIIU_ConfigureRemotePointer(0, WPAD_EXT_PRO_CONTROLLER); assert(!enabled[0]);
    WIIU_ConfigureRemotePointer(0, WPAD_EXT_CORE);
    WIIU_ConfigureRemotePointer(0, WPAD_EXT_CORE); assert(enabled[0] == 1 && sensorBar);
    update(0, -1, -1); assert(mx == 0 && my == 0);
    update(0, 1, 1, WPAD_BUTTON_A);
    assert(mouseEvents[mouseEvents.size()-2].type == 0); // absolute move BEFORE press
    assert(mx == 1279 && my == 719 && mouseEvents.back().state == SDL_PRESSED);
    const auto count = mouseEvents.size(); update(0, 1, 1, WPAD_BUTTON_A); assert(mouseEvents.size() == count);
    update(0, 0, 0, WPAD_BUTTON_A); assert(mx == 639 && my == 359); // drag
    update(0, 0, 0); assert(mouseEvents.back().state == SDL_RELEASED);
    update(0, 0.001f, 0.001f); assert(mouseEvents.back().type == 1); // subthreshold jitter
    const auto beforeLost = mouseEvents.size();
    update(0, 0, 0, 0, -1);
    update(0, std::numeric_limits<float>::quiet_NaN(), 0);
    update(0, std::numeric_limits<float>::infinity(), 0);
    update(0, 0, 0, 0, 0);
    assert(mouseEvents.size() == beforeLost);
    assert(trackingEvents.back() == 0x57495200); // negative posValid is NOT valid
    update(0, 0.5f, -0.5f, 0, 2); assert(mx == 959 && my == 179);
    // Touch yields ownership, including samples arriving during cooldown.
    ticks = 1000; WIIU_YieldRemotePointer(); mx = 20; my = 30;
    update(0, 0.5f, -0.5f); assert(mx == 20 && my == 30);
    ticks = 1400; update(0, 0.5f, -0.5f); assert(mx == 20 && my == 30);
    update(0, 0.8f, -0.5f); assert(mx > 1100);
    // An explicit click claims the Remote's position even during cooldown.
    WIIU_YieldRemotePointer(); mx = 100; my = 100;
    update(0, 0.8f, -0.5f, WPAD_BUTTON_B);
    assert(mx > 1100 && mouseEvents.back().button == SDL_BUTTON_RIGHT);
    WIIU_YieldRemotePointer(); assert(remotePointerOwner == 0); // drag owns cursor
    WIIU_ConfigureRemotePointer(1, WPAD_EXT_MPLUS_NUNCHUK);
    update(1, -1, -1, WPAD_BUTTON_A); assert(remotePointerOwner == 0);
    update(0, 0.8f, -0.5f); assert(mouseEvents.back().state == SDL_RELEASED);
    update(1, -1, -1, WPAD_BUTTON_A); assert(remotePointerOwner == 1);
    // Foreground loss releases held clicks; held buttons rearm only on release.
    window.flags = 0; update(1, -1, -1, WPAD_BUTTON_A);
    assert(mouseEvents.back().state == SDL_RELEASED && remotePointers[1].suppressed);
    window.flags = SDL_WINDOW_SHOWN;
    update(1, -1, -1, WPAD_BUTTON_A); assert(!remotePointers[1].buttons);
    update(1, -1, -1); update(1, -1, -1, WPAD_BUTTON_A); assert(remotePointers[1].buttons);
    WIIU_DisableRemotePointer(1); assert(mouseEvents.back().state == SDL_RELEASED && sensorBar);
    WIIU_DisableRemotePointer(0); assert(!sensorBar && disabled[0] == 1 && disabled[1] == 1);
    // Reconnect and changed output resolution use current window dimensions.
    window.w = 1920; window.h = 1080; WIIU_ConfigureRemotePointer(0, WPAD_EXT_MPLUS);
    ticks = 2000; update(0, 1, 1); assert(mx == 1919 && my == 1079);
    window.w = 854; window.h = 480; update(0, -1, -1); assert(mx == 0 && my == 0);
    update(0, 1, 1); assert(mx == 853 && my == 479);
    update(0, 1, 1, WPAD_BUTTON_A); assert(remotePointers[0].buttons);
    WIIU_SuspendRemotePointers(); // actual ProcUI integration entry point
    assert(!remotePointers[0].buttons && mouseEvents.back().state == SDL_RELEASED);
    update(0, 1, 1, WPAD_BUTTON_A); assert(!remotePointers[0].buttons);
    update(0, 1, 1); update(0, 1, 1, WPAD_BUTTON_A); assert(remotePointers[0].buttons);
    update(0, 1, 1);
    // Tick wrap keeps the arbitration timeout bounded.
    ticks = 0xffffff00u; WIIU_YieldRemotePointer(); update(0, 1, 1);
    ticks = 120; update(0, 1, 1); assert(remotePointerOwner == -1);
    update(0, 0, 0); assert(remotePointerOwner == 0);
    optedIn = false; WIIU_ConfigureRemotePointer(0, WPAD_EXT_CORE); assert(!sensorBar);
    puts("PASS: actual SDL IR helper: opt-in, Sensor Bar lifecycle, valid/invalid/NaN samples, hover/click/drag ordering, jitter, touch arbitration, multiple Remotes, unplug/focus release, 480/720/1080 mapping, tick wrap (host mocks)");
}
