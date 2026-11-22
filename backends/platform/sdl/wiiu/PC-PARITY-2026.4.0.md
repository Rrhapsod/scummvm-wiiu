# ScummVM 2026.4.0 desktop parity on Wii U

**Baseline checked on 2026-11-22.** I compared the released 64-bit Ubuntu and Windows packages with the exact 0.9.0 Aroma candidate. Both PCs contain **113 runtime engine families**. The Wii U links **19 of those families**, leaving **94 families to integrate**. Several of the 19 still have missing subengines or media paths. Family counts measure the executable’s contents. Game compatibility and speed need a game edition, a scene and a hardware result.

| What is being counted | Desktop reference | Wii U candidate |
| --- | --- | --- |
| Game engines | 113 runtime plugins on Ubuntu. The same 113 Windows runtime class names are present in the executable. | 19 statically linked, with the distribution and source pinned below |
| Detection | Ubuntu reports 126 detection plugins. Thirteen can recognize data for families that lack a runtime in this release binary. | Static detection for the enabled families only |
| Optional subengines | The tag declares 28 switches under 13 families. Their upstream defaults and dependencies appear below. | 10 switches on, 18 off. The switches do not increase the family count. |
| Hardware evidence | I ran inventory commands on Ubuntu and inspected the Windows executable. | Cross-build and package evidence from the candidate. Exact-candidate game tests are pending at this baseline. |

[Installed families](#in-both-desktop-references-and-the-wii-u-candidate-19) · [94 missing families](#in-both-desktop-references-missing-from-the-wii-u-candidate-94) · [Subengines](#all-28-subengine-switches) · [Features](#what-changes-for-a-player) · [Shared components](#shared-components-used-by-engines) · [Porting priorities](#porting-priorities) · [How to refresh](#how-to-refresh-this-inventory).

## Reference builds

| Evidence | Identity and use |
| --- | --- |
| Upstream source | Tag `v2026.4.0`, commit `7d2709da5136866589f7602f5f5a7b17de03420e`. I used the tag's engine manifests, user manual, configure rules and release notes. [Source at the tag](https://github.com/scummvm/scummvm/tree/v2026.4.0) · [Release notes](https://github.com/scummvm/scummvm/blob/v2026.4.0/NEWS.md) |
| Linux PC | Official Ubuntu 24.04 amd64 DEB, SHA-256 `e9964e35d2752f34f8e5fda9a61955c3ec55ba7391dc0933e54aa375c69bcf5b`. I verified the download against the published hash, extracted it and queried its executable with SDL dummy video/audio. [Official DEB](https://downloads.scummvm.org/frs/scummvm/2026.4.0/scummvm_2026.4.0-1_ubuntu24_04_amd64.deb) |
| Windows PC | Official 64-bit portable ZIP, SHA-256 `fb60cfc0828ed86ef24a60c04b9390676493ab85892d81c56d1de90367d56736`. I checked its published hash, embedded build-feature string and compiled C++ runtime types. Windows gameplay and device enumeration were outside this audit. [Official ZIP](https://downloads.scummvm.org/frs/scummvm/2026.4.0/scummvm-2026.4.0-win32-x86_64.zip) · [Published package hashes](https://www.scummvm.org/pt-BR/downloads/) |
| Wii U | Runtime `96242f5f89e7954e2cfd472e96fb8693a3b26e08`. Aroma ZIP SHA-256 `cf0ae299308f69d39b67c1a8387c0e163f9dddc14a4e8e1f841c858730d1a6b0`. Local build directory `work/build-scummvm-wiiu-0.9.0-test-20261122/` under the outer project contains `config.h`, `config.mk`, plugin tables, staged data and `BUILD-INFO.txt`. [Candidate guide](ENGINES-TEST-0.9.0.md) · [Media profile](MEDIA-SDK.md) |

For an individual title, use the release compatibility entry and its notes on language, edition, known issues and required data. The project describes its chart primarily in terms of English game versions. A registered ID, including one printed by `--list-games`, can cover demos, partially supported games or a disabled subengine. In particular, desktop 2026.4.0 explicitly added **Alpha Polaris**. The Wii U still needs Wintermute 3D to run it. [2026.4.0 compatibility chart](https://www.scummvm.org/compatibility/2026.4.0/) · [Release announcement](https://www.scummvm.org/news/20261011/)

## Engine inventory

Each family appears once. The engine ID is the configure key. Game examples come from the Linux runtime-plugin game list (from the detector list for the 13 detector-only entries) and help identify families with unfamiliar names. A dash in a dependency column means that the parent manifest lists no additional feature there. Engine-specific resources, unsupported game revisions, byte order, RAM, or code inside the engine can still affect a port. The required and optional columns retain upstream IDs for searching the source. [Engine manifests at the tag](https://github.com/scummvm/scummvm/tree/v2026.4.0/engines) · [Dependency and component selection](https://github.com/scummvm/scummvm/blob/v2026.4.0/engines.awk)

### In both desktop references and the Wii U candidate (19)

| Engine ID | Upstream name | Example from PC game list | Required features | Optional components |
| --- | --- | --- | --- | --- |
| `agos` | AGOS | Simon the Sorcerer | — | `midi`, `fmtowns_pc98_audio` |
| `ags` | Adventure Game Studio | Gemini Rue | `16bit`, `mad` | `theoradec`, `midi`, `universaltracker`, `mpeg2` |
| `bladerunner` | Blade Runner | Blade Runner | `16bit`, `highres` | — |
| `darkseed` | Darkseed | Darkseed | `highres` | `midi` |
| `gob` | Gobli*ns | Gobliiins | `indeo3` | `midi` |
| `grim` | Grim | Grim Fandango | `16bit`, `3d`, `highres`, `hmi_audio` | `theoradec`, `tinygl`, `mpeg2` |
| `groovie` | Groovie | The 7th Guest | `highres` | — |
| `kyra` | Kyra | The Legend of Kyrandia | — | `midi`, `fmtowns_pc98_audio` |
| `mm` | Might and Magic | Might and Magic: World of Xeen | `midi` | — |
| `mohawk` | Mohawk | Myst | `highres` | — |
| `saga` | SAGA | Inherit the Earth: Quest for the Orb | — | `midi`, `fmtowns_pc98_audio` |
| `sci` | SCI | King's Quest VI: Heir Today, Gone Tomorrow | — | `midi`, `fmtowns_pc98_audio` |
| `scumm` | SCUMM | The Secret of Monkey Island | — | `midi`, `fmtowns_pc98_audio`, `sid_audio`, `imgui` |
| `sherlock` | The Lost Files of Sherlock Holmes | The Lost Files of Sherlock Holmes: The Case of the Serrated Scalpel | — | `midi` |
| `stark` | The Longest Journey | The Longest Journey | `16bit`, `3d`, `highres`, `freetype2`, `vorbis`, `bink` | `tinygl` |
| `sword1` | Broken Sword | Broken Sword: The Shadow of the Templars | `highres` | `mpeg2` |
| `sword2` | Broken Sword II | Broken Sword II: The Smoking Mirror | `highres` | `mpeg2` |
| `tinsel` | Tinsel | Discworld | — | `midi`, `tinygl` |
| `wintermute` | Wintermute | Alpha Polaris (3D pending on Wii U) | `16bit`, `highres`, `jpeg`, `png` | `theoradec`, `tinygl` |

### In both desktop references, missing from the Wii U candidate (94)

| Engine ID | Upstream name | Example from PC game list | Required features | Optional components |
| --- | --- | --- | --- | --- |
| `access` | Access | Amazon: Guardians of Eden | — | `midi` |
| `adl` | ADL | Hi-Res Adventure #0: Mission Asteroid | `16bit`, `highres` | — |
| `agi` | AGI | Sierra AGI game | — | `midi` |
| `alcachofa` | Alcachofa | Mort & Phil: A Movie Adventure | `highres`, `mpeg2`, `3d` | `tinygl` |
| `alg` | American Laser Games | Mad Dog McCree | — | — |
| `asylum` | Sanitarium | Sanitarium | `highres` | `theoradec` |
| `avalanche` | Lord Avalot d'Argent | Lord Avalot d'Argent | `highres` | — |
| `awe` | Another World | Another World - Out of this World | — | — |
| `bagel` | BAGEL | The Space Bar | `16bit`, `highres`, `freetype2`, `mfc` | `midi` |
| `bbvs` | Beavis and Butthead in Virtual Stupidity | Beavis and Butt-Head in Virtual Stupidity | `indeo3` | — |
| `bolt` | Bolt | Cartoon Carnival | — | — |
| `buried` | The Journeyman Project 2: Buried in Time | The Journeyman Project 2: Buried in Time | `highres`, `freetype2` | — |
| `cge` | CGE | Sołtys | — | `midi` |
| `cge2` | CGE2 | Sfinx | — | `midi` |
| `chamber` | Chamber | Chamber of the Sci-Mutant Priestess | — | — |
| `chewy` | Chewy: Esc from F5 | Chewy: Esc from F5 | — | — |
| `cine` | Cinematique evo 1 | Future Wars | — | `midi` |
| `composer` | Magic Composer | Magic Tales: Baba Yaga and the Magic Geese | `highres` | — |
| `crab` | CRAB | Unrest | `16bit`, `highres`, `png`, `freetype2`, `vorbis` | — |
| `cruise` | Cinematique evo 2 | Cruise for a Corpse | — | — |
| `cryomni3d` | Cryo Omni3D games | Versailles 1685 | `highres`, `hnm` | — |
| `dgds` | Dynamix Game Development System | Rise of the Dragon | — | `midi`, `fmtowns_pc98_audio` |
| `director` | Macromedia Director | Spaceship Warlock | `highres` | `imgui`, `indeo3`, `qdm2`, `svq1` |
| `dm` | Dungeon Master | Dungeon Master | — | — |
| `draci` | Dragon History | Dračí Historie | — | `midi` |
| `dragons` | Blazing Dragons | Blazing Dragons | `16bit`, `vgmtrans_audio` | — |
| `drascula` | Drascula: The Vampire Strikes Back | Drascula: The Vampire Strikes Back | — | — |
| `dreamweb` | Dreamweb | DreamWeb | — | — |
| `eem` | Eagle Eye Mysteries | Eagle Eye Mysteries | — | — |
| `efh` | Escape From Hell | Escape From Hell | — | — |
| `fool` | The Fool's Errand | The Fool's Errand | `highres`, `mactoolbox` | — |
| `freescape` | Freescape | Driller | `highres`, `16bit`, `3d`, `unp64` | `tinygl`, `sid_audio` |
| `gamos` | Gamos | Gamos | `highres` | `midi` |
| `glk` | Glk Interactive Fiction games | Mini Zork I: The Great Underground Empire | `16bit`, `freetype2`, `jpeg`, `png`, `unp64` | — |
| `gnap` | UFOs | U.F.O.s | `highres`, `16bit` | `midi` |
| `got` | Got | God of Thunder | — | — |
| `griffon` | The Griffon Legend | The Griffon Legend | `16bit` | — |
| `hadesch` | Hades Challenge | Hades Challenge | `highres` | — |
| `harvester` | Harvester | Harvester | `highres` | — |
| `hdb` | Hyperspace Delivery Boy! | Hyperspace Delivery Boy! | `16bit`, `highres`, `lua` | — |
| `hopkins` | Hopkins FBI | Hopkins FBI | `16bit`, `highres` | — |
| `hpl1` | Hpl1 | Penumbra: Overture | `16bit`, `3d`, `highres`, `jpeg`, `gif`, `png`, `opengl_game_shaders` | `tinygl` |
| `hugo` | Hugo Trilogy | Hugo's House of Horrors | — | `midi` |
| `hypno` | Hypnotix Inc. | Marvel Comics Spider-Man: The Sinister Six | — | — |
| `illusions` | Illusions Engine | Beavis and Butt-Head: Do U. | — | `midi` |
| `kingdom` | Kingdom: The Far Reaches | Kingdom: The Far Reaches | — | — |
| `lab` | Labyrinth of Time | The Labyrinth of Time | — | — |
| `lastexpress` | The Last Express | The Last Express | `16bit`, `highres` | — |
| `lure` | Lure of the Temptress | Lure of the Temptress | — | `midi` |
| `m4` | M4 | Ripley's Believe It or Not!: The Riddle of Master Lu | `highres` | `midi` |
| `macs2` | Macs2 | Der Schatz im Silbersee | `imgui`, `midi` | — |
| `macventure` | MacVenture | Shadowgate | `highres` | — |
| `made` | MADE | The Manhole | — | `midi` |
| `mads` | MADS | Dragonsphere | — | `midi` |
| `mortevielle` | Mortevielle | Mortville Manor | `highres` | — |
| `mtropolis` | mTropolis | Obsidian | `16bit`, `highres` | `midi`, `mpeg2` |
| `myst3` | Myst 3 | Myst III: Exile | `16bit`, `3d`, `highres`, `jpeg`, `bink` | `tinygl` |
| `nancy` | Nancy Drew | Nancy Drew: The Secret of Shadow Ranch | `16bit`, `highres`, `vorbis`, `bink` | — |
| `neverhood` | Neverhood | The Neverhood | `highres` | — |
| `ngi` | Nikita Game Interface | Full Pipe | `16bit`, `highres` | `indeo45` |
| `parallaction` | Parallaction | Nippon Safes, Inc. | — | `midi` |
| `pegasus` | The Journeyman Project: Pegasus Prime | The Journeyman Project: Pegasus Prime | `16bit`, `highres` | `theoradec` |
| `pelrock` | Alfred Pelrock | Alfred Pelrock: En Busca de un Sueño | `highres`, `png` | — |
| `petka` | Red Comrades | Red Comrades 1: Save the Galaxy | `highres`, `16bit`, `freetype2` | — |
| `phoenixvr` | Phoenix VR | Amerzone: The Explorer's Legacy | `highres`, `16bit`, `gif` | — |
| `pink` | Pink Panther | The Pink Panther: Passport to Peril | `highres` | — |
| `plumbers` | Plumbers Don't Wear Ties | Plumbers Don't Wear Ties! | — | — |
| `prince` | The Prince and The Coward | The Prince and the Coward | `highres` | `midi` |
| `private` | Private Eye | Private Eye | `highres` | — |
| `qdengine` | QD Engine | Pilot Brothers 3: Back Side of the Earth | `vorbis`, `16bit`, `highres`, `mpeg2` | `imgui`, `mpc`, `mpeg2` |
| `queen` | Flight of the Amazon Queen | Flight of the Amazon Queen | — | `midi` |
| `saga2` | SAGA2 | Dinotopia | `highres` | `midi` |
| `sky` | Beneath a Steel Sky | Beneath a Steel Sky | — | `midi` |
| `sludge` | Sludge | Out Of Order | `16bit`, `png` | `vpx`, `universaltracker` |
| `supernova` | Mission Supernova | Mission Supernova - Part 1: The fate of Horst Hummel | — | — |
| `sword25` | Broken Sword 2.5 | Broken Sword 2.5: The Return of the Templars | `png`, `16bit`, `highres`, `theoradec`, `lua` | — |
| `teenagent` | Teen Agent | Teen Agent | — | — |
| `tetraedge` | Tetraedge | Syberia | `highres`, `3d`, `freetype2`, `vorbis`, `png`, `jpeg`, `lua`, `theoradec` | `tinygl` |
| `titanic` | Starship Titanic | Starship Titanic | `16bit`, `jpeg`, `highres`, `mad`, `indeo45` | — |
| `toltecs` | 3 Skulls of the Toltecs | 3 Skulls of the Toltecs | `highres` | `midi` |
| `tony` | Tony Tough and the Night of Roasted Moths | Tony Tough and the Night of Roasted Moths | `16bit`, `highres` | — |
| `toon` | Toonstruck | Toonstruck | `highres` | — |
| `tot` | Tot | Trick or Treat | — | — |
| `touche` | Touche: The Adventures of the Fifth Musketeer | Touché: The Adventures of the Fifth Musketeer | `highres` | `midi` |
| `trecision` | Trecision Adventure Module | Ark of Time | `highres`, `16bit` | — |
| `tsage` | TsAGE | Ringworld: Revenge of the Patriarch | — | `midi` |
| `tucker` | Bud Tucker in Double Trouble | Bud Tucker in Double Trouble | — | — |
| `twine` | Little Big Adventure | Little Big Adventure | `highres` | `imgui`, `midi`, `gif` |
| `twp` | Thimbleweed Park | Thimbleweed Park | `16bit`, `3d`, `highres`, `vorbis`, `png`, `opengl_game_shaders` | `imgui` |
| `ultima` | Ultima | Akalabeth: World of Doom | `midi` | — |
| `vcruise` | V-Cruise | A.D. 2044 | `16bit`, `highres` | `midi` |
| `voyeur` | Voyeur | Voyeur | — | — |
| `wage` | WAGE | Another Fine Mess | `highres` | `imgui` |
| `zvision` | Z-Vision | Zork Nemesis: The Forbidden Lands | `freetype2`, `16bit`, `highres` | `midi`, `truemotion1`, `mpeg2` |

### Detector only on the Ubuntu reference, absent from the Wii U runtime (13)

| Engine ID | Upstream name | Example from PC game list | Required features | Optional components |
| --- | --- | --- | --- | --- |
| `agds` | AGDS (Black Mirror and NiBiRu) | NiBiRu: Age of Secrets | `highres`, `16bit`, `jpeg` | — |
| `colony` | The Colony | The Colony | `highres`, `16bit`, `3d` | — |
| `cryo` | Lost Eden | Lost Eden | `hnm` | — |
| `icb` | In Cold Blood | In Cold Blood | `16bit`, `highres`, `bink` | — |
| `immortal` | The Immortal | The Immortal | — | — |
| `lilliput` | Lilliput | The Adventures of Robin Hood | — | `midi` |
| `mediastation` | Media Station | George Shrinks Interactive Storybook | `highres` | — |
| `mutationofjb` | Mutation of JB | Mutation of J.B. | — | — |
| `playground3d` | Playground 3D: the testing and playground environment for 3d renderers | Playground 3d: the testing and playground environment for 3d renderers | `16bit`, `3d`, `highres` | `tinygl` |
| `startrek` | Star Trek 25th Anniversary/Judgment Rites | Star Trek: 25th Anniversary | — | `midi` |
| `testbed` | TestBed: the Testing framework | Testbed: The Backend Testing Framework | — | `imgui`, `midi`, `universaltracker`, `cdtoons`, `indeo3`, `indeo45`, `vpx`, `mpc`, `hnm`, `jyv1`, `mpeg2`, `msmpeg4`, `qdm2`, `svq1`, `truemotion1`, `xan`, `tinygl` |
| `watchmaker` | The Watchmaker | The Watchmaker | `highres`, `16bit`, `3d`, `opengl_game_classic` | — |
| `waynesworld` | Wayne's World | Wayne's World | — | — |

## All 28 subengine switches

The desktop family lists do not expose individual subengine switches. The upstream column gives the release source default before dependency checks. The Wii U column comes from the packaged candidate’s build configuration. A game ID can remain visible in a detection table when its subengine is off. The required and optional columns are **additional** to the parent’s requirements.

| Parent | Switch | Scope | Upstream default | Wii U build | Required | Optional |
| --- | --- | --- | --- | --- | --- | --- |
| `agos` | `agos2` | AGOS 2 games | yes | off | `highres` | — |
| `cryomni3d` | `versailles` | Versailles 1685 | yes | off | — | — |
| `grim` | `monkey4` | Escape from Monkey Island | no | on | `bink` | — |
| `groovie` | `groovie2` | Groovie 2 games | yes | on | `jpeg`, `16bit` | `midi`, `mpeg2` |
| `kyra` | `eob` | Eye of the Beholder | yes | on | — | — |
| `kyra` | `lol` | Lands of Lore | yes | off | — | — |
| `mm` | `mm1` | Might and Magic 1 | yes | off | — | — |
| `mm` | `xeen` | Might and Magic Xeen | yes | on | — | — |
| `mohawk` | `cstime` | Where in Time is Carmen Sandiego? | no | off | — | `cdtoons` |
| `mohawk` | `myst` | Myst | yes | on | — | — |
| `mohawk` | `mystme` | Myst ME | yes | on | `16bit`, `jpeg` | `qdm2`, `svq1` |
| `mohawk` | `riven` | Riven: The Sequel to Myst | yes | on | `16bit` | — |
| `saga` | `ihnm` | IHNM | yes | on | `highres` | — |
| `sci` | `sci32` | SCI32 games | yes | on | `highres` | `indeo3`, `truemotion1` |
| `scumm` | `he` | HE71+ games | yes | off | `highres`, `bink` | `enet` |
| `scumm` | `rebel2_psx` | Rebel Assault II PlayStation | no | off | `16bit`, `3d`, `tinygl` | — |
| `scumm` | `scumm_7_8` | v7 & v8 games | yes | on | — | — |
| `tsage` | `blueforce` | Blue Force | yes | off | — | — |
| `tsage` | `ringworld` | Ringworld: Revenge of the Patriarch | yes | off | — | — |
| `tsage` | `ringworld2` | Return to Ringworld | yes | off | — | — |
| `ultima` | `akalabeth` | Akalabeth | no | off | — | — |
| `ultima` | `ultima1` | Ultima I - The First Age of Darkness | no | off | — | — |
| `ultima` | `ultima4` | Ultima IV - Quest of the Avatar | yes | off | `16bit` | — |
| `ultima` | `ultima6` | Ultima VI = The False Prophet | yes | off | `highres`, `16bit`, `lua` | — |
| `ultima` | `ultima8` | Ultima VIII - Pagan | yes | off | `highres`, `16bit` | `imgui`, `jyv1`, `xan` |
| `wintermute` | `foxtail` | FoxTail | yes | off | — | — |
| `wintermute` | `herocraft` | HeroCraft | yes | off | — | — |
| `wintermute` | `wme3d` | Wintermute3D | yes | off | `3d` | — |

## What changes for a player

The middle column describes the tagged desktop feature or the package that supplies it. “Shared” in the Wii U column means the port uses the common ScummVM implementation. Each engine decides which options it exposes. A feature flagged “built” still needs its Wii U input, asset or output checked on hardware.

| Area | Desktop capability | Wii U 0.9.0 candidate |
| --- | --- | --- |
| Launcher and library | Add a game or mass-add a tree, edit/remove targets, list/grid with artwork, search including field filters, group by engine/language/series and other fields, start or resume a target. | Shared launcher and local file browser. Data can be on SD or FAT32 USB. Grid artwork uses local resources. Full detection for absent engines is off. |
| Settings and help | Global and per-target overrides, game ID/name/language/original platform, backend and engine options, Help, About, in-game global menu and game-specific help. | Shared dialogs. Themes, help archive and translations ship. Availability changes by engine and target edition. |
| Saves and sessions | In-game or launcher load, save slots, autosave and period, deletion, thumbnails, timestamps, play time, extended metadata, return to launcher, relaunch. Engines advertise these individually. | Shared save manager, timestamp support and configurable Save Path. SD is the default. USB save durability and the new engines’ save/load/reentry need physical tests. |
| Accessibility and game assistance | Speech/text/both, subtitle speed and volume, launcher scaling, hotspot markers/labels, per-game aids, TTS and selectable voice on supported builds. | Shared subtitle, scaling and engine-specific controls. TTS is off. Hotspot availability depends on the engine. |
| Localization and fonts | Translated GUI, automatic language handling, language-specific game data, fonts, bidirectional text. | GUI translations, language detection, font resources and FreeType are present. FriBidi is off. Ubuntu ships a separate CJK font archive that is absent from the Aroma ZIP. Check glyph coverage and text entry per language. |
| Graphics and output | Window/fullscreen, render palettes for original systems, aspect correction, filter and scale factor, stretch modes, scalers, shaders, screenshot path, VSync, arbitrary resolutions or 3D antialiasing where supported. | Fullscreen SDL presentation on TV/GamePad. RGB, high resolution, aspect, normal/HQ/Edge scaler support are built. Rendering options still depend on the active graphics manager. Test filters, screenshots, resolution and frame pacing on the candidate. |
| 3D and shaders | TinyGL CPU rendering and hardware OpenGL with shaders in both PC builds. Desktop can select a game renderer and apply GLSL post-processing presets. | TinyGL on the CPU for GrimE and Stark. SDL presents the result through GX2. OpenGL game contexts, GL shaders and desktop shader presets are unavailable. Shipping `shaders.dat` does not enable them. Wintermute 3D has no usable software path in this tag. |
| Input and navigation | Mouse, keyboard shortcuts, controller buttons/axes, hotplug, per-action keymaps, pointer speed and deadzone. Text consoles and virtual keyboards depend on the backend. | Shared keymapper, GamePad sticks/touch, Remote IR and Nunchuk mappings, Wii U native on-screen keyboard. Generic virtual-keyboard flag is off. Test native text fields, controller handoff and HOME/return in each new engine. [Controls and historical hardware reports](../../../../README.md) |
| Music and mixer | Separate music/effects/voice levels and mute, stereo PCM, output rate/channels, AdLib/OPL variants, GM/GS, MIDI gain, mixed AdLib/MIDI, MT-32 and SoundFont synth, reverb/chorus/interpolation. | SDL mixer defaults to 48 kHz. Software MIDI, Nuked OPL, MT-32 with user ROMs, and FluidSynth with user SF2 are built. Ubuntu bundles `Roland_SC-55.sf2`; the Wii U package supplies no bank. Native MIDI ports, SF3 and physical MIDI device playback are separate gaps. |
| Audio files and CD music | Raw PCM, WAV, AIFF, VOC, ADPCM, Mac snd/MACE, XA and engine-specific streams, ripped audio tracks and compressed speech. Physical CD audio depends on the desktop device/backend. | FLAC, MP3/libmad, Ogg Vorbis, QDM2 and supported in-game streams built. AAC/FAAD, Dolby Digital AC-3/A52 and Musepack are off. File-backed tracks from game data are the applicable CD path. No Wii U optical audio-CD workflow established. |
| Tracker and legacy music | Built-in module players and selectable PC speaker, PCjr, AdLib, CMS, C64, FM Towns, PC-98, Mac, Amiga, Sega CD or external devices depending on game. Windows also advertises MikMod and RetroWave. | FM Towns/PC-98 audio, SID, HMI, MIDI and built-in game-specific players available to enabled engines. External MikMod/OpenMPT both off. AGS games relying on the external tracker reader can therefore lose module music. |
| FMV and images | AVI, QuickTime/QTVR, Smacker, Bink, FLIC, DXA, 3DO, 4XM, Coktel, MVE, MPEG-PS, PSX and PACo have ScummVM readers. Container and stream codec must both be supported. VPX/Matroska-WebM and HNM have additional gates. | Shared video objects for those core readers plus MPEG-2 and Theora. JPEG/PNG, Cinepak, MJPEG, Indeo 3, TrueMotion 1 and SVQ1 are examples in this build. GIF, VPX, Indeo 4/5, MS MPEG-4 v3, HNM and other optional paths are off. MPEG-2/Theora game playback needs console evidence. |
| Images, archives and data access | Game loaders use files, directories, Mac resource/data forks, disk images or installer resources according to the engine. Shared support includes BMP, ICO/CUR, PCX, PICT, IFF, TGA, ZIP, InstallShield, StuffIt, ARJ, Clickteam, Gentee and VISE. | Common code and zlib available. Each new engine may require an extra data archive or font in the SD package. The 2026.4.0 InstallShield improvement benefits compatible game loaders. |
| Cloud saves and game downloads | Connect Dropbox, OneDrive, Google Drive or Box, synchronize saves, manage active storage and download game files where the provider supports it. Google Drive has a download limitation. | Cloud and libcurl off. Save exchange uses local files. |
| LAN, online play and content delivery | SDL_net local web server can transfer files from a browser. ENet provides engine-specific networking such as supported HE games. Other optional paths include downloadable icon/shader packs, HTTP integrity checks and a DLC backend. | SDL_net, ENet, HTTP and DLC off. Local artwork, themes and help data ship. No LAN file server, online HE play, remote verification, or in-app pack download in this build. DLC and auto-update are separate build options; desktop availability depends on distribution. |
| OS integration and portable use | Desktop systems may offer native file dialogs, clipboard, open URL/log, window minimization, taskbar jump lists/progress, printers and Discord presence. Windows has portable configuration and the portable ZIP. | Built-in SD/USB browser and fixed fullscreen. Wii U explicitly disables minimize, clipboard, open URL and fullscreen switching. Native dialogs, printing, taskbar, Discord and update-check support are off. Portable desktop folders and shell shortcuts use different launch paths from Aroma. |
| Debugging and automation | Command-line detection, batch adding, game/engine/stat listings, saving and MD5 queries, debug channels, graphical console, scripts, recording/playback with optional ImGui, autostart and per-engine controls. | Static build and SD log for reports. Generic event recorder, ImGui, readline and text-console backend are off. Desktop CLI workflows are documented below. Exact hardware test logs identify the runtime commit. |
| Achievements, engine enhancements and original behavior | Local achievement descriptions and per-title options can cover restored content, alternate palettes, original UI/save screens, MIDI modes, automaps, quality-of-life fixes, subtitles or original timing. | `achievements.dat` is packaged. Availability depends on each engine, game data and integration. For example Kyra/EOB is built, MM1 improvements require `mm1`, and new SCUMM HE options require `he`. |
| Platform lifecycle and storage | Desktop users can switch windows, choose storage paths, configure shortcuts, and use the system file manager. | Aroma application with SDL-owned ProcUI/GX2, GamePad and TV, native keyboard and HOME suspend/resume. Configuration and default saves live under `SD:/scummvm/`. Establish lifecycle, storage, latency and memory behavior per game on the exact WUHB. |

For container/codec boundaries see the tagged [video build rules](https://github.com/scummvm/scummvm/blob/v2026.4.0/video/module.mk), [image build rules](https://github.com/scummvm/scummvm/blob/v2026.4.0/image/module.mk) and [audio build rules](https://github.com/scummvm/scummvm/blob/v2026.4.0/audio/module.mk). Wii U platform overrides are in [wiiu.cpp](wiiu.cpp).

### What the three binaries advertise

This compares the entire feature string produced by the release's `gScummVMFeatures`. I read it from Linux `--version`, the Windows PE and the Wii U version object. Many built-in decoders, MT-32, game data, per-game options and engine requirements are invisible in this string. [Tagged feature-string definition](https://github.com/scummvm/scummvm/blob/v2026.4.0/base/version.cpp)

| Feature group | Ubuntu x64 | Windows x64 | Wii U |
| --- | --- | --- | --- |
| Vorbis, FLAC, MP3, RGB, zlib, MPEG-2, FluidSynth, Theora, FreeType2, JPEG, PNG, SDL2, TinyGL | Yes | Yes | Yes |
| AAC, A/52, GIF, text-to-speech, cloud, libcurl, SDL_net, ENet, OpenGL with shaders | Yes | Yes | Off |
| ALSA, OSS sequencer (SEQ), sndio, TiMidity | Yes | — | Off |
| MikMod, VPX, FriBidi, taskbar, RetroWave | — | Yes | Off |
| TAINTED (upstream build marker for opt-in engines/subengines) | — | — | Yes, Monkey4 is opt-in at this tag |

Other upstream providers include Tremor, FluidLite, Sonivox EAS, OpenMPT, OPL2LPT and nFM. The candidate does not enable them. A different desktop OS or distribution can select a different provider for the same player-facing function.

### Resources in the two packages

These are top-level resource names in Ubuntu’s packaged share directory and the Aroma ZIP staging data directory. The shader directory is counted once. A shared name establishes packaging only. An engine can require a specific version or generated variant, as with Xeen.

| Package comparison | Resource names |
| --- | --- |
| Both | `achievements.dat`, `classicmacfonts.dat`, `darkseed.dat`, `encoding.dat`, `fonts.dat`, `grim-patch.lab`, `gui-icons.dat`, `helpdialog.zip`, `kyra.dat`, `macgui.dat`, `mm.dat`, `residualvm.zip`, `scummclassic.zip`, `scummmodern.zip`, `scummremastered.zip`, `shaders.dat`, `translations.dat`, `wintermute.zip` |
| Ubuntu only | `Roland_SC-55.sf2`, `access.dat`, `bagel.dat`, `cryomni3d.dat`, `drascula.dat`, `eem.dat`, `fonts-cjk.dat`, `fonts-imgui.dat`, `freescape.dat`, `got.aud`, `got.gfx`, `hadesch_translations.dat`, `hugo.dat`, `lure.dat`, `macs2_translation.dat`, `macventure.dat`, `mort.dat`, `myst3.dat`, `nancy.dat`, `neverhood.dat`, `pred.dic`, `prince_translation.dat`, `queen.tbl`, `shaders`, `sky.cpt`, `supernova.dat`, `teenagent.dat`, `titanic.dat`, `tony.dat`, `toon.dat`, `ultima.dat`, `ultima8.dat`, `wwwroot.zip` |
| Wii U only | `monkey4-patch.m4b` |

## Shared components used by engines

Every `add_component` entry in the tagged configure script is listed here. Cross-reference an ID with the required and optional columns above. Some components build from ScummVM’s own source. A missing component does not always need a new SDK library.

| ID | Purpose | Wii U macro | Configured |
| --- | --- | --- | --- |
| `cdtoons` | CDTOONS | `USE_CDTOONS` | off |
| `enet` | ENet | `USE_ENET` | off |
| `fmtowns_pc98_audio` | FM-TOWNS/PC98 audio | `USE_FMTOWNS_PC98_AUDIO` | on |
| `gif` | GIF | `USE_GIF` | off |
| `hmi_audio` | HMI Infinite Audio | `USE_HMI_AUDIO` | on |
| `hnm` | HNM | `USE_HNM` | off |
| `imgui` | Dear ImGui based debugger | `USE_IMGUI` | off |
| `indeo3` | Indeo 3 | `USE_INDEO3` | on |
| `indeo45` | Indeo 4&5 | `USE_INDEO45` | off |
| `jyv1` | JYV1 | `USE_JYV1` | off |
| `lua` | Lua | `USE_LUA` | off |
| `mactoolbox` | Mac Toolbox | `USE_MACTOOLBOX` | off |
| `mfc` | MFC | `USE_MFC` | off |
| `vpx` | libvpx | `USE_VPX` | off |
| `theoradec` | libtheoradec | `USE_THEORADEC` | on |
| `midi` | MIDI synthesis | `USE_MIDI` | on |
| `mpc` | MPC | `USE_MPCDEC` | off |
| `mpeg2` | mpeg2 | `USE_MPEG2` | on |
| `msmpeg4` | Microsoft MPEG-4 v3 | `USE_MSMPEG4` | off |
| `qdm2` | QDM2 | `USE_QDM2` | on |
| `sid_audio` | SID audio | `USE_SID_AUDIO` | on |
| `svq1` | Sorenson Video 1 | `USE_SVQ1` | on |
| `tinygl` | TinyGL | `USE_TINYGL` | on |
| `truemotion1` | TrueMotion 1 | `USE_TRUEMOTION1` | on |
| `universaltracker` | External Tracker Libraries | `USE_UNIVERSALTRACKER` | on |
| `unp64` | UNP64 | `USE_UNP64` | off |
| `vgmtrans_audio` | VGMTrans Soundfont audio | `USE_VGMTRANS_AUDIO` | off |
| `xan` | XAN | `USE_XAN` | off |

The `universaltracker` component builds its adapter. In this candidate the factory returns an empty stream because both external providers are off. Engine-specific module players have their own paths. [Tagged tracker implementation](https://github.com/scummvm/scummvm/blob/v2026.4.0/audio/mods/universaltracker.cpp)

### Feature requirements shared across engines

| Tagged virtual feature | Meaning | Wii U profile |
| --- | --- | --- |
| `16bit` | 16bit color | on |
| `3d` | 3D rendering | TinyGL only |
| `bink` | Bink | on |
| `cloud` | cloud | off |
| `faad` | libfaad | off |
| `flac` | FLAC | on |
| `fribidi` | FriBidi | off |
| `freetype2` | FreeType2 | on |
| `highres` | high resolution | on |
| `jpeg` | JPEG | on |
| `mad` | MAD | on |
| `opengl_game_classic` | OpenGL (classic) | off |
| `opengl_game_shaders` | OpenGL with shaders | off |
| `png` | PNG | on |
| `vorbis` | Vorbis file support | on |
| `zlib` | zlib | on |
| `test_cxx11` | Test C++11 | off |
| `printing` | Printing | off |

Of the 94 missing runtime families, **80 already have all their declared parent-level mandatory features switched on**. The other **14** require the following changes. This calculation covers the current configure profile. It excludes optional components, subengine requirements, engine resources, unimplemented renderers, memory limits and game testing.

| Required features currently off | Affected missing desktop families |
| --- | --- |
| `gif` | `phoenixvr` |
| `gif`, `opengl_game_shaders` | `hpl1` |
| `hnm` | `cryomni3d` |
| `imgui` | `macs2` |
| `indeo45` | `titanic` |
| `lua` | `hdb`, `sword25`, `tetraedge` |
| `mactoolbox` | `fool` |
| `mfc` | `bagel` |
| `opengl_game_shaders` | `twp` |
| `unp64` | `freescape`, `glk` |
| `vgmtrans_audio` | `dragons` |

### Remaining configuration switches in the Wii U header

This is the remaining generated header after the shared component rows. It includes developer and architecture switches so that a future build diff can be audited. A negative macro such as `DISABLE_NUKED_OPL` has inverted meaning. SDL2 is set separately in `config.mk`.

**Defined** — `SCUMM_BIG_ENDIAN`, `SCUMM_NEED_ALIGNMENT`, `DETECTION_STATIC`, `USE_MT32EMU`, `USE_RGB_COLOR`, `USE_HIGHRES`, `USE_SAVEGAME_TIMESTAMP`, `USE_SCALERS`, `USE_HQ_SCALERS`, `USE_EDGE_SCALERS`, `USE_ASPECT`, `USE_OGG`, `USE_VORBIS`, `USE_FLAC`, `USE_MAD`, `USE_JPEG`, `USE_PNG`, `USE_ZLIB`, `USE_FLUIDSYNTH`, `USE_FREETYPE2`, `USE_TRANSLATION`, `USE_DETECTLANG`, `USE_BINK`, `BUILTIN_RESOURCES`, `TAINTED_BUILD`.

**Undefined** — `SCUMM_LITTLE_ENDIAN`, `USE_ELF_LOADER`, `DYNAMIC_MODULES`, `DETECTION_FULL`, `DISABLE_NUKED_OPL`, `USE_TREMOR`, `ENABLE_OPL2LPT`, `USE_RETROWAVE`, `USE_NFM`, `USE_ALSA`, `USE_FAAD`, `USE_SEQ_MIDI`, `USE_SNDIO`, `USE_TIMIDITY`, `USE_A52`, `USE_LIBCURL`, `USE_BASIC_NET`, `USE_HTTP`, `USE_OPENMPT`, `USE_MIKMOD`, `USE_DLC`, `USE_SCUMMVMDLC`, `USE_FLUIDLITE`, `USE_SONIVOX`, `USE_READLINE`, `USE_TEXT_CONSOLE_FOR_DEBUGGER`, `USE_OPENGL`, `USE_GLAD`, `USE_OPENGL_GAME`, `USE_OPENGL_SHADERS`, `USE_NASM`, `USE_PANDOC`, `USE_CURL`, `USE_FRIBIDI`, `ENABLE_TEST_CPP_11`, `USE_DISCORD`, `ENABLE_VKEYBD`, `ENABLE_EVENTRECORDER`, `USE_TASKBAR`, `USE_SYSTEM_PRINTING`, `USE_SYSDIALOGS`, `USE_TTS`, `USE_UPDATES`, `USE_CLOUD`, `SCUMMVM_SSE2`, `SCUMMVM_AVX2`, `SCUMMVM_NEON`.

## Detailed settings, commands and platform contracts

The following references give the per-title options and platform contracts at the same tag. Global/game overrides, engine defaults, original editions and available input devices determine which controls appear.

- [Game-specific options, including engine enhancements](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/game.rst)
- [Launcher themes, language and downloads](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/gui.rst)
- [Scaling, shaders, fullscreen, filtering and 3D](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/graphics.rst)
- [Audio, subtitles, volume, MIDI, MT-32 and SoundFonts](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/audio.rst)
- [Controller options](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/control.rst)
- [Global and game keymaps](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/keymaps.rst)
- [Speech accessibility](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/accessibility.rst)
- [Autosave, hotspots, seed, status and updates](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/misc.rst)
- [Paths, file browser and resources](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/paths.rst)
- [Platform-specific controls](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/backend.rst)
- [Cloud storage](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/cloud.rst)
- [LAN server](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/lan.rst)
- [Library, game help and in-game menu](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/the_launcher.rst)
- [Adding, detecting and launching games](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/add_play_games.rst)
- [Preparing game files](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/game_files.rst)
- [Mac resource forks and disk images](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/mac_game_files.rst)
- [Save and load](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/save_load_games.rst)
- [Keyboard shortcuts](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/keyboard_shortcuts.rst)
- [Taskbar integration](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/taskbar_integration.rst)
- [Installing on a desktop computer](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/install_computer.rst)
- [Global versus game options](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/how_to_settings.rst)
- [Cloud setup and provider restrictions](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/connect_cloud.rst)
- [LAN transfers](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/use_scummvm/LAN.rst)
- [Autostart](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/autostart.rst)
- [Command-line interface](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/command_line.rst)
- [Configuration keys and domains](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/configuration_file.rst)
- [Music devices and formats](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/understand_audio.rst)
- [Graphics modes](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/understand_graphics.rst)
- [Search syntax](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/advanced_topics/understand_search_box.rst)

### Documented game-specific settings

The tagged manual groups the following controls by engine. Games in the same family can expose different subsets. The live dialog also consults the engine and detected edition. Use the engine ID tables above for Wii U build coverage. [Tagged game settings and keys](https://github.com/scummvm/scummvm/blob/v2026.4.0/doc/docportal/settings/game.rst)

| Engine section | Controls documented for its games |
| --- | --- |
| ADL | TV emulation, Color Graphics, Show scanlines, Always use sharp monochrome text |
| AGI | Use original save/load screens, Use an alternative palette, Mouse support, Use Hercules hires font, Pause when entering commands, Add speed menu |
| AGOS | AdLib OPL3 Mode, Use DOS version music tempos, Use Windows version music tempos, Prefer digital sound effects, Disable fade-out effects |
| Alcachofa | High Quality, 32-Bit Mode |
| ALG | Use lower quality single speed CD-ROM video |
| Bladerunner | Sitcom mode, Shorty mode, Frame limiter high performance mode, Max frames per second limit, Disable McCoy's quick stamina drain, Show subtitles during text crawl, Fix credits for voice actors |
| Buried | Skip support |
| CGE | Color Blind Mode, Enable Text to Speech |
| CGE2 | Color Blind Mode, Enable Text to Speech for Objects and Options, Enable Text to Speech for Subtitles |
| Chewy | Use original save/load screen |
| Cine | Use original save/load screen, Use transparent dialog boxes in 16 color scenes |
| Drascula | Use original save/load screens |
| Dreamweb | Use original save/load screens, Use bright palette mode, Enable Text to Speech for Objects, Options, and the Bible Quote, Enable Text to Speech for Subtitles |
| Freescape | Prerecorded sounds, Extended timer, Automatic drilling, Disable demo mode, Disable sensors, Disable falling |
| GLK | Font, Color, Border, Window margin, Text margin, Window padding, More align, More font, Link style, Caret shape, More prompt, Leading, Baseline, Monosize, Propsize, Caps, Typographic quotes, Dashes, Spaces, Style hints, Safe clicks, Column count, Lock columns, Row count, Lock rows, Justify, Graphics |
| Griffon | Enable Text to Speech |
| GrimE | Load user patch (unsupported), Show FPS |
| Groovie | Fast movie speed, Use original save/load screens, Easier AI, Updated Credits Music, Slim Left/Right Hotspots, Speedrun Mode |
| HDB | Enable cheat mode |
| Hopkins | Gore Mode |
| Hypno | Enable original cheats, Enable infinite health cheat, Enable infinite ammo cheat, Unlock all levels, Enable restored content |
| Kyra | Enable studio audience, Skip support, Enable helium mode, Smooth scrolling, Enable floating cursors, Suggest save names, HP bar graphs, Fight Button L/R Swap |
| Lure | TTS Narrator |
| Macs2 | Use original save/load screens, Enable Text to Speech, Audio-visual improvements, Modern UI/UX adjustments |
| MADE | Play a digital soundtrack during the opening movie |
| MADS | Easy mouse interface, Animated inventory items, Animated game interface, Naughty game mode, TTS Narrator |
| Mohawk | Zip Mode activated, Transitions enabled, Play the Myst fly by movie, Improve Selenitic Age puzzle accessibility, Simulate loading times of old CD drives, Water Effect Enabled, Transitions (Riven only) |
| mTropolis | 16:9 widescreen mod, Improved music mixing, Autosave at progress points, Enable short transitions, Enable subtitles for important sound effects, Start with debugger |
| Myst3 | Widescreen mod |
| Nancy | Player Speech, Character Speech |
| Neverhood | Use original save/load screens, Skip the Hall of Records storyboard scenes, Scale the making of videos to full screen, Repeat useful Willie's hint |
| Queen | Alternate intro |
| SCI | Skip EGA dithering pass (full color backgrounds), Enable high resolution graphics, Enable black-lined video, Use high-quality video scaling, Use high-quality "LarryScale" cel scaling, Prefer digital sound effects, Use original save/load screens, Use CD audio, Use Windows cursors, Use silver cursors, Upscale videos, Enable content censoring, Use RGB rendering, Use per-resource modified palettes, Enable bearded musicians, MIDI Mode |
| SCUMM | Show Object Line, Use NES Classic Palette, Trim FM-TOWNS games to 200 pixels height, Play simplified music, Enable smooth scrolling, Allow semi-smooth scrolling, Enable game-specific enhancements, Load modded audio, Enable the original GUI and Menu |
| Sherlock | Use original load/save screens, Pixellated scene transitions, Don't show hotspots when moving mouse, Show character portraits, Slide dialogs into view, Transparent windows, TTS Narrator |
| Sky | Floppy intro |
| Stark | Load modded assets, Enable linear filtering of the backgrounds images, Enable font anti-aliasing |
| Supernova | Improved mode, Enable Text to Speech |
| Sword2 | Show object labels |
| Sword25 | Use English speech |
| Thimbleweed Park | Toilet paper over, Annoying in-jokes, Invert verb colors, Retro fonts, Retro verbs, Classic Sentence, Ransome *unbeeped* (DLC) |
| Toltecs | Use original save/load screens |
| Tot | Use original save/load screens, Disable transitions, AdLib OPL3 mode, Copy protection |
| Trecision | Use original save/load screens |
| TwineE | Enable wall collisions, Enable debug mode, Enable audio CD, Enable sound, Enable voices, Enable text, Enable movies, Enable mouse, Use the USA version, Enable high resolution, TTS Narrator |
| Ultima | Use original save/load screens, Enable frame skipping, Enable frame limiting, Enable cheats, Enable high resolution, Play foot step sounds, Enable jump to mouse position, Enable font replacement, Enable font anti-aliasing, Camera moves with Silencer, Always enable Christmas easter-egg |
| Wintermute | Show FPS-counter, Sprite bilinear filtering (SLOW), Force to use 2D renderer (2D games only) |
| V-Cruise | Start with debugger, Faster animations |
| Xeen | Show item costs in standard inventory mode, More durable armor |
| ZVision | Use original save/load screens, Double FPS, Enable Venus, Disable animation while turning, Use high resolution MPEG video |

### PC command inventory

These are all long options printed by the reference Ubuntu binary’s `--help`. They include engine-specific switches and diagnostics. The CLI accepts a target argument and describes inverted boolean forms with `no-`. The Wii U package normally starts from the Aroma menu. The presence of common CLI code does not verify argument delivery from a Wii U loader.

- `--version`, `--help`, `--list-games`, `--list-games-json`, `--list-all-games`, `--list-targets`, `--list-targets-json`, `--list-engines`
- `--list-engines-json`, `--list-all-engines`, `--dump-all-detection-entries`, `--stats`, `--list-debugflags`, `--list-all-debugflags`, `--list-saves`, `--add`
- `--detect`, `--game`, `--engine`, `--auto-detect`, `--recursive`, `--no-exit`, `--config`, `--initial-cfg`
- `--logfile`, `--screenshotpath`, `--path`, `--save-slot`, `--fullscreen`, `--no-fullscreen`, `--gfx-mode`, `--shader`
- `--stretch-mode`, `--scaler`, `--scale-factor`, `--filtering`, `--no-filtering`, `--window-size`, `--gui-theme`, `--themepath`
- `--list-themes`, `--music-driver`, `--list-audio-devices`, `--language`, `--platform`, `--music-volume`, `--sfx-volume`, `--speech-volume`
- `--midi-gain`, `--subtitles`, `--boot-param`, `--debuglevel`, `--debugflags`, `--debug-channels-only`, `--dump-scripts`, `--cdrom`
- `--joystick`, `--savepath`, `--extrapath`, `--iconspath`, `--soundfont`, `--multi-midi`, `--native-mt32`, `--dump-midi`
- `--enable-gs`, `--output-channels`, `--output-rate`, `--opl-driver`, `--show-fps`, `--no-show-fps`, `--random-seed`, `--renderer`
- `--antialiasing`, `--aspect-ratio`, `--[no-]dirtyrects`, `--render-mode`, `--alt-intro`, `--copy-protection`, `--talkspeed`, `--demo-mode`
- `--start-movie`, `--tempo`, `--host-game`, `--join-game`, `--engine-speed`, `--md5`, `--md5mac`, `--md5-path`
- `--md5-length`, `--md5-engine`

### Capabilities requested by individual games and backends

**Feature** — `kFeatureFullscreenMode`, `kFeatureAspectRatioCorrection`, `kFeatureFilteringMode`, `kFeatureStretchMode`, `kFeatureVirtualKeyboard`, `kFeatureCursorPalette`, `kFeatureCursorAlpha`, `kFeatureCursorMask`, `kFeatureCursorMaskInvert`, `kFeatureCursorMaskPaletteXorColorXnor`, `kFeatureOverlaySupportsAlpha`, `kFeatureIconifyWindow`, `kFeatureOpenGLForGame`, `kFeatureShadersForGame`, `kFeatureVSync`, `kFeatureDisplayLogFile`, `kFeatureClipboardSupport`, `kFeatureOpenUrl`, `kFeatureTouchpadMode`, `kFeatureKbdMouseSpeed`, `kFeatureJoystickDeadzone`, `kFeatureScalers`, `kFeatureShaders`, `kFeatureDLC`, `kFeatureSystemBrowserDialog`, `kFeatureNoQuit`, `kFeatureTouchscreen`, `kFeatureCpuNEON`, `kFeatureCpuSSE2`, `kFeatureCpuSSE41`, `kFeatureCpuAVX2`, `kFeatureCpuAltivec`, `kFeatureRotationMode`, `kFeatureStaleMousePositionWorkaround`. [Tagged interface](https://github.com/scummvm/scummvm/blob/v2026.4.0/common/system.h)

**EngineFeature** — `kSupportsSubtitleOptions`, `kSupportsReturnToLauncher`, `kSupportsLoadingDuringRuntime`, `kSupportsSavingDuringRuntime`, `kSupportsChangingOptionsDuringRuntime`, `kSupportsArbitraryResolutions`, `kSupportsHelp`, `kSupportsQuitDialogOverride`. [Tagged interface](https://github.com/scummvm/scummvm/blob/v2026.4.0/engines/engine.h)

**MetaEngineFeature** — `kSupportsListSaves`, `kSupportsLoadingDuringStartup`, `kSupportsDeleteSave`, `kSavesSupportMetaInfo`, `kSavesSupportThumbnail`, `kSavesSupportCreationDate`, `kSavesSupportPlayTime`, `kSimpleSavesNames`, `kSavesUseExtendedFormat`. [Tagged interface](https://github.com/scummvm/scummvm/blob/v2026.4.0/engines/metaengine.h)

## Porting priorities

| Work item | What it unlocks | Evidence needed to close it |
| --- | --- | --- |
| Complete desktop-default branches inside the 19 linked families | AGOS2, Lands of Lore, Might and Magic 1, HE71+, FoxTail, HeroCraft and Wintermute 3D. Monkey4 goes beyond the upstream default and already has a Wii U switch. | Check each branch and its actual files. Include a new game, intro/audio, save/load and return to the launcher. Track HE online play separately from local HE games. |
| First missing families to investigate with existing core features | AGI, Sky, Queen and Lure declare no mandatory shared features in their manifests. Examples with existing required features include Director and Myst III. Director editions vary by projector and media. Myst III declares a 3D requirement and can select TinyGL. | Stage each engine’s required data, test detection and play on PPC, record startup, cutscene playback, input and save/load. Measure memory and speed before grouping 3D games into a release. |
| Restore reusable media and language paths | GIF for Phoenix VR, HNM for Cryo Omni3D, Indeo 4/5 for Titanic, Lua for Sword 2.5, UNP64 for Glk, external tracker playback for applicable AGS modules, AAC/AC-3, VPX and FriBidi for affected assets/languages. | Use identified game assets or a reference stream, include seek/endian tests and confirm playback or text order on hardware. The 28 shared component flags distinguish code already in the tag from missing libraries. |
| Provide the renderer required by the game | Alpha Polaris needs Wintermute 3D. The tag’s Wintermute implementation explicitly leaves the TinyGL 3D constructor inactive. HPL1 and Thimbleweed Park require OpenGL shaders by manifest. | Establish a working rendering path, measure scene speed and memory, compare geometry, textures, blending and text. Preserve SDL’s ownership of GX2 and ProcUI. [Tagged Wintermute renderer selection](https://github.com/scummvm/scummvm/blob/v2026.4.0/engines/wintermute/base/base_game.cpp) |
| Restore desktop services and accessibility | TTS, bidirectional text, cloud save sync, LAN transfer and downloadable artwork affect many games or the launcher. OS-specific APIs can have console equivalents. | Test a named language/voice, save round-trip and offline behavior, transfer a known file, verify the resulting UI and user data after relaunch. |
| Prove compatibility and deployment size | Desktop engine count is a build target. Wii U playability also depends on the game edition, data archives, memory, frame pacing, audio and lifecycle. | For each candidate record the runtime revision, ZIP hash, game ID, platform/language/edition, controller, SD/USB, scenes, save/reload, HOME/reentry, observed result and log. Use distinct entries for a cross-build, host/PPC test and console report. |

## How to refresh this inventory

1. Pin the upstream release tag and the exact Wii U runtime revision and package SHA-256. Use the packaged build’s generated `config.mk`, `config.h`, `engines/plugins_table.h`, stage and `BUILD-INFO.txt`. The current checkout can contain later documentation changes.
2. Download the matching desktop packages, check the official hashes, and extract to a temporary directory. On a matching Linux desktop run the packaged executable with a temporary config and `--version`, `--list-engines`, `--list-all-engines`, `--list-games`, `--list-audio-devices` and `--help`. Use `--list-engines` for runtime families. `--stats` also counts detection and reports 126 engines here. Linux device enumeration depends on the host.
3. On Windows prefer running those same commands. The static check used here takes each `REGISTER_PLUGIN_STATIC(..., PLUGIN_TYPE_ENGINE, Class)` at the tag and looks up the exact NUL-delimited GCC/Itanium RTTI name in the official x64 PE. For a global class use the decimal name length followed by its name. A namespaced type uses `N`, each length/name pair and a final `E`. All 126 families had a registration to check and the 113 matched runtime class names equal the Linux runtime set. Detection classes have different names. Revisit this method for a different compiler or packaging format.
4. From the checkout run the commands below. Read each `add_engine` declaration as ID, name, default, children, base-game description, required features and optional components. Build the child-to-parent map from the fourth argument before counting families. Compare every ID with its `ENABLE_<ID>` assignment. Check `add_feature` and `add_component` plus the module rules and package contents for implied dependencies. Prefer new binary evidence over projecting a default onto a delivered build.

   ```sh
   git rev-parse 'v2026.4.0^{commit}'
   git grep -h '^add_engine ' v2026.4.0 -- 'engines/*/configure.engine'
   git grep -h -E '^add_(feature|component) ' v2026.4.0 -- configure
   ```

5. Update the tables, evidence date and outstanding hardware results together. If moving to a different upstream release, keep this baseline available for comparisons. The public [compatibility chart](https://www.scummvm.org/compatibility/2026.4.0/), [tagged manual](https://github.com/scummvm/scummvm/tree/v2026.4.0/doc/docportal) and [release notes](https://github.com/scummvm/scummvm/blob/v2026.4.0/NEWS.md) provide the edition-level details behind the inventory.
