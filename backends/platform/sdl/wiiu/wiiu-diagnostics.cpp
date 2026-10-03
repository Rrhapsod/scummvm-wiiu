/* ScummVM Wii U lifecycle diagnostics. GPL-3.0-or-later; see COPYING. */
#include "backends/platform/sdl/wiiu/wiiu-diagnostics.h"

#if defined(WIIU) && defined(WIIU_LIFECYCLE_DIAGNOSTICS)
#include "common/system.h"
#include <stdarg.h>
#include <stdio.h>

static unsigned long traceSequence = 0;
static unsigned long gameCycle = 0;

void wiiuTrace(const char *format, ...) {
	if (!g_system)
		return;
	char detail[512];
	va_list args;
	va_start(args, format);
	vsnprintf(detail, sizeof(detail), format, args);
	va_end(args);
	char line[640];
	snprintf(line, sizeof(line), "[WIIU-DIAG seq=%lu cycle=%lu ms=%lu] %s\n",
	         ++traceSequence, gameCycle, (unsigned long)g_system->getMillis(), detail);
	// Unlike debug(), this is independent of debuglevel. SDL's file logger
	// flushes each message; no callback logging or extra locking is introduced.
	g_system->logMessage(LogMessageType::kInfo, line);
}

void wiiuTraceBeginGame(const char *engine, const char *game) {
	++gameCycle;
	wiiuTrace("GAME begin engine=%s game=%s", engine, game);
}
#endif
