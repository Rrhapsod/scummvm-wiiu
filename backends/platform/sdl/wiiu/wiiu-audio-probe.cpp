/* Opt-in Wii U callback progress counters. GPL-3.0-or-later; see COPYING. */
#include "backends/platform/sdl/wiiu/wiiu-audio-probe.h"

#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)
#include "backends/platform/sdl/wiiu/wiiu-diagnostics.h"

static_assert(sizeof(unsigned int) == 4, "Audio probes require 32-bit counters");
static_assert(__atomic_always_lock_free(sizeof(unsigned int), nullptr),
              "Audio probes must never acquire a hidden lock");
static unsigned int counters[kWiiUAudioProbeCount] = {};

void wiiuAudioProbeHit(WiiUAudioProbeEvent event) {
	__atomic_fetch_add(&counters[event], 1U, __ATOMIC_RELAXED);
}

void wiiuAudioProbeReport() {
	unsigned int values[kWiiUAudioProbeCount];
	for (int i = 0; i < kWiiUAudioProbeCount; ++i)
		values[i] = __atomic_load_n(&counters[i], __ATOMIC_RELAXED);
	WIIU_TRACE("AUDIO progress timer=%u/%u imuse=%u/%u paused=%u reentered=%u wave=%u/%u/%u (enter/leave; wave enter/locked/leave; process totals)",
		values[kWiiUTimerEnter], values[kWiiUTimerLeave],
		values[kWiiUIMuseEnter], values[kWiiUIMuseLeave],
		values[kWiiUIMusePaused], values[kWiiUIMuseReentered],
		values[kWiiUWaveEnter], values[kWiiUWaveLocked], values[kWiiUWaveLeave]);
}
#endif
