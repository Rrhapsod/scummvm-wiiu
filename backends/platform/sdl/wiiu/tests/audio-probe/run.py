#!/usr/bin/env python3
"""Compile the actual lock-free probe with host logging and concurrent callers."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
engine_source = (root / "engines/scumm/imuse_digi/dimuse_engine.cpp").read_text()
start = engine_source.index("void IMuseDigital::callback()")
callback = engine_source[start:engine_source.index("\nvoid IMuseDigital::diMUSEHeartbeat()", start)]
test = r'''
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include "backends/platform/sdl/wiiu/wiiu-audio-probe.h"
static std::thread::id mainThread = std::this_thread::get_id();
static std::vector<std::string> messages;
void wiiuTrace(const char *format, ...) {
    assert(std::this_thread::get_id() == mainThread);
    char text[640];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    messages.emplace_back(text);
}
class IMuseDigital {
public:
    int _cmdsPauseCount = 0, _callbackInterruptFlag = 0, heartbeats = 0;
    void diMUSEHeartbeat() { ++heartbeats; }
    void callback();
};
CALLBACK_BODY
int main() {
#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back([] {
        for (int j = 0; j < 10000; ++j) {
            WIIU_AUDIO_HIT(kWiiUTimerEnter);
            WIIU_AUDIO_HIT(kWiiUTimerLeave);
            WIIU_AUDIO_HIT(kWiiUIMuseEnter);
            WIIU_AUDIO_HIT(kWiiUIMuseLeave);
        }
    });
    // Exercise concurrent snapshots; they need not be transactionally equal.
    for (int i = 0; i < 20; ++i) WIIU_AUDIO_REPORT();
    for (auto &thread : threads) thread.join();
    WIIU_AUDIO_HIT(kWiiUIMusePaused);
    WIIU_AUDIO_HIT(kWiiUWaveEnter);
    WIIU_AUDIO_HIT(kWiiUWaveLocked);
    WIIU_AUDIO_REPORT();
    assert(messages.size() == 21);
    assert(messages.back().find("timer=40000/40000 imuse=40000/40000 paused=1 reentered=0 wave=1/1/0") != std::string::npos);
    IMuseDigital engine;
    engine.callback();
    assert(engine.heartbeats == 1 && engine._callbackInterruptFlag == 0);
    engine._cmdsPauseCount = 1;
    engine.callback();
    engine._cmdsPauseCount = 0;
    engine._callbackInterruptFlag = 1;
    engine.callback();
    assert(engine.heartbeats == 1 && engine._callbackInterruptFlag == 1);
    assert(messages.size() == 21); // Actual callback never logs, including skip paths.
    WIIU_AUDIO_REPORT();
    assert(messages.back().find("imuse=40003/40003 paused=2 reentered=1") != std::string::npos);
#else
    int evaluated = 0;
    WIIU_AUDIO_HIT(++evaluated);
    WIIU_AUDIO_REPORT();
    assert(evaluated == 0 && messages.empty());
#endif
    puts("PASS: actual audio probe counters, main-thread-only logging and disabled macros (host)");
}
'''.replace("CALLBACK_BODY", callback)
with tempfile.TemporaryDirectory(prefix="scummvm-audio-probe-") as tmp:
    path = Path(tmp)
    (path / "test.cpp").write_text(test)
    for mode, flags in (
        ("enabled", ["-DWIIU", "-DWIIU_COMPAT_DIAGNOSTICS", "-DWIIU_LIFECYCLE_DIAGNOSTICS"]),
        ("disabled", ["-DWIIU"]),
        ("nonwiiu", ["-DWIIU_COMPAT_DIAGNOSTICS", "-DWIIU_LIFECYCLE_DIAGNOSTICS"]),
    ):
        binary = path / mode
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-Werror",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie", "-pthread",
                        *flags, "-I" + str(root), str(path / "test.cpp"),
                        str(root / "backends/platform/sdl/wiiu/wiiu-audio-probe.cpp"),
                        "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
