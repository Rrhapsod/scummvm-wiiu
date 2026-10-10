#include "backends/platform/sdl/wiiu/wiiu-log.h"
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

static bool failSync = false;
static unsigned syncCalls = 0;
extern "C" int __real_fsync(int fd);
extern "C" int __wrap_fsync(int fd) {
	++syncCalls;
	if (failSync) { errno = EIO; return -1; }
	return __real_fsync(fd);
}

static std::string readFile(const char *path) {
	FILE *file = fopen(path, "rb");
	assert(file);
	char buffer[64] = {};
	size_t size = fread(buffer, 1, sizeof(buffer), file);
	fclose(file);
	return std::string(buffer, size);
}

int main() {
	char path[] = "/tmp/scummvm-log-test-XXXXXX";
	int fd = mkstemp(path);
	assert(fd >= 0);
	close(fd);
	StdioStream *stream = openWiiUDiagnosticLog(path);
	assert(stream && stream->write("session1\n", 9) == 9 && stream->flush());
	assert(syncCalls == 1 && readFile(path) == "session1\n");
	delete stream;
	stream = openWiiUDiagnosticLog(path);
	assert(stream && stream->write("session2\n", 9) == 9 && stream->flush());
	assert(syncCalls == 2 && readFile(path) == "session1\nsession2\n");
	failSync = true;
	assert(!stream->flush() && syncCalls == 3); // Propagate failure, no recursive log.
	delete stream;
	assert(openWiiUDiagnosticLog(std::string(path) + "/not-a-directory") == nullptr);
	unlink(path);
	puts("PASS: diagnostic append, flush+fsync, visibility before close, error propagation (host, not SD power-loss test)");
}
