#include "common/system.h"
#include "backends/platform/sdl/wiiu/wiiu-diagnostics.h"
#include <cassert>
#include <cstdio>

OSystem *g_system = nullptr;

int main() {
	WIIU_TRACE("safe before system exists");
	OSystem system;
	g_system = &system;
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
