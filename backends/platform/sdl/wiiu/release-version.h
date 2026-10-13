// Version of the community Wii U port, independent of upstream ScummVM.
#ifndef WIIU_RELEASE_VERSION_H
#define WIIU_RELEASE_VERSION_H
#ifdef WIIU_IO_FIX_TEST
#define WIIU_PORT_VERSION "0.6.0 + IO optimization candidate 20261012 (Curse / QFG4; timer fix retained)"
#elif defined(WIIU_TIMER_FIX_TEST)
#define WIIU_PORT_VERSION "0.6.0 + timer fix candidate 20261012 (SDL zero timeout; diagnostics enabled)"
#elif defined(WIIU_COMPAT_DIAGNOSTICS)
#define WIIU_PORT_VERSION "0.6.0 + compatibility diagnostics 20261012 (audio timer / SCI resources)"
#elif defined(WIIU_COMPATIBILITY_TEST)
#define WIIU_PORT_VERSION "0.6.0 + compatibility test 20261009 (SCUMM v7/v8, MM/Xeen, SCI/SCI32)"
#else
#define WIIU_PORT_VERSION "0.6.0"
#endif
#endif
