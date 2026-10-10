/* Wii U diagnostic log persistence. GPL-3.0-or-later; see COPYING. */
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "backends/platform/sdl/wiiu/wiiu-log.h"
#include <stdio.h>
#include <unistd.h>

class WiiUDiagnosticLogStream : public StdioStream {
public:
	explicit WiiUDiagnosticLogStream(FILE *file) : StdioStream(file) {}

	bool flush() override {
		// fflush only drains libc. WUT's fsync calls FSAFlushFile on the SD.
		// Do not issue warnings here: the warning would recursively use this log.
		if (!StdioStream::flush())
			return false;
		return fsync(fileno(static_cast<FILE *>(_handle))) == 0;
	}
};

StdioStream *openWiiUDiagnosticLog(const Common::String &path) {
	FILE *file = fopen(path.c_str(), "ab");
	return file ? new WiiUDiagnosticLogStream(file) : nullptr;
}
