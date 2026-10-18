#!/usr/bin/env python3
"""Exercise the actual render-stage accumulation block with a mock clock."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / 'backends/graphics/surfacesdl/surfacesdl-graphics.cpp').read_text()
header = (root / 'backends/graphics/surfacesdl/surfacesdl-graphics.h').read_text()
start = source.index('\t\tif (traceRender) {\n\t\t\tconst uint32 presentMs')
actual = source[start:source.index('\n#endif', start)]
start = header.index('\tuint32 _wiiuRenderFrames')
fields = header[start:header.index('\n#endif', start)]
prefix = r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
using uint32 = uint32_t;
#define MAX std::max
uint32 now = 0;
uint32 SDL_GetTicks() { return now; }
const char *SDL_GetPixelFormatName(unsigned) { return "mock"; }
unsigned reports = 0;
void trace(const char *, unsigned convert, unsigned software, unsigned upload,
           unsigned submit, unsigned present, unsigned maximum,
           const char *, const char *, int, int, unsigned index) {
    assert(convert == 1200 && software == 240 && upload == 360 && submit == 120);
    assert(present == 480 && maximum == 4 && index == ++reports);
}
#define WIIU_TRACE trace
struct Format { unsigned format = 0; } formatMock;
struct Surface { Format *format = &formatMock; int w = 640, h = 480; } surfaceMock;
struct Renderer {
    Surface *_screen = &surfaceMock, *_hwScreen = &surfaceMock;
'''
method = r'''
    void frame(bool overlay = false, unsigned bpp = 4) {
        const bool traceRender = !overlay && bpp == 4 && _wiiuRenderReports < 24;
        const uint32 presentStart = now;
        now += 4;
        uint32 convertMs = 10, softwareMs = 2;
        _wiiuFrameUploadMs = 3; _wiiuFrameSubmitMs = 1;
'''
checks = r'''
    }
};
int main() {
    Renderer r;
    r.frame(true); r.frame(false, 1); r.frame(false, 2);
    assert(r._wiiuRenderFrames == 0);
    for (int i = 0; i < 120 * 30; ++i) r.frame();
    assert(reports == 24 && r._wiiuRenderFrames == 0);
    assert(r._wiiuConvertMs == 0 && r._wiiuPresentMs == 0);
    now = 0xfffffffe; r._wiiuRenderReports = 0; r.frame();
    assert(r._wiiuPresentMs == 4 && r._wiiuMaxPresentMs == 4);
    puts("PASS: actual render-stage sums, 120-frame windows, cap, overlay/8/16-bit exclusion and clock wrap (host mocks)");
}
'''
# Check that measuring does not introduce per-frame logging or change rendering.
assert source.index('SDL_UpdateTexture(_screenTexture') < source.index('_wiiuFrameUploadMs = uploadDone')
roq = (root / 'engines/groovie/video/roq.cpp').read_text()
load = roq[roq.index('uint16 ROQPlayer::loadInternal()'):roq.index('void ROQPlayer::waitFrame()')]
assert '_diagFrames = _diagDecodeMs = _diagDrawMs = _diagWaitMs = 0;' in load
assert '_diagReports = 0' not in load
with tempfile.TemporaryDirectory(prefix='wiiu-render-stages-') as tmp:
    file = Path(tmp) / 'test.cpp'
    file.write_text(prefix + fields + method + actual + checks)
    binary = file.with_suffix('')
    subprocess.run(['g++', '-std=c++11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
        '-no-pie', str(file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
