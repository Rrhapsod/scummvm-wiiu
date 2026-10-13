/* ScummVM Wii U lifecycle diagnostics. GPL-3.0-or-later; see COPYING. */
#ifndef PLATFORM_SDL_WIIU_DIAGNOSTICS_H
#define PLATFORM_SDL_WIIU_DIAGNOSTICS_H

// Opt-in build only. Call on the main thread, never from audio/timer callbacks.
#if defined(WIIU) && defined(WIIU_LIFECYCLE_DIAGNOSTICS)
void wiiuTrace(const char *format, ...);
void wiiuTraceBeginGame(const char *engine, const char *game);
#define WIIU_TRACE(...) wiiuTrace(__VA_ARGS__)
#define WIIU_TRACE_GAME(engine, game) wiiuTraceBeginGame(engine, game)
#if defined(WIIU_COMPAT_DIAGNOSTICS)
// Main-thread scopes only. Measures work, never user time inside a dialog.
class WiiUPerformanceScope {
	const char *_phase;
	int _detail;
	unsigned int _start;
public:
	WiiUPerformanceScope(const char *phase, int detail);
	~WiiUPerformanceScope();
};
#define WIIU_PERF_SCOPE(phase, detail) WiiUPerformanceScope wiiuPerfScope(phase, detail)
#else
#define WIIU_PERF_SCOPE(phase, detail) do {} while (0)
#endif
#else
#define WIIU_TRACE(...) do {} while (0)
#define WIIU_TRACE_GAME(engine, game) do {} while (0)
#define WIIU_PERF_SCOPE(phase, detail) do {} while (0)
#endif

#endif
