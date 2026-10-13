#!/usr/bin/env python3
"""Exercise the actual Common buffer used by Groovie, with a bounded mock.

No game files, GPU, SD timings or claims of FMV/crash resolution.
"""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
stream = (root / "common/stream.cpp").read_text()
begin = stream.index("class BufferedReadStream :")
end = stream.index("\n#pragma mark -", stream.index("SeekableReadStream *wrapBufferedSeekableReadStream("))
actual = "namespace {\n" + stream[begin:end]
resource = (root / "engines/groovie/resource.cpp").read_text()
assert "wrapBufferedSeekableReadStream(file, 64 * 1024, DisposeAfterUse::YES)" in resource
assert "SeekableSubReadStream(gjdFile, resInfo.offset, resInfo.offset + resInfo.size, DisposeAfterUse::YES)" in resource
roq = (root / "engines/groovie/video/roq.h").read_text()
assert "bool _flagOverlay = false;" in roq
assert "bool _isFileHandled = false;" in roq
roq_cpp = (root / "engines/groovie/video/roq.cpp").read_text()
assert "drawDone - decodeStart >= 100 && _diagSlowFrames < 24" in roq_cpp
assert roq_cpp.index("GROOVIE ROQ slow frame") < roq_cpp.index("// Wait until the current frame can be shown")
script = (root / "engines/groovie/script.cpp").read_text()
assert "if (++_diagVideoFiles <= 24)" in script

prefix = r'''
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using byte = uint8_t;
using uint32 = uint32_t;
using int64 = int64_t;
#define MIN std::min
namespace DisposeAfterUse { enum Flag { NO, YES }; }
struct ReadStream {
    virtual ~ReadStream() = default;
    virtual bool eos() const = 0;
    virtual bool err() const = 0;
    virtual void clearErr() = 0;
    virtual uint32 read(void *, uint32) = 0;
};
struct SeekableReadStream : virtual ReadStream {
    virtual int64 pos() const = 0;
    virtual int64 size() const = 0;
    virtual bool seek(int64, int = SEEK_SET) = 0;
};
template<class T> struct DisposablePtr {
    T *p; DisposeAfterUse::Flag own;
    DisposablePtr(T *v, DisposeAfterUse::Flag o) : p(v), own(o) {}
    ~DisposablePtr() { if (own == DisposeAfterUse::YES) delete p; }
    T *operator->() const { return p; }
};
static int deleted = 0;
struct BoundedStream : SeekableReadStream {
    std::vector<byte> data;
    int64 at = 0;
    bool ended = false, failed = false;
    unsigned reads = 0;
    explicit BoundedStream(size_t n) : data(n) {
        for (size_t i = 0; i < n; ++i) data[i] = i % 251;
    }
    ~BoundedStream() { ++deleted; }
    bool eos() const override { return ended; }
    bool err() const override { return failed; }
    void clearErr() override { ended = failed = false; }
    int64 pos() const override { return at; }
    int64 size() const override { return data.size(); }
    bool seek(int64 n, int mode) override {
        n += mode == SEEK_END ? size() : mode == SEEK_CUR ? at : 0;
        assert(n >= 0 && n <= size()); at = n; ended = false; return true;
    }
    uint32 read(void *out, uint32 n) override {
        ++reads;
        if (n > size() - at) { n = size() - at; ended = true; }
        memcpy(out, data.data() + at, n); at += n; return n;
    }
};
'''
checks = r'''
int main() {
    assert(!wrapBufferedSeekableReadStream(nullptr, 65536, DisposeAfterUse::YES));
    for (int size : {1, 65535, 65536, 65537, 200003}) {
        auto *parent = new BoundedStream(size);
        auto *file = wrapBufferedSeekableReadStream(parent, 65536, DisposeAfterUse::YES);
        assert(file->size() == size && file->pos() == 0);
        for (int i = 0; i < size; ++i) {
            byte b = 255; assert(file->read(&b, 1) == 1 && b == i % 251);
            assert(file->pos() == i + 1 && !file->eos());
        }
        assert(parent->reads == unsigned((size + 65535) / 65536));
        byte b; assert(file->read(&b, 1) == 0 && file->eos());
        assert(file->seek(-1, SEEK_END) && !file->eos());
        assert(file->read(&b, 1) == 1 && b == (size - 1) % 251);
        assert(file->seek(0));
        std::vector<byte> copy(size + 10);
        assert(file->read(copy.data(), copy.size()) == unsigned(size));
        assert(file->eos() && file->pos() == size && copy[size] == 0);
        for (int i = 0; i < size; ++i) assert(copy[i] == i % 251);
        assert(file->seek(-size, SEEK_CUR) && file->pos() == 0);
        parent->failed = true; assert(file->err()); file->clearErr(); assert(!file->err());
        delete file;
    }
    assert(deleted == 5);
    puts("PASS: actual 64 KiB buffer, byte/read equivalence, bounds, EOF, seek, error and ownership; ROQ initialization/trace source guards (host only)");
}
'''
with tempfile.TemporaryDirectory(prefix="scummvm-groovie-") as temp:
    path = Path(temp)
    (path / "test.cpp").write_text(prefix + actual + checks)
    binary = path / "test"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra",
                    "-Wno-unknown-pragmas", "-fsanitize=address,undefined", "-no-pie",
                    str(path / "test.cpp"), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
