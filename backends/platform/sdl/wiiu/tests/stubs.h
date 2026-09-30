// Host-only test doubles. The test compiles the real Wii U event adapter.
#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <deque>
#include <string>
#include <locale>
#include <codecvt>
using uint32 = uint32_t;
using int32 = int32_t;
using uint16 = uint16_t;
namespace Common {
enum KeyCode { KEYCODE_INVALID, KEYCODE_TILDE = 176 };
struct KeyState {
    KeyCode keycode; uint16 ascii; unsigned char flags;
    KeyState(KeyCode k = KEYCODE_INVALID, uint16 a = 0, unsigned char f = 0) : keycode(k), ascii(a), flags(f) {}
};
enum EventType { EVENT_INVALID, EVENT_KEYDOWN, EVENT_KEYUP, EVENT_QUIT };
struct Event { EventType type = EVENT_INVALID; KeyState kbd; };
struct U32String : std::u32string {
    U32String() = default;
    explicit U32String(const char *s) : std::u32string(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(s)) {}
};
}
enum { SDL_FALSE, SDL_TRUE, SDL_ENABLE, SDL_SYSWMEVENT, SDL_TEXTINPUT, SDL_SYSWM_WIIU,
       SDL_WIIU_SYSWM_SWKBD_OK_START_EVENT, SDL_WIIU_SYSWM_SWKBD_OK_FINISH_EVENT,
       SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT, SDL_OTHER };
struct SDL_Window {};
struct SDL_SysWMmsg { int subsystem; struct { struct { unsigned event; } wiiu; } msg; };
struct SDL_Event {
    int type = SDL_OTHER;
    struct { SDL_SysWMmsg *msg = nullptr; } syswm;
    struct { char text[32] = {}; } text;
};
extern SDL_Window window;
extern bool focused, shown, animateClose;
extern int starts, stops, forwarded;
extern std::deque<SDL_Event> input;
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
    SdlGraphicsManager *_graphicsManager = nullptr;
    virtual bool dispatchSDLEvent(SDL_Event &, Common::Event &ev) {
        ++forwarded; ev.type = Common::EVENT_QUIT; return true;
    }
};
