# ScummVM Wii U 0.9.0

This community release for Aroma runs 19 engine families on the ScummVM
2026.4.0 base. It adds four engines to the 0.8.0 lineup.

| New engine | Wii U scope |
| --- | --- |
| GrimE | Original Grim Fandango and Escape from Monkey Island (Monkey Island 4), using TinyGL software rendering |
| Stark | The Longest Journey with original artwork. On Wii U the engine now starts directly with original assets even if a saved configuration enables external replacements. |
| Wintermute | Supported 2D titles. Wintermute 3D, including Alpha Polaris, requires further renderer work. |
| Adventure Game Studio (AGS) | Supported AGS game editions, including Gemini Rue, the Blackwell series and the original Kathy Rain |

FLAC supports lossless tracks and compressed speech where the game engine can
read them. For General MIDI, select FluidSynth and supply your own SF2 SoundFont.
PNG, FreeType, Bink, MPEG-2 and Theora support is available to the engines
that use those formats. The port retains MT-32 emulation, GamePad touch,
Wii Remote IR pointing and FAT32 USB game loading from 0.8.0.

## Installation

For Aroma, download **ScummVM-wiiu-0.9.0-aroma.zip**. Extract the contents to
the SD root and merge the folders, preserving your configuration and saves.
Launch ScummVM from the Wii U Menu. When upgrading, copy both
`wiiu/apps/ScummVM.wuhb` and the bundled `scummvm/data/` directory. The new
engines use files including `grim-patch.lab`, `monkey4-patch.m4b`,
`wintermute.zip` and `fonts.dat`. Supply your own game data.

**ScummVM-wiiu-0.9.0-rpx-experimental.zip** is a separate download for an
existing RPX-capable Homebrew Launcher. It contains the same RPX as the Aroma
package. Operation outside Aroma still needs hardware testing. Package hashes
and build information accompany the downloads.

## Wii U testing

I ran Grim Fandango (Windows, Brazilian Portuguese) on revision `e21b8136`
and The Longest Journey (GOG, Windows, English) on revision `a817fa1d`.
The latter session reached engine teardown and closed its log cleanly.
The release carries the same engine code as that successful Stark test and
uses a final version label. Other editions, complete playthroughs and the
final packaged binary still need separate console coverage.
