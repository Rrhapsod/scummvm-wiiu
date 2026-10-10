#ifndef TEST_WIIU_STDIOSTREAM_H
#define TEST_WIIU_STDIOSTREAM_H
#include <cstdio>
#include <string>
namespace Common { using String = std::string; }
// Only mock ScummVM's wrapper; fopen/fwrite/fflush/fsync use the host OS.
class StdioStream {
protected:
	void *_handle;
public:
	explicit StdioStream(void *file) : _handle(file) {}
	virtual ~StdioStream() { fclose(static_cast<FILE *>(_handle)); }
	virtual bool flush() { return fflush(static_cast<FILE *>(_handle)) == 0; }
	size_t write(const void *p, size_t n) { return fwrite(p, 1, n, static_cast<FILE *>(_handle)); }
};
#endif
