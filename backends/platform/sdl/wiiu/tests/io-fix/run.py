#!/usr/bin/env python3
"""Actual changed methods, synthetic data, ASan/UBSan; no console timing claims."""
import os
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[6]
sci = (root / "engines/sci/resource/resource.cpp").read_text()
start = sci.index("class WiiUCachedVolumeFile final")
cached = sci[start:sci.index("\n#endif", start)]
start = sci.index("bool ResourceManager::validateResource(")
validate = sci[start:sci.index("\nResource *ResourceManager::addResource", start)]
bundle = (root / "engines/scumm/imuse_digi/dimuse_bndmgr.cpp").read_text()
start = bundle.index("bool BundleMgr::loadCompTable(")
table = bundle[start:bundle.index("\nint32 BundleMgr::seekFile", start)]
old_table = table.replace("BundleMgr::loadCompTable", "OldBundleMgr::loadCompTable").replace(
    "_file->readUint32BE(); // Reserved word; keep the table read sequential.",
    "_file->seek(4, SEEK_CUR);")
old_table = old_table.replace("\t_file->readUint32BE();\n\t_lastBlock", "\t_file->seek(4, SEEK_CUR);\n\t_lastBlock")

prefix = r'''
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
using int64 = int64_t;
using uint32 = uint32_t;
using int32 = int32_t;
using byte = uint8_t;
namespace Common {
struct Path { std::string toString() const { return "volume"; } };
struct File {
    mutable int queries = 0;
    int64 length = 1234;
    bool closed = false;
    virtual ~File() = default;
    virtual int64 size() const { ++queries; return length; }
    virtual void close() { closed = true; }
};
}
struct ResourceId { std::string toString() const { return "resource"; } };
void warning(const char *, ...) {}
struct ResourceManager {
    bool validateResource(const ResourceId &, const Common::Path &, const Common::Path &,
                          uint32, uint32, uint32) const;
};
#define MKTAG(a,b,c,d) ((uint32(a)<<24)|(uint32(b)<<16)|(uint32(c)<<8)|uint32(d))
void debug(const char *, ...) {}
const char *tag2str(uint32) { return "tag"; }
struct Stream {
    std::vector<byte> data;
    size_t pos = 0;
    int seeks = 0;
    void word(uint32 w) { for (int s = 24; s >= 0; s -= 8) data.push_back(w >> s); }
    void seek(int off, int whence) {
        ++seeks;
        pos = whence == SEEK_SET ? off : pos + off;
        assert(pos <= data.size());
    }
    uint32 readUint32BE() {
        assert(pos + 4 <= data.size());
        uint32 value = 0;
        for (int i = 0; i < 4; ++i) value = (value << 8) | data[pos++];
        return value;
    }
    std::string getDebugName() { return "synthetic.bun"; }
};
struct BundleMgr {
    struct Entry { int32 offset; } _bundleTable[1] = {{0}};
    struct CompTable { int32 offset, size, codec; } *_compTable = nullptr;
    Stream *_file;
    int _numCompItems = 0, _lastBlockDecompressedSize = 0;
    bool _isUncompressed = false;
    byte *_compInputBuff = nullptr;
    explicit BundleMgr(Stream *file) : _file(file) {}
    ~BundleMgr() { free(_compTable); free(_compInputBuff); }
    bool loadCompTable(int32 index);
};
struct OldBundleMgr : BundleMgr {
    using BundleMgr::BundleMgr;
    bool loadCompTable(int32 index);
};
'''
checks = r'''
int main() {
    WiiUCachedVolumeFile file;
    ResourceManager manager;
    for (int i = 0; i < 10000; ++i) {
        assert(file.size() == 1234);
        assert(manager.validateResource({}, {}, {}, 1000, 234, file.size()));
        assert(!manager.validateResource({}, {}, {}, 1000, 235, file.size()));
        assert(!manager.validateResource({}, {}, {}, 1234, 0, file.size()));
    }
    assert(file.queries == 1);
    file.close();
    file.length = 8000;
    assert(file.closed && file.size() == 8000 && file.queries == 2);
    WiiUCachedVolumeFile reopened;
    reopened.length = 99;
    assert(reopened.size() == 99 && reopened.queries == 1);
    WiiUCachedVolumeFile failure;
    failure.length = -1;
    assert(failure.size() == -1 && failure.size() == -1 && failure.queries == 2);
    failure.length = 0;
    assert(failure.size() == 0 && failure.size() == 0 && failure.queries == 3);
    for (int count : {1, 37, 4096}) {
        Stream now;
        now.word(MKTAG('C','O','M','P'));
        now.word(count);
        now.word(0xdeadbeef); // reserved words need not be zero
        now.word(8111);
        for (int i = 0; i < count; ++i) {
            now.word(16 + count * 16 + i * 128);
            now.word(64 + i % 64);
            now.word(i % 16);
            now.word(0xabcdef00 + i);
        }
        Stream before = now;
        BundleMgr changed(&now);
        OldBundleMgr previous(&before);
        assert(changed.loadCompTable(0) && previous.loadCompTable(0));
        assert(now.pos == before.pos && now.pos == now.data.size());
        assert(now.seeks == 1 && before.seeks == count + 2);
        assert(changed._lastBlockDecompressedSize == previous._lastBlockDecompressedSize);
        for (int i = 0; i < count; ++i) {
            assert(changed._compTable[i].offset == previous._compTable[i].offset);
            assert(changed._compTable[i].size == previous._compTable[i].size);
            assert(changed._compTable[i].codec == previous._compTable[i].codec);
        }
    }
    Stream raw;
    raw.word(MKTAG('i','M','U','S'));
    BundleMgr uncompressed(&raw);
    assert(uncompressed.loadCompTable(0) && uncompressed._isUncompressed);
    Stream invalid;
    for (uint32 w : {0u, 1u, 0u, 100u}) invalid.word(w);
    BundleMgr bad(&invalid);
    assert(!bad.loadCompTable(0) && !bad._compTable);
    puts("PASS: actual SCI size cache/bounds checks and BUN table equivalence; 4098 -> 1 seeks for 4096 entries (host mocks)");
}
'''
# Ensure the tested class is used only by the existing owned file cache.
factory = sci[sci.index("Common::SeekableReadStream *ResourceManager::getVolumeFile("):
              sci.index("void ResourceManager::disposeVolumeFileStream(")]
assert "file = new WiiUCachedVolumeFile;" in factory
assert "return source->_resourceFile->createReadStream();" in factory
assert "return res ? res->makeStream() : nullptr;" in factory
assert "volumeFile->size()" in sci[sci.index("Resource *ResourceManager::updateResource("):]
with tempfile.TemporaryDirectory(prefix="scummvm-io-fix-") as tmp:
    path = Path(tmp)
    (path / "test.cpp").write_text(prefix + cached + validate + table + old_table + checks)
    binary = path / "test"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-Werror",
                    "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
                    str(path / "test.cpp"), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
