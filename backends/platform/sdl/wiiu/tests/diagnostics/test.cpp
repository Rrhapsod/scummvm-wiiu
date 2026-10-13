#include "common/system.h"
#include "backends/platform/sdl/wiiu/wiiu-diagnostics.h"
#include <cassert>
#include <cstdio>

OSystem *g_system = nullptr;

int main() {
	WIIU_TRACE("safe before system exists");
	OSystem system;
	g_system = &system;
#if defined(WIIU) && defined(WIIU_LIFECYCLE_DIAGNOSTICS) && defined(WIIU_COMPAT_DIAGNOSTICS)
	{ WIIU_PERF_SCOPE("fast", 1); system.now += 49; }
	assert(system.messages.empty());
	system.now = 0xfffffff0;
	{ WIIU_PERF_SCOPE("wrapped", 2); system.now += 50; }
	assert(system.messages.size() == 1);
	assert(system.messages.back().find("elapsedMs=50") != std::string::npos);
	for (int i = 0; i < 100; ++i) {
		WIIU_PERF_SCOPE("slow", i);
		system.now += 3000;
	}
	assert(system.messages.size() == 24);
	WIIU_TRACE_GAME("scumm", "reset-budget");
	{ WIIU_PERF_SCOPE("new-game", 0); system.now += 3000; }
	assert(system.messages.size() == 26);
	assert(system.messages.back().find("sample=1/24") != std::string::npos);
	g_system = nullptr;
	{ WIIU_PERF_SCOPE("null-system", 0); }
	assert(system.messages.size() == 26);
	std::puts("Performance scope tests passed (threshold, wraparound, cap, game reset, null)");
	return 0;
#else
	int perfSideEffects = 0;
	{ WIIU_PERF_SCOPE("disabled", ++perfSideEffects); }
	assert(perfSideEffects == 0);
#endif
#if defined(WIIU) && defined(WIIU_LIFECYCLE_DIAGNOSTICS)
	WIIU_TRACE("startup %d", 42);
	assert(system.messages.size() == 1);
	assert(system.messages[0] == "[WIIU-DIAG seq=1 cycle=0 ms=123] startup 42\n");
	WIIU_TRACE_GAME("scumm", "game1");
	assert(system.messages[1].find("seq=2 cycle=1") != std::string::npos);
	assert(system.messages[1].find("engine=scumm game=game1") != std::string::npos);
	WIIU_TRACE("first exit");
	system.now = 456;
	WIIU_TRACE_GAME("mohawk", "game2");
	assert(system.messages[3].find("seq=4 cycle=2 ms=456") != std::string::npos);
	std::string large(2000, 'x');
	WIIU_TRACE("%s", large.c_str());
	assert(system.messages.back().size() < 640);
	assert(system.messages.back().back() == '\n');
	assert(system.messages.back().find(std::string(511, 'x')) != std::string::npos);
	std::puts("Lifecycle diagnostic tests passed (enabled)");
#else
	int sideEffects = 0;
	WIIU_TRACE("%d", ++sideEffects);
	WIIU_TRACE_GAME((++sideEffects, "engine"), (++sideEffects, "game"));
	assert(sideEffects == 0);
	assert(system.messages.empty());
	std::puts("Lifecycle diagnostic tests passed (disabled, arguments not evaluated)");
#endif
}
