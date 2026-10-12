/* Opt-in Wii U callback progress counters. GPL-3.0-or-later; see COPYING. */
#ifndef PLATFORM_SDL_WIIU_AUDIO_PROBE_H
#define PLATFORM_SDL_WIIU_AUDIO_PROBE_H

#if defined(WIIU) && defined(WIIU_COMPAT_DIAGNOSTICS)
enum WiiUAudioProbeEvent {
	kWiiUTimerEnter, kWiiUTimerLeave,
	kWiiUIMuseEnter, kWiiUIMuseLeave, kWiiUIMusePaused, kWiiUIMuseReentered,
	kWiiUWaveEnter, kWiiUWaveLocked, kWiiUWaveLeave,
	kWiiUAudioProbeCount
};
// Callback-safe: lock-free counters only, no allocation, clock reads or IO.
void wiiuAudioProbeHit(WiiUAudioProbeEvent event);
// Main thread only. Cumulative process totals, not a transactional snapshot.
void wiiuAudioProbeReport();
#define WIIU_AUDIO_HIT(event) wiiuAudioProbeHit(event)
#define WIIU_AUDIO_REPORT() wiiuAudioProbeReport()
#else
#define WIIU_AUDIO_HIT(event) do {} while (0)
#define WIIU_AUDIO_REPORT() do {} while (0)
#endif
#endif
