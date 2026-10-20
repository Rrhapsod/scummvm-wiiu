// Host-only test doubles. The test compiles the real Wii U event adapter.
#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <deque>
#include <string>
#include <locale>
#include <codecvt>
using int16 = int16_t;
using uint32 = uint32_t;
using int32 = int32_t;
using uint16 = uint16_t;
namespace Common {
enum KeyCode { KEYCODE_INVALID, KEYCODE_BACKSPACE = 8, KEYCODE_END = 279, KEYCODE_TILDE = 176 };
struct KeyState {
    KeyCode keycode; uint16 ascii; unsigned char flags;
    KeyState(KeyCode k = KEYCODE_INVALID, uint16 a = 0, unsigned char f = 0) : keycode(k), ascii(a), flags(f) {}
};
enum EventType { EVENT_INVALID, EVENT_KEYDOWN, EVENT_KEYUP, EVENT_QUIT,
    EVENT_JOYBUTTON_DOWN, EVENT_JOYBUTTON_UP, EVENT_JOYAXIS_MOTION, EVENT_INPUT_CHANGED };
enum JoystickButton { JOYSTICK_BUTTON_INVALID, JOYSTICK_BUTTON_A, JOYSTICK_BUTTON_B,
    JOYSTICK_BUTTON_X, JOYSTICK_BUTTON_Y, JOYSTICK_BUTTON_BACK, JOYSTICK_BUTTON_GUIDE,
    JOYSTICK_BUTTON_START, JOYSTICK_BUTTON_LEFT_STICK, JOYSTICK_BUTTON_RIGHT_STICK,
    JOYSTICK_BUTTON_LEFT_SHOULDER, JOYSTICK_BUTTON_RIGHT_SHOULDER,
    JOYSTICK_BUTTON_DPAD_UP, JOYSTICK_BUTTON_DPAD_DOWN, JOYSTICK_BUTTON_DPAD_LEFT, JOYSTICK_BUTTON_DPAD_RIGHT };
struct Event { EventType type = EVENT_INVALID; KeyState kbd;
    struct { int button = 0, axis = 0, position = 0; } joystick;
};
template<class T> class Queue {
    std::deque<T> values;
public:
    bool empty() const { return values.empty(); }
    void push(const T &v) { values.push_back(v); }
    T pop() { T v = values.front(); values.pop_front(); return v; }
};
struct U32String : std::u32string {
    U32String() = default;
    explicit U32String(const char *s) : std::u32string(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(s)) {}
    std::string encode() const { return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(*this); }
};
}
enum { SDL_FALSE, SDL_TRUE, SDL_ENABLE, SDL_SYSWMEVENT, SDL_TEXTINPUT, SDL_SYSWM_WIIU,
       SDL_WIIU_SYSWM_SWKBD_OK_START_EVENT, SDL_WIIU_SYSWM_SWKBD_OK_FINISH_EVENT,
       SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT, SDL_OTHER,
       SDL_JOYDEVICEADDED, SDL_JOYDEVICEREMOVED, SDL_JOYBUTTONDOWN, SDL_JOYBUTTONUP, SDL_JOYAXISMOTION,
       SDL_CONTROLLERBUTTONDOWN, SDL_CONTROLLERBUTTONUP, SDL_CONTROLLERAXISMOTION,
       SDL_CONTROLLERDEVICEADDED, SDL_CONTROLLERDEVICEREMOVED, SDL_CONTROLLERDEVICEREMAPPED,
       SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, SDL_USEREVENT };
#define SDL_HINT_MOUSE_TOUCH_EVENTS "SDL_MOUSE_TOUCH_EVENTS"
using SDL_JoystickID = int32_t;
struct SDL_Joystick { SDL_JoystickID id = -1; bool attached = false; int refs = 0; std::string name; };
inline SDL_Joystick *mockDevices() { static SDL_Joystick devices[8]; return devices; }
inline SDL_Joystick *mockDeviceIndex(int index) {
    for (int i = 0; i < 8; ++i) if (mockDevices()[i].attached && index-- == 0) return &mockDevices()[i];
    return nullptr;
}
inline int SDL_NumJoysticks() { int n = 0; for (int i = 0; i < 8; ++i) n += mockDevices()[i].attached; return n; }
inline SDL_JoystickID SDL_JoystickGetDeviceInstanceID(int index) {
    auto *d = mockDeviceIndex(index); return d ? d->id : -1;
}
inline SDL_Joystick *SDL_JoystickOpen(int index) {
    auto *d = mockDeviceIndex(index); if (d) ++d->refs; return d;
}
inline void SDL_JoystickClose(SDL_Joystick *d) { assert(d && d->refs > 0); --d->refs; }
inline SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *d) { return d->id; }
inline const char *SDL_JoystickName(SDL_Joystick *d) { return d->name.c_str(); }
inline const char *SDL_GetError() { return "mock"; }
inline bool SDL_SetHint(const char *, const char *) { return true; }
struct SDL_Window {};
struct SDL_SysWMmsg { int subsystem; struct { struct { unsigned event; } wiiu; } msg; };
struct SDL_Event {
    int type = SDL_OTHER;
    struct { SDL_SysWMmsg *msg = nullptr; } syswm;
    struct { char text[32] = {}; } text;
    struct { SDL_JoystickID which = -1; } jdevice;
    struct { SDL_JoystickID which = -1; unsigned char button = 0; } jbutton;
    struct { SDL_JoystickID which = -1; unsigned char axis = 0; int16 value = 0; } jaxis;
    struct { uint32 which = 0; int x = 0, y = 0; } motion;
    struct { uint32 which = 0; int x = 0, y = 0; unsigned button = 0; } button;
    struct { int32 code = 0; } user;
};
extern SDL_Window window;
extern bool focused, shown, animateClose;
extern int starts, stops, forwarded;
extern std::deque<SDL_Event> input;
extern std::string initialText;
inline void SDL_WiiUSetSWKBDInitialText(const char *s) { initialText = s ? s : ""; }
inline int SDL_EventState(int, int) { return SDL_ENABLE; }
inline SDL_Window *SDL_GetKeyboardFocus() { return focused ? &window : nullptr; }
inline int SDL_IsScreenKeyboardShown(SDL_Window *w) { return w && shown ? SDL_TRUE : SDL_FALSE; }
inline void SDL_StartTextInput() { ++starts; shown = true; }
inline void SDL_StopTextInput() {
    ++stops;
    if (!animateClose) shown = false;
    // Like SDL, disabling text input flushes queued text.
    for (auto it = input.begin(); it != input.end();) {
        if (it->type == SDL_TEXTINPUT) it = input.erase(it); else ++it;
    }
}
struct System { uint32 now = 0; uint32 getMillis() { return now; } };
extern System *g_system;
class SdlWindow { public: SDL_Window *getSDLWindow() { return &window; } };
class SdlGraphicsManager {
public:
    int redraws = 0;
    SdlWindow w;
    SdlWindow *getWindow() { return &w; }
    void notifyVideoExpose() { ++redraws; }
};
class SdlEventSource {
public:
    virtual ~SdlEventSource() = default;
    virtual bool allowMapping() const { return true; }
    virtual bool pollEvent(Common::Event &ev) {
        while (!input.empty()) {
            SDL_Event s = input.front(); input.pop_front();
            if (dispatchSDLEvent(s, ev)) return true;
        }
        return false;
    }
    void setGraphicsManager(SdlGraphicsManager *g) { _graphicsManager = g; }
protected:
    SDL_Joystick *_joystick = nullptr;
    void closeJoystick() { if (_joystick) SDL_JoystickClose(_joystick); _joystick = nullptr; }
    SdlGraphicsManager *_graphicsManager = nullptr;
    virtual bool dispatchSDLEvent(SDL_Event &, Common::Event &ev) {
        ++forwarded; ev.type = Common::EVENT_QUIT; return true;
    }
};
