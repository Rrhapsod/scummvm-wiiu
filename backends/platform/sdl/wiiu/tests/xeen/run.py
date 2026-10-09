#!/usr/bin/env python3
"""Exercise the actual MM keymap factory with host mocks, not game/hardware code."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / "engines/mm/metaengine.cpp").read_text()
start = source.index("Common::KeymapArray MMMetaEngine::initKeymaps(")
end = source.index("Common::String MMMetaEngine::getGameId(", start)
factory = source[start:end]

prefix = r'''
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#define _(x) x
namespace Common {
using String = std::string;
enum KeyCode { KEYCODE_INVALID, KEYCODE_UP, KEYCODE_DOWN, KEYCODE_LEFT,
    KEYCODE_RIGHT, KEYCODE_RETURN, KEYCODE_ESCAPE };
enum { ASCII_RETURN = 13, ASCII_ESCAPE = 27 };
struct KeyState {
    KeyCode keycode;
    int ascii;
    KeyState(KeyCode k, int a = 0) : keycode(k), ascii(a) {}
};
const char *kStandardActionLeftClick = "LCLK";
const char *kStandardActionRightClick = "RCLK";
const char *kStandardActionMoveUp = "UP";
const char *kStandardActionMoveDown = "DOWN";
const char *kStandardActionMoveLeft = "LEFT";
const char *kStandardActionMoveRight = "RIGHT";
const char *kStandardActionSkip = "SKIP";
struct Action {
    String id;
    KeyState key{KEYCODE_INVALID};
    int mouse = 0;
    bool repeats = false;
    std::vector<String> mappings;
    Action(const char *i, const char *) : id(i) {}
    void setKeyEvent(KeyState k) { key = k; }
    void setLeftClickEvent() { mouse = 1; }
    void setRightClickEvent() { mouse = 2; }
    void addDefaultInputMapping(const char *m) { mappings.push_back(m); }
    void allowKbdRepeats() { repeats = true; }
};
struct Keymap {
    enum { kKeymapTypeGame };
    String id;
    bool partial = true;
    std::vector<Action *> actions;
    Keymap(int, const char *i, const char *) : id(i) {}
    ~Keymap() { for (auto a : actions) delete a; }
    void addAction(Action *a) { actions.push_back(a); }
    void setPartialMatchAllowed(bool p) { partial = p; }
};
using KeymapArray = std::vector<Keymap *>;
}
namespace MM { namespace MM1 {
struct MetaEngine {
    static Common::KeymapArray initKeymaps() {
        return { new Common::Keymap(0, "mm1-original", "MM1") };
    }
};
} }
class MMMetaEngine {
public:
    Common::KeymapArray initKeymaps(const char *target) const;
    static Common::String getGameId(const char *target) { return target; }
    static bool isXeenGame(const char *target) {
        return getGameId(target) != "mm1" && getGameId(target) != "mm1_enh";
    }
};
'''

checks = r'''
int main() {
    using namespace Common;
    MMMetaEngine engine;
    for (const char *target : {"worldofxeen", "cloudsofxeen", "darksideofxeen", "swordsofxeen", "mm1", "mm1_enh"}) {
        auto maps = engine.initKeymaps(target);
        assert(maps[0]->id == "mm-main");
        assert(maps[0]->actions.size() == 2);
        assert(maps[0]->actions[0]->mouse == 1);
        assert(maps[0]->actions[0]->mappings[1] == "JOY_A");
        assert(maps[0]->actions[1]->mouse == 2);
        assert(maps[0]->actions[1]->mappings[1] == "JOY_B");
        unsigned expected = 1;
#ifdef ENABLE_XEEN
        if (MMMetaEngine::isXeenGame(target)) {
            ++expected;
            auto map = maps[1];
            assert(map->id == "xeen-controls" && !map->partial);
            assert(map->actions.size() == 6);
            const KeyCode keys[] = {KEYCODE_UP, KEYCODE_DOWN, KEYCODE_LEFT,
                KEYCODE_RIGHT, KEYCODE_RETURN, KEYCODE_ESCAPE};
            const char *joy[] = {"JOY_UP", "JOY_DOWN", "JOY_LEFT", "JOY_RIGHT", "JOY_X", "JOY_Y"};
            const char *kbd[] = {"UP", "DOWN", "LEFT", "RIGHT", "RETURN", "ESCAPE"};
            for (unsigned i = 0; i < 6; ++i) {
                auto action = map->actions[i];
                assert(action->key.keycode == keys[i]);
                assert(action->mappings.size() == 2);
                assert(action->mappings[0] == kbd[i]);
                assert(action->mappings[1] == joy[i]);
                assert(action->repeats == (i < 4));
            }
            assert(map->actions[4]->key.ascii == ASCII_RETURN);
            assert(map->actions[5]->key.ascii == ASCII_ESCAPE);
        }
#endif
#ifdef ENABLE_MM1
        if (!MMMetaEngine::isXeenGame(target)) {
            ++expected;
            assert(maps[1]->id == "mm1-original");
        }
#endif
        assert(maps.size() == expected);
        for (auto map : maps) delete map;
    }
}
'''

with tempfile.TemporaryDirectory(prefix="scummvm-xeen-keymap-") as tmp:
    path = Path(tmp)
    (path / "test.cpp").write_text(prefix + factory + checks)
    for flags in (["-DENABLE_XEEN"], ["-DENABLE_XEEN", "-DENABLE_MM1"], ["-DENABLE_MM1"]):
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall",
                        "-Wextra", "-Werror", "-fsanitize=address,undefined",
                        "-fno-omit-frame-pointer", "-no-pie", *flags,
                        str(path / "test.cpp"), "-o", str(path / "test")], check=True)
        subprocess.run([str(path / "test")], check=True)
print("PASS: actual MM keymap factory, Xeen bindings, repeats, confirm/cancel, MM1 isolation (host mocks)")
