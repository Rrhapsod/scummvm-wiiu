// Version of the community Wii U port, independent of upstream ScummVM.
#ifndef WIIU_RELEASE_VERSION_H
#define WIIU_RELEASE_VERSION_H
#ifdef WIIU_FMV_BLIT_TEST
#define WIIU_PORT_VERSION "0.6.0 + FMV blit candidate 20261018-r2 (RGBA conversion / stage timings)"
#elif defined(WIIU_INPUT_FMV_TEST)
#define WIIU_PORT_VERSION "0.6.0 + input/FMVs candidate 20261018 (Pro keyboard / Groovie rendering)"
#elif defined(WIIU_MEDIA_TEST)
#define WIIU_PORT_VERSION "0.6.0 + media test 20261013 (keyboard editing / Ogg Vorbis / Groovie diagnostics)"
#elif defined(WIIU_IO_FIX_TEST)
#define WIIU_PORT_VERSION "0.6.0 + IO optimization candidate 20261012 (Curse / QFG4; timer fix retained)"
#elif defined(WIIU_TIMER_FIX_TEST)
#define WIIU_PORT_VERSION "0.6.0 + timer fix candidate 20261012 (SDL zero timeout; diagnostics enabled)"
#elif defined(WIIU_COMPAT_DIAGNOSTICS)
#define WIIU_PORT_VERSION "0.6.0 + compatibility diagnostics 20261012 (audio timer / SCI resources)"
#elif defined(WIIU_COMPATIBILITY_TEST)
#define WIIU_PORT_VERSION "0.6.0 + compatibility test 20261009 (SCUMM v7/v8, MM/Xeen, SCI/SCI32)"
#else
#define WIIU_PORT_VERSION "0.7.0"
#endif
#endif
