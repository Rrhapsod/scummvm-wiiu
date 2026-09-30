#include "stubs.h"
#include "backends/platform/sdl/wiiu/wiiu-events.h"
#include <iostream>
SDL_Window window;
bool focused = true, shown = false, animateClose = false;
int starts = 0, stops = 0, forwarded = 0;
std::deque<SDL_Event> input;
System systemClock;
System *g_system = &systemClock;
static SDL_SysWMmsg messages[] = {
    {SDL_SYSWM_WIIU, {{SDL_WIIU_SYSWM_SWKBD_OK_START_EVENT}}},
    {SDL_SYSWM_WIIU, {{SDL_WIIU_SYSWM_SWKBD_OK_FINISH_EVENT}}},
    {SDL_SYSWM_WIIU, {{SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT}}}
};
void wm(int i) { SDL_Event e; e.type = SDL_SYSWMEVENT; e.syswm.msg = &messages[i]; input.push_back(e); }
void text(const char *s) {
    assert(std::strlen(s) < 32);
    SDL_Event e; e.type = SDL_TEXTINPUT; std::strcpy(e.text.text, s); input.push_back(e);
}
std::u32string drain(WiiUEventSource &s) {
    std::u32string out;
    Common::Event e;
    for (int i = 0; i < 100; ++i) {
        int downs = 0, ups = 0;
        while (s.pollEvent(e)) {
            assert(e.type == Common::EVENT_KEYDOWN || e.type == Common::EVENT_KEYUP);
            assert(!s.allowMapping() && e.kbd.flags == 0);
            if (e.kbd.ascii < 127) assert(e.kbd.keycode != Common::KEYCODE_INVALID);
            if (e.type == Common::EVENT_KEYDOWN) { out += e.kbd.ascii; ++downs; }
            else { assert(!out.empty() && out.back() == e.kbd.ascii); ++ups; }
        }
        assert(downs <= 1 && downs == ups); // no per-tick SCUMM overwrite
        systemClock.now += 100;
    }
    return out;
}
int main() {
    WiiUEventSource s;
    SdlGraphicsManager graphics; s.setGraphicsManager(&graphics);
    Common::Event e;
    focused = false; s.setKeyboardVisible(true); assert(starts == 0);
    focused = true; s.setKeyboardVisible(true); s.setKeyboardVisible(true);
    assert(starts == 1 && s.isKeyboardActive());
    assert(!s.pollEvent(e) && graphics.redraws == 1); // redraw idle keyboard
    wm(0); text("Sam e Max "); text("Ação 123 - nome longo "); text("mais de 31 bytes"); wm(1);
    assert(drain(s) == U"Sam e Max Ação 123 - nome longo mais de 31 bytes");
    assert(stops == 1 && !s.isKeyboardActive() && graphics.redraws >= 2);
    // SDK can repeat a decision while its disappearance animates.
    wm(0); text("duplicate"); wm(1); assert(drain(s).empty() && stops == 1);
    s.setKeyboardVisible(true); wm(0); text("discard"); wm(2);
    assert(drain(s).empty() && !s.isKeyboardActive()); // no Escape/Enter
    s.setKeyboardVisible(true); wm(0); wm(1); assert(drain(s).empty());
    s.setKeyboardVisible(true); wm(0); text("AB"); wm(1);
    assert(s.pollEvent(e) && e.type == Common::EVENT_KEYDOWN && e.kbd.ascii == 'A');
    s.setKeyboardVisible(false); // leaving a field drops remaining text, not key-up
    assert(s.pollEvent(e) && e.type == Common::EVENT_KEYUP && e.kbd.ascii == 'A');
    assert(drain(s).empty());
    // Clock wrap and Unicode supplementary/control values cannot become keys.
    systemClock.now = 0xffffffceU;
    s.setKeyboardVisible(true); wm(0); text("A😀B\nC"); wm(1);
    assert(drain(s) == U"ABC");
    // Native disappearance is asynchronous. Focus loss/engineDone/quit must
    // not restart it, and a new request must wait until it has finished.
    s.setKeyboardVisible(true);
    animateClose = true;
    wm(2);
    assert(drain(s).empty());
    const int stopped = stops, started = starts, redraws = graphics.redraws;
    s.setKeyboardVisible(false);
    s.setKeyboardVisible(false);
    s.setKeyboardVisible(true);
    assert(stops == stopped && starts == started && shown);
    assert(!s.pollEvent(e) && graphics.redraws > redraws);
    shown = false;
    animateClose = false;
    s.setKeyboardVisible(true);
    assert(starts == started + 1);
    // Discard a partially received submission before entering another field.
    wm(0); text("stale"); assert(!s.pollEvent(e));
    s.setKeyboardVisible(false);
    s.setKeyboardVisible(true);
    wm(0); text("new"); wm(1); assert(drain(s) == U"new");
    input.push_back(SDL_Event()); assert(s.pollEvent(e));
    assert(e.type == Common::EVENT_QUIT && forwarded == 1); // fallback stays intact
    assert(s.allowMapping());
    std::cout << "PASS: WiiUEventSource mock tests (not hardware): UTF-8, pacing, key pairs, confirm/cancel, duplicate close, fade-out redraw/reopen, discarded partial submission, focus loss, timer wrap, fallback\n";
}
