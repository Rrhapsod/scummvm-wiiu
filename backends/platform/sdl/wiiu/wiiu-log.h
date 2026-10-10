/* Wii U diagnostic log persistence. GPL-3.0-or-later; see COPYING. */
#ifndef PLATFORM_SDL_WIIU_LOG_H
#define PLATFORM_SDL_WIIU_LOG_H
#include "backends/fs/stdiostream.h"
// Append, never truncate; only used by the opt-in diagnostic build.
StdioStream *openWiiUDiagnosticLog(const Common::String &path);
#endif
