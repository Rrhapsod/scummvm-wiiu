#!/usr/bin/env python3
"""Exercise actual SCI trace macros, startup scope and load wrapper with mocks."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / "engines/sci/resource/resource.cpp").read_text()
begin = source.index("#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)")
macros = source[begin:source.index("\nnamespace Sci", begin)]
begin = source.index("\tstruct InitTraceScope {")
scope = source[begin:source.index("\n#endif", begin)]
begin = source.index("void ResourceManager::loadResource(Resource *res)")
load = source[begin:source.index("\n\n\nvoid PatchResourceSource", begin)]
prefix = r'''
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
static std::vector<std::string> messages;
void trace(const char *format, ...) {
    char text[640];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    messages.emplace_back(text);
}
#define WIIU_TRACE(...) trace(__VA_ARGS__)
struct Path { std::string toString() { return "resource.000"; } };
struct Resource;
struct ResourceManager;
struct Source {
    int loads = 0;
    Path getLocationName() { return {}; }
    void loadResource(ResourceManager *, Resource *) { ++loads; }
};
struct Resource {
    Source *_source;
    int getType() { return 6; }
    unsigned getNumber() { return 65535; }
    unsigned size() { return 42; }
};
struct Patcher { int patches = 0; void applyPatch(Resource &) { ++patches; } };
'''
body = r'''
struct ResourceManager {
    bool _diagInitActive = false, _detectionMode = false;
    unsigned _diagInitTraceCount = 0;
    bool g_sci = true;
    Patcher *_patcher = nullptr;
    void loadResource(Resource *res);
    void initMock(bool earlyReturn) {
#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)
SCOPE
#endif
        SCI_INIT_TRACE("startup");
        if (earlyReturn) return;
        SCI_INIT_TRACE("completed");
    }
    void stress() {
        for (int i = 0; i < 300; ++i) SCI_INIT_TRACE("detail");
        SCI_PHASE_TRACE("phase after detail cap");
    }
    void noEvaluation() {
        int value = 0;
        SCI_INIT_TRACE("disabled=%d", ++value);
        SCI_PHASE_TRACE("disabled=%d", ++value);
        assert(value == 0);
    }
};
'''.replace("SCOPE", scope)
checks = r'''
int main() {
    ResourceManager manager;
    Source source;
    Resource resource{&source};
    Patcher patcher;
    manager._patcher = &patcher;
    manager.noEvaluation();
    manager.loadResource(&resource); // Post-init/audio path must not trace.
    assert(messages.empty());
    manager.initMock(true);
    assert(!manager._diagInitActive);
    manager.initMock(false);
    assert(!manager._diagInitActive);
#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)
    assert(messages.size() == 3);
    manager._detectionMode = true;
    manager.initMock(false);
    assert(messages.size() == 3 && !manager._diagInitActive);
    manager._diagInitActive = true;
    manager.loadResource(&resource);
    assert(messages.back() == "SCI load/patch complete");
    assert(source.loads == 2 && patcher.patches == 2);
    messages.clear();
    manager._diagInitTraceCount = 0;
    manager.stress();
    assert(messages.size() == 258);
    assert(messages[256].find("limit reached") != std::string::npos);
    assert(messages[257] == "phase after detail cap");
#else
    manager._diagInitActive = true;
    manager.stress();
    manager.noEvaluation();
    assert(messages.empty());
#endif
    puts("PASS: actual SCI trace guards, cap, early-return cleanup and resource-load wrapper (host mocks)");
}
'''
with tempfile.TemporaryDirectory(prefix="scummvm-sci-diagnostics-") as tmp:
    path = Path(tmp)
    (path / "test.cpp").write_text(prefix + macros + body + load + checks)
    for mode, flags in (("enabled", ["-DWIIU", "-DWIIU_COMPAT_DIAGNOSTICS"]),
                        ("disabled", ["-DWIIU"]), ("nonwiiu", ["-DWIIU_COMPAT_DIAGNOSTICS"])):
        binary = path / mode
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-Werror",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
                        *flags, str(path / "test.cpp"), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
