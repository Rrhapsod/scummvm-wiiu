#!/usr/bin/env python3
"""Regression for the actual SDL Wii U zero-timeout code; no OS emulation."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

source = Path(sys.argv[1]).read_text()
start = source.index("int\nSDL_SemTryWait(")
methods = source[start:source.index("int\nSDL_SemWait(", start)]
assert "return SDL_SemTryWait(sem);" in methods
old = methods.replace("return SDL_SemTryWait(sem);", "SDL_SemTryWait(sem);")
prefix = r'''
#include <cassert>
#include <cstdio>
using Uint32 = unsigned int;
constexpr int SDL_MUTEX_TIMEDOUT = 1;
struct OSMutex {};
struct OSSemaphore { int count; };
struct OSCondition {};
struct OSContext {};
struct OSAlarm { void *data; };
struct SDL_sem { OSMutex mtx; OSSemaphore sem; OSCondition cond; };
struct WIIU_SemWaitTimeoutData { OSCondition *cond; bool timed_out; };
static int tries, locks, arms, waits, cancels;
static OSAlarm *active;
static bool deliverToken;
static SDL_sem *current;
int OSTryWaitSemaphore(OSSemaphore *sem) {
    ++tries;
    int before = sem->count;
    if (before > 0) --sem->count;
    return before;
}
void OSLockMutex(OSMutex *) { ++locks; }
void OSUnlockMutex(OSMutex *) {}
void OSCreateAlarm(OSAlarm *) {}
void OSSetAlarmUserData(OSAlarm *alarm, void *data) { alarm->data = data; }
void *OSGetAlarmUserData(OSAlarm *alarm) { return alarm->data; }
void OSSignalCond(OSCondition *) {}
Uint32 OSMillisecondsToTicks(Uint32 ms) { return ms; }
void OSSetAlarm(OSAlarm *alarm, Uint32 ms, void (*)(OSAlarm *, OSContext *)) {
    ++arms;
    // Deterministic detector for entering the forbidden zero-timeout path.
    // Not a claim about what the real OS does with a zero-duration alarm.
    if (ms == 0) throw 42;
    active = alarm;
}
void OSCancelAlarm(OSAlarm *) { ++cancels; active = nullptr; }
void OSWaitCond(OSCondition *, OSMutex *) {
    ++waits;
    assert(active);
    if (deliverToken) ++current->sem.count;
    else static_cast<WIIU_SemWaitTimeoutData *>(active->data)->timed_out = true;
}
void reset(SDL_sem &sem, int tokens) {
    sem.sem.count = tokens;
    current = &sem;
    tries = locks = arms = waits = cancels = 0;
    active = nullptr;
    deliverToken = false;
}
'''
checks = r'''
int main() {
    SDL_sem sem{};
#ifdef OLD_IMPLEMENTATION
    for (int tokens : {0, 1, 2}) {
        reset(sem, tokens);
        bool rejected = false;
        try { SDL_SemWaitTimeout(&sem, 0); }
        catch (int code) { rejected = code == 42; }
        assert(rejected && arms == 1); // Old code violates immediate-return path.
    }
    puts("PASS: pre-fix code reproduces forbidden zero-timeout alarm path (mock)");
#else
    for (int tokens : {0, 1, 2}) {
        reset(sem, tokens);
        int result = SDL_SemWaitTimeout(&sem, 0);
        assert(result == (tokens ? 0 : SDL_MUTEX_TIMEDOUT));
        assert(sem.sem.count == (tokens ? tokens - 1 : 0));
        assert(tries == 1 && locks == 0 && arms == 0 && waits == 0 && cancels == 0);
    }
    reset(sem, 0);
    assert(SDL_SemWaitTimeout(&sem, 10) == SDL_MUTEX_TIMEDOUT);
    assert(arms == 1 && waits == 1 && cancels == 1);
    reset(sem, 1);
    assert(SDL_SemWaitTimeout(&sem, 10) == 0 && sem.sem.count == 0);
    assert(tries == 1 && waits == 0 && cancels == 1);
    reset(sem, 0);
    deliverToken = true;
    assert(SDL_SemWaitTimeout(&sem, 10) == 0 && sem.sem.count == 0);
    assert(waits == 1 && cancels == 1);
    reset(sem, 0);
    for (int i = 0; i < 10000; ++i)
        assert(SDL_SemWaitTimeout(&sem, 0) == SDL_MUTEX_TIMEDOUT);
    assert(arms == 0 && waits == 0);
    puts("PASS: actual patched SDL wait, zero timeout/token counts, positive timeout/acquire/wake (host mocks)");
#endif
}
'''
with tempfile.TemporaryDirectory(prefix="scummvm-timer-zero-") as tmp:
    path = Path(tmp)
    for name, code, flags in (("old", old, ["-DOLD_IMPLEMENTATION"]), ("fixed", methods, [])):
        cpp = path / (name + ".cpp")
        cpp.write_text("#include <initializer_list>\n" + prefix + code + checks)
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
                        *flags, str(cpp), "-o", str(path / name)], check=True)
        subprocess.run([str(path / name)], check=True)
