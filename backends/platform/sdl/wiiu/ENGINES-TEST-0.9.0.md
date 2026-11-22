# ScummVM Wii U 0.9.0 — engine/media candidate

I am integrating these additions on the existing ScummVM 2026.4.0 base. This
is a hardware test candidate, not the final 0.9.0 release. I have not tested
this binary on a Wii U or in Cemu. Engine detection and successful compilation
do not establish playability, performance, or a complete playthrough.

| Addition | Candidate scope | Games to test |
| --- | --- | --- |
| GrimE (`grim`, `monkey4`) | TinyGL software 3D | Original Grim Fandango; original Escape from Monkey Island |
| Stark (`stark`) | TinyGL software 3D, FreeType, Vorbis, Bink | The Longest Journey |
| Wintermute (`wintermute`) | Software 2D | Supported 2D Wintermute games |
| AGS (`ags`) | Software rendering, original supported editions | Indiana Jones and the Temple of Spheres, Heroine's Quest, Blackwell series, Kathy Rain, Gemini Rue |

All 15 engine families from 0.8.0 remain enabled. Pointer stabilization,
FAT32 USB browsing, default save/configuration paths and the existing SDL2
input/audio/lifecycle implementation are retained. See README.md for controls
and the 0.8.0 engine table. No games, SoundFonts or Roland ROMs are included.

**Wintermute 3D / Alpha Polaris remains pending.** This upstream snapshot
explicitly disables the TinyGL 3D constructor in `BaseGame::initialize2()`;
enabling `wme3d` alone would expose games without a usable renderer. I keep
that subengine disabled. Alpha Polaris needs a separate renderer integration
and validation milestone. The optional FoxTail/HeroCraft forks are also off.

GrimE and Stark rasterize 3D on the CPU and present through the existing SDL
surface. SDL still owns GX2 and ProcUI. This candidate adds no hardware 3D
acceleration or OpenGL context. Test original game editions; remasters and
Kathy Rain: Director's Cut do not become supported through the AGS engine.

New media support:

- FLAC 1.5.0: FLAC audio tracks and engine-supported compressed speech.
- libpng 1.6.59: PNG images; existing JPEG remains enabled.
- FluidSynth 2.5.6: General MIDI with a user-supplied `.sf2`. Select FluidSynth
  as the music device and choose the SoundFont in ScummVM's audio settings.
  Start with a small SoundFont; large banks consume RAM. MT-32 remains a
  separate device and still needs the user's own Roland ROMs.
- libmpeg2 0.5.1 and Theora 1.2.0: optional video decoders where the engine
  supports those formats. Bink and existing built-in decoders remain enabled.
  This does not add arbitrary FFmpeg formats or convert unsupported videos.
- FreeType 2.14.1: font rendering required by Stark.

Merge the contents of the Aroma ZIP into the SD root. Preserve existing
configuration/saves and keep the previous WUHB as a fallback. Keep the matching
`scummvm/data/` resources, including `grim-patch.lab`, `monkey4-patch.m4b`,
`wintermute.zip`, `fonts.dat`, `mm.dat`, `kyra.dat` and `darkseed.dat`.

For each tested game, record the exact candidate revision from the log,
edition/language, controller, SD/USB path and scene. Check intro video/audio,
interactive controls, one scene change, save/load, return to launcher, relaunch
and HOME/exit. For Grim/Monkey4, test movement and inventory separately from
mouse navigation. Measure perceived speed/audio breakup on representative
scenes; a compile result supplies no FPS measurement.

For media, identify a real FLAC track/voice asset, an MPEG-2 cutscene, a Theora
cutscene and the exact SF2 bank used. Check seeking/skipping, stereo channels,
music changes and repeated game exit/reentry. Also run one previously working
2D game to check Pointer/keyboard/USB regressions. Copy
`scummvm/scummvm.log` before relaunch: the normal log is overwritten.

The separate RPX ZIP remains experimental; successful WUHB packaging does not
validate a legacy loader. The final 0.9.0 release depends on hardware reports
and fixes for any reproduced failures.
