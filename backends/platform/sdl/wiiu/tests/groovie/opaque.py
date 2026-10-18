#!/usr/bin/env python3
"""Actual opaque row helper vs original blending; both byte orders, host only."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / 'engines/groovie/video/roq.cpp').read_text()
start = source.index('static bool copyOpaqueRow(')
helper = source[start:source.index('\n#endif', start)]
start = source.index('static inline void copyPixel(')
reference = source[start:source.index('\n#ifdef WIIU', start)]
prefix = r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using byte = uint8_t;
using uint32 = uint32_t;
#define MAX std::max
#ifdef TEST_BIG_ENDIAN
const int kAIndex = 3, kRIndex = 0, kGIndex = 1, kBIndex = 2;
#else
const int kAIndex = 0, kRIndex = 3, kGIndex = 2, kBIndex = 1;
#endif
'''
checks = r'''
int main() {
    for (int width : {0, 1, 17, 640, 1280}) for (int mode : {0, 1, 2}) {
        std::vector<uint32> input(width + 2, 0xabcdef01), output(width + 2, 0x12345678);
        byte *src = (byte *)(input.data() + 1);
        byte *dst = (byte *)(output.data() + 1);
        for (int i = 0; i < width; ++i) src[i * 4 + kAIndex] = mode == 0 ? 255 : (i * 13 + mode) % 256;
        auto expected = output;
        for (int i = 0; i < width; ++i) copyPixelWithA((byte *)(expected.data() + 1) + i * 4, src + i * 4);
        if (!copyOpaqueRow(dst, src, width)) {
            assert(output == std::vector<uint32>(width + 2, 0x12345678));
            for (int i = 0; i < width; ++i) copyPixelWithA(dst + i * 4, src + i * 4);
        }
        assert(output == expected);
    }
    assert(copyOpaqueRow(nullptr, nullptr, 0) && copyOpaqueRow(nullptr, nullptr, -10));
    puts("PASS: actual opaque-row fast path == original alpha blending; opaque/partial/transparent, clipping widths and canaries (host)");
}
'''
assert 'if (!_alpha && !_flagMasked && destBuf != _overBuf && _scaleX == 1' in source
with tempfile.TemporaryDirectory(prefix='roq-opaque-') as temp:
    file = Path(temp) / 'test.cpp'
    file.write_text(prefix + reference + helper + checks)
    for flags in [[], ['-DTEST_BIG_ENDIAN']]:
        binary = file.with_suffix('')
        subprocess.run(['g++', '-std=c++11', '-fsanitize=address,undefined', '-no-pie', *flags, str(file), '-o', str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
