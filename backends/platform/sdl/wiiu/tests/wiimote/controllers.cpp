// Host mocks exercise the real Wii U event adapter; no console claims.
#include "stubs.h"
#include "backends/platform/sdl/wiiu/wiiu-events.h"
#include <iostream>
SDL_Window window;
bool focused = true, shown = false, animateClose = false;
int starts = 0, stops = 0, forwarded = 0;
std::deque<SDL_Event> input;
std::string initialText;
System systemClock;
System *g_system = &systemClock;

static void device(int slot, int id, const char *name) {
    auto &d = mockDevices()[slot]; d.id = id; d.name = name; d.attached = true;
}
static void button(int id, unsigned index, bool down) {
    SDL_Event e; e.type = down ? SDL_JOYBUTTONDOWN : SDL_JOYBUTTONUP;
    e.jbutton.which = id; e.jbutton.button = index; input.push_back(e);
}
static void axis(int id, unsigned index, int value) {
    SDL_Event e; e.type = SDL_JOYAXISMOTION;
    e.jaxis.which = id; e.jaxis.axis = index; e.jaxis.value = value; input.push_back(e);
}
static void changed(int indexOrId, bool added) {
    SDL_Event e; e.type = added ? SDL_JOYDEVICEADDED : SDL_JOYDEVICEREMOVED;
    e.jdevice.which = indexOrId; input.push_back(e);
}
static void expect(WiiUEventSource &s, Common::EventType t, int code, int value = 0) {
    Common::Event e; assert(s.pollEvent(e) && e.type == t);
    if (t == Common::EVENT_JOYAXIS_MOTION) assert(e.joystick.axis == code && e.joystick.position == value);
    if (t == Common::EVENT_JOYBUTTON_DOWN || t == Common::EVENT_JOYBUTTON_UP) assert(e.joystick.button == code);
}
static void none(WiiUEventSource &s) { Common::Event e; assert(!s.pollEvent(e)); }

int main() {
    device(0, 41, "Wii Remote #2"); // Remote can enumerate before a late GamePad.
    {
        WiiUEventSource s;
        assert(mockDevices()[0].refs == 2); // own sample handle + base connection status
        button(41, 0, true); button(41, 0, false);
        button(41, 1, true); button(41, 1, false); none(s); // no duplicated A/B mouse clicks
        button(41, 10, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_START);
        button(41, 10, false); expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_START);
        button(41, 11, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_BACK);
        button(41, 11, false); expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_BACK);
        for (auto pair : {std::make_pair(12, Common::JOYSTICK_BUTTON_DPAD_LEFT),
                         std::make_pair(13, Common::JOYSTICK_BUTTON_DPAD_UP),
                         std::make_pair(14, Common::JOYSTICK_BUTTON_DPAD_RIGHT),
                         std::make_pair(15, Common::JOYSTICK_BUTTON_DPAD_DOWN),
                         std::make_pair(6, Common::JOYSTICK_BUTTON_LEFT_SHOULDER),
                         std::make_pair(7, Common::JOYSTICK_BUTTON_RIGHT_SHOULDER)}) {
            button(41, pair.first, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, pair.second);
            button(41, pair.first, false); expect(s, Common::EVENT_JOYBUTTON_UP, pair.second);
        }
        device(1, 99, "WiiU Gamepad"); changed(1, true);
        expect(s, Common::EVENT_INPUT_CHANGED, 0); assert(mockDevices()[1].refs == 1);
        button(99, 0, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_A);
        button(99, 0, false); expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_A);
        // Shared buttons stay held until all devices release them.
        button(41, 13, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_DPAD_UP);
        button(99, 13, true); none(s);
        button(41, 13, false); none(s);
        button(99, 13, false); expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_DPAD_UP);
        axis(99, 0, 12000); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, 12000);
        axis(41, 0, 0); none(s); // idle Nunchuk cannot cancel GamePad movement
        axis(41, 0, -20000); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, -20000);
        axis(41, 0, 0); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, 12000);
        axis(99, 0, 0); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, 0);
        button(99, 8, true); expect(s, Common::EVENT_JOYAXIS_MOTION, 4, 32767);
        button(99, 8, false); expect(s, Common::EVENT_JOYAXIS_MOTION, 4, 0);
        // Stick emulation indices above 15 are not duplicate keypresses.
        button(99, 17, true); axis(99, 7, 1); button(1234, 0, true); none(s);
        SDL_Event duplicate; duplicate.type = SDL_CONTROLLERBUTTONDOWN; input.push_back(duplicate); none(s);
        // Disconnection releases held buttons and axes, then reports topology.
        button(41, 12, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_DPAD_LEFT);
        axis(41, 1, 30000); expect(s, Common::EVENT_JOYAXIS_MOTION, 1, 30000);
        mockDevices()[0].attached = false; changed(41, false);
        expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_DPAD_LEFT);
        expect(s, Common::EVENT_JOYAXIS_MOTION, 1, 0);
        expect(s, Common::EVENT_INPUT_CHANGED, 0);
        assert(mockDevices()[0].refs == 0 && mockDevices()[1].refs == 2);
        // Same physical channel returns with a NEW SDL instance and a new index.
        device(2, 210, "Wii Remote #2 and Nunchuk"); changed(1, true);
        expect(s, Common::EVENT_INPUT_CHANGED, 0);
        changed(1, true); expect(s, Common::EVENT_INPUT_CHANGED, 0);
        assert(mockDevices()[2].refs == 1); // duplicate add does not leak a reference
        button(210, 0, true); none(s);
        button(210, 2, true); expect(s, Common::EVENT_JOYBUTTON_DOWN, Common::JOYSTICK_BUTTON_X);
        button(210, 2, false); expect(s, Common::EVENT_JOYBUTTON_UP, Common::JOYSTICK_BUTTON_X);
        axis(210, 0, -32768); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, -32768);
        axis(210, 0, 0); expect(s, Common::EVENT_JOYAXIS_MOTION, 0, 0);
        // USB/text/other SDL traffic still follows the existing fallback.
        input.push_back(SDL_Event()); expect(s, Common::EVENT_QUIT, 0);
        none(s);
    }
    for (int i = 0; i < 8; ++i) assert(mockDevices()[i].refs == 0);
    std::cout << "PASS: real WiiUEventSource: simultaneous devices, Remote A/B deduplication, keymap buttons/triggers, axis arbitration, late GamePad, unplug/reconnect/index changes, held release, no handle leaks (host mocks)\n";
}
