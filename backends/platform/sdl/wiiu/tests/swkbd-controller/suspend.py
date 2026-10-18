#!/usr/bin/env python3
"""Actual SDL suspend/finalize flow with native UI lifecycle mocks."""
from pathlib import Path
import subprocess
import sys
import tempfile

source = Path(sys.argv[1]).read_text()
def function(name):
    start = source.index('void ' + name + '(')
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]

prefix = r'''
#include <cassert>
#include <cstdio>
#include <optional>
#include <vector>
enum { SDL_SYSWM_WIIU = 1, SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT = 2 };
struct SDL_SysWMmsg { int version, subsystem; struct { struct { int event; } wiiu; } msg; };
#define SDL_VERSION(v) (*(v) = 1)
std::vector<int> actions;
namespace nn { namespace swkbd { void Destroy() { actions.push_back(3); } } }
namespace detail {
namespace create {
bool created = false;
std::optional<int> region;
void cleanup() { assert(!created); actions.push_back(4); }
}
namespace appear { void *window = nullptr; bool decisionSent = false; }
struct { int value = 0; } controllerInfo;
SDL_SysWMmsg wmMsgFinish;
}
void WIIU_SWKBD_HideScreenKeyboard(void *, void *) { actions.push_back(1); }
void SDL_SendSysWMEvent(SDL_SysWMmsg *m) {
    assert(m->subsystem == SDL_SYSWM_WIIU && m->msg.wiiu.event == SDL_WIIU_SYSWM_SWKBD_CANCEL_EVENT);
    actions.push_back(2);
}
'''
checks = r'''
int main() {
    WIIU_SWKBD_Suspend(); assert(actions.empty());
    for (bool decided : {false, true}) {
        actions.clear();
        detail::create::created = true; detail::create::region = 1;
        detail::appear::window = &actions; detail::appear::decisionSent = decided;
        detail::controllerInfo.value = 42;
        WIIU_SWKBD_Suspend();
        assert(actions == (decided ? std::vector<int>{3, 4} : std::vector<int>{1, 2, 3, 4}));
        assert(!detail::create::created && !detail::create::region && !detail::appear::window);
        assert(!detail::appear::decisionSent && detail::controllerInfo.value == 0);
        auto previous = actions;
        WIIU_SWKBD_Suspend(); WIIU_SWKBD_Finalize(); assert(actions == previous);
    }
    puts("PASS: actual SDL HOME suspend sends Cancel before destroy, no duplicate decisions, reset input/window, cleanup order and idempotence (native UI mocked)");
}
'''
with tempfile.TemporaryDirectory(prefix='swkbd-suspend-') as temp:
    file = Path(temp) / 'test.cpp'
    file.write_text(prefix + function('WIIU_SWKBD_Finalize') + function('WIIU_SWKBD_Suspend') + checks)
    binary = file.with_suffix('')
    subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-no-pie', str(file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
