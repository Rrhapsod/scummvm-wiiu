#!/usr/bin/env python3
"""Host mocks for the actual iMUSE snapshot method and PCM counter block."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
source = (root / "engines/scumm/imuse_digi/dimuse_engine.cpp").read_text()
start = source.index("void IMuseDigital::reportWiiUDiagnostics()")
method = source[start:source.index("\n#endif", start)]
waveout = (root / "engines/scumm/imuse_digi/dimuse_waveout.cpp").read_text()
guard = "#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)\n"
start = waveout.index(guard) + len(guard)
counter = waveout[start:waveout.index("#endif", start)]

prefix = r'''
#include <cassert>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
using uint32 = uint32_t;
using uint8 = uint8_t;
static bool locked = false;
namespace Common {
struct Mutex {};
struct StackLock {
    StackLock(Mutex &) { assert(!locked); locked = true; }
    ~StackLock() { locked = false; }
};
}
struct System { uint32 now = 0xfffff000; uint32 getMillis() { return now; } };
static System systemMock;
static System *g_system = &systemMock;
static std::vector<std::string> messages;
void trace(const char *format, ...) {
    assert(!locked); // Never hold the audio/resource lock during filesystem IO.
    char text[640];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    messages.emplace_back(text);
}
#define WIIU_TRACE(...) trace(__VA_ARGS__)
#define WIIU_AUDIO_REPORT() do { assert(!locked); } while (0)
namespace Audio {
struct Mixer {
    enum SoundType { kPlainSoundType, kMusicSoundType, kSpeechSoundType, kSFXSoundType };
    bool isReady() const { return true; }
    int getVolumeForSoundType(SoundType type) const { return 100 + type; }
    bool isSoundTypeMuted(SoundType type) const { return type == kMusicSoundType; }
};
struct Queue { int numQueuedStreams() const { assert(locked); return 3; } };
}
struct InternalMixer { Audio::Queue *_stream; };
class IMuseDigital {
public:
    Common::Mutex mutex;
    Audio::Mixer mixer;
    Audio::Queue queue;
    InternalMixer internal{&queue};
    Common::Mutex *_mutex = &mutex;
    Audio::Mixer *_mixer = &mixer;
    InternalMixer *_internalMixer = &internal;
    uint32 _diagQueuedBuffers = 0, _diagNonzeroBuffers = 0, _diagLastReport = 0;
    int _diagReports = 0;
    int _cmdsPauseCount = 2, _isEngineDisabled = 0, _waveOutDisableWrite = 0;
    int _curMusicState = 4, _curMusicSeq = 5;
    int _maxQueuedStreams = 3, _tracksPauseTimer = 0, _waveOutXorTrigger = 1;
    void reportWiiUDiagnostics();
    void record(const uint8 *ptr, int _outputFeedSize, int _waveOutBytesPerSample, int _waveOutNumChannels) {
        Common::StackLock lock(*_mutex);
'''
middle = '\n    }\n};\n'
checks = r'''
int main() {
    IMuseDigital engine;
    const uint8 silence[8] = {};
    const uint8 signal[8] = {0,0,0,0,0,0,0,1};
    engine.record(silence, 2, 2, 2);
    engine.record(signal, 2, 2, 2);
    assert(engine._diagQueuedBuffers == 2 && engine._diagNonzeroBuffers == 1);
    assert(messages.empty()); // Counting queued PCM must not log from callback.
    engine.reportWiiUDiagnostics();
    assert(messages.size() == 3);
    assert(messages[0].find("queuedTotal=2 nonzeroTotal=1 queueNow=3") != std::string::npos);
    assert(messages[0].find("paused=2 disabled=0") != std::string::npos);
    assert(messages[1].find("music=101 speech=102 sfx=103") != std::string::npos);
    assert(messages[1].find("mutePlain=0 muteMusic=1 muteSpeech=0 muteSfx=0") != std::string::npos);
    assert(messages[2].find("maxQueued=3 tracksPaused=0 xorTrigger=1") != std::string::npos);
    for (int i = 1; i < 16; ++i) {
        systemMock.now += 1999;
        engine.reportWiiUDiagnostics();
        assert(messages.size() == unsigned(i * 3));
        ++systemMock.now; // Also crosses uint32 milliseconds wraparound.
        engine.reportWiiUDiagnostics();
        assert(messages.size() == unsigned((i + 1) * 3));
    }
    systemMock.now += 2000;
    engine.reportWiiUDiagnostics();
    assert(messages.size() == 48);
    IMuseDigital lowLatency;
    lowLatency.internal._stream = nullptr;
    lowLatency.reportWiiUDiagnostics();
    assert(messages[48].find("queueNow=-1") != std::string::npos);
    puts("PASS: actual audio snapshot/counters, no callback logging, lock release, rate limit, timer wrap and null stream (host mocks)");
}
'''
with tempfile.TemporaryDirectory(prefix="scummvm-audio-diagnostic-") as tmp:
    path = Path(tmp)
    (path / "test.cpp").write_text(prefix + counter + middle + method + checks)
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra",
                    "-Werror", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                    "-no-pie", str(path / "test.cpp"), "-o", str(path / "test")], check=True)
    subprocess.run([str(path / "test")], check=True)
