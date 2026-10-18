#!/usr/bin/env python3
"""Actual bounded frame metrics, using a mock clock/queue, not hardware timing."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / 'engines/groovie/video/roq.cpp').read_text()
begin = source.index('void ROQPlayer::recordFrameTiming(')
actual = source[begin:source.index('\n#endif', begin)]
prefix = r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
using uint32 = uint32_t;
#define MAX std::max
int reports = 0;
// Evaluate arguments as the real logger would, without performing file IO.
template<class... Args> void trace(const char *, Args...) { ++reports; }
#define WIIU_TRACE trace
struct Clock { uint32 now = 0; uint32 getMillis() { return now; } } clockMock;
struct Queue { unsigned count = 0; unsigned numQueuedStreams() { return count; } } queueMock;
struct File { bool end = false; bool eos() { return end; } } fileMock;
struct Surface { int w = 640, h = 320; } surfaceMock;
struct ROQPlayer {
    Clock *_syst = &clockMock;
    Queue *_audioStream = &queueMock;
    File *_file = &fileMock;
    Surface *_currBuf = &surfaceMock;
    bool first = false;
    bool playFirstFrame() { return first; }
    float _millisBetweenFrames = 33.333f;
    unsigned _fps = 30;
    float getFrameDuration() const { return _millisBetweenFrames; }
    unsigned getFrameRate() const { return _fps; }
    uint32 _diagFrames = 0, _diagDecodeMs = 0, _diagDrawMs = 0, _diagWaitMs = 0;
    uint32 _diagMaxWorkMs = 0, _diagMaxGapMs = 0, _diagLateFrames = 0, _diagEmptyAudio = 0;
    uint32 _diagLastDone = 0, _diagReports = 0;
    void recordFrameTiming(uint32, uint32, uint32);
};
'''
checks = r'''
int main() {
    ROQPlayer p;
    clockMock.now = 160; p.recordFrameTiming(100, 120, 150);
    assert(p._diagDecodeMs == 20 && p._diagDrawMs == 30 && p._diagWaitMs == 10);
    assert(p._diagMaxWorkMs == 50 && p._diagLateFrames == 1 && p._diagEmptyAudio == 1);
    queueMock.count = 3;
    clockMock.now = 203; p.recordFrameTiming(170, 180, 190);
    assert(p._diagMaxGapMs == 10 && p._diagLateFrames == 1 && p._diagEmptyAudio == 1);
    for (int i = 2; i < 120 * 30; ++i) {
        uint32 start = clockMock.now; clockMock.now += 34;
        p.recordFrameTiming(start, start + 10, start + 20);
    }
    assert(reports == 24 && p._diagReports == 24 && p._diagFrames == 0);
    ROQPlayer wrap; clockMock.now = 14;
    wrap.recordFrameTiming(0xfffffff0, 0xfffffffa, 4);
    assert(wrap._diagDecodeMs == 10 && wrap._diagDrawMs == 10 && wrap._diagWaitMs == 10);
    puts("PASS: actual ROQ metrics, time budget, queue observation, 120-frame reporting, 24-report cap and clock wrap (host mocks)");
}
'''
with tempfile.TemporaryDirectory(prefix='roq-timing-') as temp:
    file = Path(temp) / 'test.cpp'
    file.write_text(prefix + actual + checks)
    binary = file.with_suffix('')
    subprocess.run(['g++', '-std=c++11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-no-pie', str(file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
