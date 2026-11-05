# Wii U port — start here in a new chat

Last verified locally: **2026-11-05**. This is a compact handoff, not the full
conversation or proof that every earlier test still applies. Recheck Git/files
before acting; the current user request takes precedence over old next steps.

## Workspace and reading map

The local workspace is now `/home/renan/Documentos/wiiu-dev/scummvm`, with the Git
checkout at `work/scummvm`. The former path below `VisualStudio` no longer
exists. Paths in historical build logs and publication guides may still refer
to that location; do not blindly execute them or rewrite preserved evidence.

Paths below are relative to the **outer workspace** unless marked checkout:

| Need | Read |
| --- | --- |
| Durable architecture/work rules | `AGENTS.md` and checkout `AGENTS.md` |
| Current user features/controls/audio | checkout `README.md` |
| Current development / joint engine tests | checkout `backends/platform/sdl/wiiu/ENGINES-TEST-0.8.0.md` |
| Current 0.7.1 release/base update | checkout `backends/platform/sdl/wiiu/RELEASE-0.7.1.md` |
| Build/SDK and SDL patches (7 release / 9 candidate) | checkout `backends/platform/sdl/wiiu/README.WIIU` |
| Full GamePad / Remote + Nunchuk controls | checkout `README.md`, Controls |
| Current prepared release changes | checkout `backends/platform/sdl/wiiu/RELEASE-0.7.1.md` |
| Binary provenance/packages/checksums | `work/releases/0.7.1/BUILD-INFO.txt`, `SHA256SUMS.txt`, `PUBLISH.md` |
| Detailed history/evidence | `docs/WIIU_PORT_PLAN.md`, relevant complete sections |
| Older build packages after cleanup | `work/archives/builds-pruned-20261026/` |
| Native input and regression tests | checkout `backends/platform/sdl/wiiu/tests/` |
| SDK source/install and archived logs | shared `../sdk/sources/`, `../sdk/current/`, `../sdk/backups/`; project `work/logs/`; old SDK/source paths are compatibility links |

Plan sections: 26–27 renderer freeze; 28 Xeen/data/compatibility; 29–32 Curse
audio and QFG4 loading; 33 text editing/Vorbis; 34 controller keyboard;
35–36 Groovie FMV and 0.7.0; 38 original Wii Remote IR candidate; 42 button 2/C
swap and hardware feedback; 43 upstream 2026.4.0; 44 release 0.7.1;
45 storage cleanup and archival; 46 original combined 0.8.0 engine candidate;
47 SAGA, Kyra3 IO and FAT32 USB candidate; 48 IHNM opening skip fix;
49 IHNM success and pixel-precision IR candidate; 50 adaptive IR stabilization.
Read the whole plan for an architectural audit or
when explicitly requested, not by default for every small follow-up.
`WIIU_IMPLEMENTATION_STATUS.md` describes the historical first integration,
not the current release. Missing outer files in a standalone clone mean local
evidence is unavailable; they do not imply the source port is absent.

## Current development: adaptive IR stabilization — 2026-11-05

- Runtime **6666255db53d008d678903244955afe4771b86f2**, upstream 2026.4.0,
  `0.8.0 candidate 20261105 (Remote pointer stabilization)`. All combined
  engines/codecs, confirmed SAGA/Kyra fixes and pending FAT USB retained.
  **The exact new binary has not been run on Wii U/Cemu.**
- I reported smoother motion but excessive stationary jitter compared with
  the Wii U Menu after the last delivered October 30 candidate (**51ae1b0d**
  by conversation context; no new log/build ID supplied). This is a hardware
  scenario report, not full regression coverage. No new raw IR trace, display
  mode or Remote model was supplied. Evidence/baseline:
  `work/logs/remote-stability-20261105-baseline/`.
- Patch 9 `sdl2-wiimote-stability.patch` retains 1–8 and adds a speed-adaptive
  low-pass filter before integer conversion. It independently implements the
  1 Euro principle (Casiez/Roussel/Vogel, CHI 2012), not Nintendo's menu code.
  0.8 Hz minimum / beta 0.03 / derivative 1 Hz, in 1280x720 reference units.
  No spatial dead zone; stronger stabilization near rest, more response during
  motion. Smoothing introduces some tracking delay; buttons remain immediate
  at the filtered cursor. Per-channel elapsed-time state resets on invalid IR,
  ownership loss, resize, suspend/disconnect or gaps >250 ms. Same-tick polls
  do not advance. Touch/stick arbitration and existing releases remain.
- Actual helper/adapter ASan/UBSan: raw predecessor fails stationary-noise
  regression (RMS ratio 1.018, x span 8 pixels). New synthetic noise at
  8/16/33 ms and 854/1280/1920 widths gives RMS .245–.318 and x span 1–2 pixels.
  24 timed slow ramps preserve one-pixel steps, <=3.13-pixel lag at 15.625 px/s,
  and final convergence. Synthetic 300 px/s fast ramp delay stays <32 ms after
  startup; reversal/clicks/cycle-of-life/native keyboard pass. These are mocks,
  not physical tracking/noise, end-to-end latency or console FPS measurements.
- Pin `../sdk/versions/r51-wut1.9.1-sdl2.32.7-stable-pointer-20261105` before
  sourcing env.sh. Based on preserved October 30 profile; only SDL rebuilt.
  New libSDL2.a SHA256
  `c7dd4529c74845db23d833b2166d6cece6bcf83f796323ba00ffd9f85e1fe552`.
  Compiler/WUT/codecs/USB unchanged; `current` unchanged. Nine patches plus
  KPADError reproduce every modified SDL file. Doctor passes 2611 files/6 links;
  old profile/default manifests and October 30 package checksums still pass.
  Reproduction: POINTER-SDK.md/build-pointer-sdk.sh; shared evidence:
  `../sdk/validation/20261105-pointer-stability/REPORT.md`.
- Fresh full cross-build/package checks pass: engine/codec feature equality,
  SAGA/Kyra/USB/adapter source equality, imports/no strong undefineds, RPX/WUHB
  equality, unchanged name/icon/data, nine patches/licenses and ZIP integrity.
  Artifacts: `work/build-scummvm-wiiu-0.8.0-test-20261105/`. Aroma ZIP
  `ScummVM-wiiu-0.8.0-candidate-20261105-aroma.zip`, SHA256
  `28195d38ffe50954ae4e1f60b4f38d30fc8bd24df4b022f1e5a6f11eeb582469`.
  RPX ZIP/ELF/RPX/WUHB, BUILD-INFO/SHA256SUMS and test guides also present.
  Verifier: `work/package-0.8.0-test-20261105.py FULL_RUNTIME_COMMIT`.
- Next hardware comparison: stationary jitter/small launcher targets, slow
  diagonals/A drag, quick motion/reversal and delay feel in launcher + a game,
  GamePad/Nunchuk handover, keyboard, HOME/disconnect/Quit, then copied log.
  Physical USB/broader engines still pending. No push/tag/publication. Older
  binaries, unrelated dirty docs and Android caches are preserved. Detailed
  evidence: plan section 50, `work/logs/remote-stability-*20261105*`.

## Predecessor: Wii Remote pointer precision — 2026-10-30

- Runtime **51ae1b0d6f5d8c833b7edad48aed0b90c62511da**, upstream 2026.4.0,
  `0.8.0 candidate 20261030 (Remote pointer precision)`. Complete combined
  engine/codec profile, Kyra IO, SAGA input fix and pending FAT USB retained.
  My November 5 follow-up reports smoother motion but excessive stationary
  jitter, referring to the last delivered candidate; no fresh log was supplied.
- I reported **IHNM worked perfectly** with **8d242742**. New evidence is the
  October 29 20:02–20:04 session, archived unchanged in
  `work/logs/remote-precision-20261030-baseline/scummvm.log`, 6408 bytes/97 lines,
  SHA256 `31cf7794273a5cd09fc26e0cf60e92b0df14c54198603f313cddbd42ba8e5925`.
  It records an accepted opening skip, interactive character/chapter clicks
  and clean Quit. No raw IR sample trace or physical USB test is present.
- My next report describes grid-like slow Remote motion. The actual SDL helper
  had a width/320 deadband for its active owner: 4 display pixels at 720p,
  6 at 1080p, including drags. Eighth incremental patch
  `sdl2-wiimote-precision.patch` reports every changed pixel for the owner.
  Touch/stick acquisition hysteresis and the 350 ms yield remain. Identical
  positions are silent; no extra averaging delay, reads or engine timing change.
- A final slow-ramp host regression fails against the old helper with 16 moves
  in 4-pixel steps over 64 pixels at 720p. Updated actual-code ASan/UBSan mocks
  pass 24 horizontal/vertical/diagonal ramp cases, both directions, 320/854/
  1280/1920 widths, hover/held drag, same-pixel noise, ownership, clicks, invalid
  tracking, multiple Remotes, disconnect/suspend and tick wrap. Native keyboard
  tests pass. This does not measure physical IR noise or console smoothness.
- Pin **`../sdk/versions/r51-wut1.9.1-sdl2.32.7-pointer-20261030`** before
  sourcing shared env.sh. Based on the preserved USB profile; only SDL rebuilt,
  with eight patches + the existing KPADError fix. Toolchain/WUT/codecs/USB
  libraries are byte-equal to the base. New SDL SHA256
  `379f97bb867c2d1da9903a98735ea9433d6f4ba7dbaf916b53d498efbf43555d`.
  `current` is unchanged. Reproduction: POINTER-SDK.md / build-pointer-sdk.sh;
  shared SDK evidence `../sdk/validation/20261030-pointer/REPORT.md`.
- Fresh full cross-build, linked engines/codecs, no strong undefineds, imports,
  embedded RPX, unchanged icon/name/data, eight patches/licenses and ZIP checks
  pass. SAGA source equals confirmed 8d242742; Kyra/USB/adapter unchanged.
  SDK doctor passes; prior default/USB manifests and October 29 package hashes
  still pass. Unrelated documentation diff and Android caches are preserved.
- Artifacts: `work/build-scummvm-wiiu-0.8.0-test-20261030/`. Aroma ZIP
  `ScummVM-wiiu-0.8.0-candidate-20261030-aroma.zip`, SHA256
  `8ddfb9bd05e49983c0d4bfdc9f36e5ee0a8d664fe0b66cddde53f965987cf2ee`.
  RPX ZIP, ELF/RPX/WUHB, BUILD-INFO/SHA256SUMS and test guides also present.
  Verifier: `work/package-0.8.0-test-20261030.py FULL_RUNTIME_COMMIT`.
  Evidence: plan section 49 and `work/logs/remote-precision-*20261030*`.
- Next: compare slow IR motion/small targets and A drag in launcher + a game,
  stationary jitter, GamePad touch/sticks and Nunchuk handover, keyboard,
  HOME/disconnect/Quit; copy log. Physical USB and broader 0.8.0 game tests
  remain pending. No push/tag/publication or old package replacement.

## Predecessor: IHNM opening skip fix — 2026-10-29

- Runtime **8d242742c53e964ba4378c1dc2bf09a243053861**, upstream 2026.4.0,
  `0.8.0 candidate 20261029 (SAGA intro input)`, revision `-wiiu-8d242742`.
  All engines/codecs/data, the confirmed Kyra IO change and pending FAT USB
  implementation retained. I subsequently reported IHNM working perfectly;
  the October 29 log is archived in the current development section above.
- I reported **Kyrandia 3 worked perfectly** with predecessor **7cd5beaa**.
  IHNM DOS/English started, but I initially described no controls/cursor;
  I clarified this was **during the opening, with Y unable to skip**. This is
  not evidence of missing mouse controls in character selection/gameplay.
  **USB explicitly not tested.** New log is the October 28 21:11–21:17 session,
  archived unchanged at `work/logs/saga-input-20261029-baseline/`, SHA256
  `a3cae78031eda5419722134aed6069d834f222c27626a338c11215132652ba01`.
  It shows ongoing IHNM cutaways, keyboard cancellation, launcher return,
  Kyra3 and clean Quit. Large AUD initializations: 487/499 ms; total scene
  entries include movement/animations and are not frozen-IO measurements.
- SAGA's game shortcut map had no JOY_Y Escape and was disabled for video/
  cutaway/null panels. The separate IHNM intro poll ignored keymapped actions.
  Fixed panel/text map selection before polling, including first-press input;
  initialized panel state and removed conflicting unconditional map re-enables.
  Original map/action IDs and keyboard bindings retained. Y/Remote 2 skips;
  X/Nunchuk Z skips speech; L/Remote 1 opens original options where allowed.
  Logo/title also accepts Wii U A/B/touch/IR clicks. Cursor remains intentionally
  hidden during videos; existing show/restore behavior is preserved.
- Actual SAGA factory/panel routing/intro and normal input consumers pass host
  ASan/UBSan with mocked keymapper/event transport in portable, Wii U and
  diagnostic builds: first skip, custom actions, mouse press/release, options,
  exclusive panel maps, save/status text and global map state. This is not
  hardware or game progression validation. Bounded WIIU_SAGA_INPUT_DIAGNOSTICS
  records panel/cursor requests and accepted skip/click/action events; existing
  Kyra diagnostic limits retained, other detailed traces off.
- Fresh full build: `work/build-scummvm-wiiu-0.8.0-test-20261029/`. Aroma ZIP
  `ScummVM-wiiu-0.8.0-candidate-20261029-aroma.zip`, SHA256
  `a6e53a3f7b4048098326bbba563dd5747328beca69203f1ef5341ed850c0e0a4`.
  RPX ZIP/ELF/WUHB, BUILD-INFO/SHA256SUMS and updated guides also present.
  Verifier: `work/package-0.8.0-test-20261029.py FULL_RUNTIME_COMMIT`.
  All required engines/symbols, codec equality, no strong undefineds, unchanged
  imports/icon/name, embedded RPX, data/ZIP/licenses and pinned SDK doctor pass.
- Pin the same **USB SDK profile dated 20261028** as below; no SDK/profile/
  default changes. Kyra, USB and input backend source are byte-equal to
  7cd5beaa; old packages and unrelated doc diff/Android caches are preserved.
  No push/tag/publication. Evidence: plan section 48 and `work/logs/*20261029*`.
  My subsequent report confirms IHNM in the tested scenario. USB remains
  pending separately; 0.8.0 final still depends on the broader game tests.

## Predecessor: SAGA / Kyra IO / FAT USB candidate — 2026-10-28

- Runtime/source **7cd5beaac29de66257719985dd1fbdb7f2718053**, based on upstream
  2026.4.0. Identity: `0.8.0 candidate 20261028 (SAGA / Kyra IO / FAT USB)`,
  revision `-wiiu-7cd5beaa`. Final 0.8.0 is still pending hardware results.
- My supplied Oct 27 log identifies predecessor **67e2ae12**. I reported Simon
  1 (CD/Windows/English), Sword1 (GOG/Windows/English) and Gob3 (CD v1.02/DOS/
  English) working perfectly. Kyra3 DOS/English is playable but I reported
  approximately 4-second scene stalls. The log has no per-scene timings and
  does not establish a cause. Archived unchanged at
  `work/logs/io-saga-usb-20261028-baseline/scummvm-67e2ae12-20261027.log`, SHA256
  `6fa0b7bdf40fcf8bd590e69fad7f251e1b2e27055aa8a5e80edb15a5b7407600`.
- Added static **SAGA + IHNM** for I Have No Mouth, and I Must Scream. All
  previous engines, codecs and data retained; no additional engine-data archive.
  The guide now lists 23 game IDs. No new game's Wii U compatibility is implied.
- Wii U AUD input now has a bounded 64 KiB Common buffer per sound, targeting
  thousands of per-chunk seeks in the duration scan on music changes. Actual
  decoder/buffer host tests preserve samples, duration, seek/rewind and ownership.
  2,000 synthetic 2,048-byte chunks: 4,004 reads/2,001 seeks -> 63 reads/63 seeks.
  This is not console timing or proof of fixing Kyra3. `WIIU_KYRA_IO_DIAGNOSTICS`
  records up to 24 AUD initializations and 24 Kyra3 transitions; other tracing off.
- **FAT32 USB hardware candidate:** libfat + Mocha raw USB, one PC-readable
  drive connected before launch, 512-byte logical sectors and MBR (or FAT
  volume without table). `usb:/` is listed only after mounting. Games and an
  explicitly selected save path use existing ScummVM FS/save abstractions;
  SD config/log/data/default saves stay unchanged. No migration or formatting.
  NTFS/exFAT/GPT/Wii U encrypted WFS/multiple drives/hot removal are excluded.
  Mount owned by a static main object, flushed after backend destruction,
  including SDL quit/fatalError `exit()` paths. No SD remount or new ProcUI loop.
- Pin SDK **`../sdk/versions/r51-wut1.9.1-sdl2.32.7-usb-20261028`** explicitly
  before sourcing env.sh. **Shared current remains the earlier r51 profile**;
  it lacks these USB dependencies. Compiler/WUT/SDL/codecs unchanged; new local
  libfat/libmocha patches fix Mocha/newlib types, partition locking, aligned/
  partial cache allocation and mirrored FAT writes. Sources/patches/licenses
  preserved; README.WIIU + USB-SDK.md + build-usb-sdk.sh explain reproduction.
- Full cross-build, final revision relink, linked SAGA/IHNM + previous engines,
  required flags, no strong undefineds, unchanged import modules, embedded RPX,
  unchanged name/icon, ZIP contents/licenses/data and SDK doctors pass. Host
  ASan/UBSan owner tests include init/mount failure, collision, repeated mount
  and exit() order. Actual libfat MBR/VBR image tests pass concurrent game reads
  plus save overwrite/fsync/close, external mtools bytes and clean fsck.fat.
  At packaging this binary had no hardware test. My subsequent 7cd5beaa
  Kyra3 success/IHNM opening failure and untested USB are recorded above.
- Artifacts: `work/build-scummvm-wiiu-0.8.0-test-20261028/`:
  `ScummVM-wiiu-0.8.0-candidate-20261028-aroma.zip`, SHA256
  `b9de11fd397ba00f431c8736d1d4f16c8290e357b58922635235dd852fc1231e`;
  separate experimental RPX ZIP, BUILD-INFO/SHA256SUMS, guides and ELF/RPX/WUHB.
  Verifier: `work/package-0.8.0-test-20261028.py FULL_RUNTIME_COMMIT`.
  Main guide: checkout `backends/platform/sdl/wiiu/ENGINES-TEST-0.8.0.md`.
- Next hardware checks: same Kyra scenes on SD + copied timing log; IHNM;
  FAT32 USB Add Game, save/load/overwrite, Quit/relaunch/reload, HOME/return,
  SD-only startup and a previously working SD game. Keep the drive connected
  until exit; cancel the Wii U Menu's format prompt for a FAT32 test drive.
  Record model/power/layout and SD vs USB game/save paths. Older three build/
  release groups and pre-existing doc diff/Android caches preserved. No push,
  tag or publication. Detail: plan section 47; `work/logs/*20261028*`; shared
  SDK report `../sdk/validation/20261028-usb/REPORT.md`.

## Predecessor: combined 0.8.0 engine candidate — 2026-10-26

- I requested all engines in one build for joint hardware tests. Source/binary
  **67e2ae127bdf0101d25290e50908e3af97c398fd**, upstream 2026.4.0. Identity:
  `0.8.0 candidate 20261026 (expanded engines)`, revision `-wiiu-67e2ae12`.
- Added static Tinsel, Sword1, Sword2, AGOS, Gob, Kyra/EOB, Darkseed and
  Sherlock. All previous engines/codecs/features remain. Rebel Assault I/II
  already use SCUMM v7/v8; Sherlock covers Scalpel and Rose Tattoo.
  EOB I/II enabled; **EOB III is not implemented by this base** and excluded.
  AGOS2/LoL/Rebel2 PSX/HE/MM1/TinyGL remain off; original PC editions targeted.
- Fresh candidate build with unchanged pinned r51 SDK/seven SDL patches. SDK
  doctor, full cross-build, all linked engine/subengine symbols, required flags,
  no strong undefineds, RPX imports, embedded RPX equality, name/icon and ZIP
  data/licenses pass. No prior codec/component was disabled. Backend input,
  keyboard, rendering and audio code are unchanged from 0.7.1.
- Added bundled kyra.dat (INDEX 123/MD5 checked) and darkseed.dat (ZIP checked).
  All other engine/theme/translation bytes equal 0.7.1; generated Xeen 1.3
  remains in both stages. Sword SMK/DXA works at decoder/build level; old MP2
  cutscenes need the still-disabled MPEG-2 codec. Runtime testing is pending.
- Artifacts: `work/build-scummvm-wiiu-0.8.0-test-20261026/`:
  `ScummVM-wiiu-0.8.0-candidate-20261026-aroma.zip`, SHA256
  `76e2c6cc3a409756611e7fd6ae631f842ddb2468f6ff273c010bc58e6b1b2337`;
  separate RPX ZIP, ELF/RPX/WUHB, BUILD-INFO, SHA256SUMS and test template.
  Verifier: `work/package-0.8.0-test-20261026.py FULL_RUNTIME_COMMIT`.
- Guide: checkout `backends/platform/sdl/wiiu/ENGINES-TEST-0.8.0.md`; 22 game
  IDs/checklist and `TEST-RESULTS-0.8.0.md`. Bundled root TESTING.md and
  TEST-RESULTS.md provide the same information. At packaging time all 22 games awaited tests. My later October 27 report
  and log are recorded in the current section above; the original ZIP is intact.
  Non-Aroma RPX remains unverified. 0.8.0 is not a finished release yet.
- Existing 0.7.1 and tested 2026.4.0 packages/builds are intact; a new third
  build directory exists for this candidate. No SDK changes, push, tag or
  publication. Pre-existing r51 docs and Android caches preserved. Detailed
  evidence: plan section 46 and `work/logs/engines-0.8.0-20261026-*`.

## Storage cleanup — 2026-10-26

- I authorized deleting obsolete builds to save disk space. At that cleanup,
  two full compilation trees were retained: `work/build-scummvm-wiiu-0.7.1/` and the tested
  `work/build-scummvm-wiiu-2026.4.0/`. All `work/releases/` files are unchanged.
- 16 older build trees were removed after their 31 original ZIPs, RPX/WUHB,
  config/checksum/build records and otherwise-unique staging files were
  archived and verified. Archive: `work/archives/builds-pruned-20261026/`,
  with the former build names as children. Old debug ELF/object/library/
  dependency outputs were discarded. Net recovery: approximately **12.19 GiB**
  after the 848 MiB archive; workspace work/ is now about 4.2 GiB.
- SHA256 verification covers all 5002 files in the two retained trees and
  releases. 0.7.1, retained 2026.4.0 and archived Remote SHA256SUMS pass.
  Source/Git, SDK/default, existing logs and unrelated Android caches remain.
- Old build paths below and in historical logs now refer to removed caches;
  use the archive's same-named child for preserved packages/records. No
  compatibility links recreate build paths. Old packaging helpers need a
  fresh matching source/SDK build; never compile into the evidence archive.
- Audit: `work/logs/build-cleanup-20261026/REPORT.md`, JSON inventories and
  before/after hashes. Plan section 45. No binaries rebuilt or publication
  performed; this cleanup does not update release-publication status.

## Prepared release 0.7.1 — 2026-10-22

- I reported “O teste foi um sucesso” for the delivered upstream 2026.4.0
  predecessor **90b6e646**, Aroma ZIP SHA256
  `10e09be18d6f31731dce9659241f4948276229f5efb5aac8730eb269c7d08d0c`.
  No new log/game/scenario/model details supplied; preserve this as a user
  success report, not a comprehensive regression matrix.
- I requested preparing **0.7.1 for GitHub** and all physical GamePad and
  Wii Remote + Nunchuk buttons in README. Complete tables now cover sticks,
  D-pad, face/shoulder/trigger/stick-click/system buttons, equivalent inputs,
  IR vs analog precision, native keyboard, remapping and Xeen exceptions.
- Exact runtime/release source: **56bbe22e8c7350aa15bf25f7a030fdc633c130af**.
  Intended future tag: v0.7.1 at that commit, not later handoff HEAD.
  Upstream remains 2026.4.0; community version now 0.7.1. Runtime identifier:
  `2026.4.0-wiiu-56bbe22e`. Only port version metadata, build/staging defaults
  and docs differ from the tested predecessor; runtime implementations match.
- Fresh build: `work/build-scummvm-wiiu-0.7.1/`, same pinned r51 SDK, engine/
  codec flags and seven patches, diagnostics off. SDK doctor verified all
  2602 files and 6 symlinks. Config.h, compiler/linker flags and all packaged
  engine/theme/translation data match the tested predecessor exactly.
  Cross-build, ELF/PPC/imports/no strong undefineds, embedded RPX equality,
  unchanged icon, committed docs, ZIP CRC/layout/XML/licenses passed.
- Assets: `work/releases/0.7.1/`:
  Aroma ZIP SHA256 `fc706fbc5cea130f4263f8a253d3b27a4a901fce019787da16aa130e3879beb3`;
  experimental RPX ZIP SHA256
  `00d1e79a0fc3af84745b82329d488a0c88d8cb7544d43353a392bd9be4879488`.
  Also BUILD-INFO, SHA256SUMS and RELEASE-NOTES. Local PUBLISH.md and
  RELEASE-BODY.md prepare the exact source/tag/assets and GitHub text.
  Verifier: `work/package-0.7.1.py FULL_RUNTIME_COMMIT`.
- No separate hardware/Cemu run of the newly versioned 0.7.1 binary. Non-Aroma
  RPX, specific older pending scenarios and broader compatibility remain
  unverified. The reported predecessor success is recorded in public notes.
- **Prepared locally; no push, v0.7.1 tag or GitHub Release created.** Read-only
  fork check found default wiiu-aroma-port and v0.7.0 both at bc0452e7;
  v0.7.1 absent at check. Recheck before future publication. Older packages,
  SDK/default and Android caches preserved. Install ZIPs use committed
  README.WIIU, keeping unrelated local SDK-validation edits out of assets.
- Notes: checkout `backends/platform/sdl/wiiu/RELEASE-0.7.1.md`; detailed
  evidence in plan section 44 and `work/logs/release-0.7.1-*`.

## Upstream 2026.4.0 predecessor — 2026-10-21

- Official ScummVM **2026.4.0** tag merged, upstream commit
  `7d2709da5136866589f7602f5f5a7b17de03420e`; merge milestone `c3a4d080`.
  Runtime/package source: **`90b6e6461bff438c80d25af97e0560234711731f`**.
  That candidate used community Wii U version 0.7.0, independently of its base.
- Fresh full profile built with shared r51/GCC 16.2.0, all seven existing SDL
  patches and diagnostic/test macros disabled. Wii Remote adapter is byte-for-
  byte identical to confirmed `d37ee3f4`: 2 = Escape/back, Nunchuk C = slow
  analog pointer. Native keyboard/touch/audio/rendering/IO changes remain.
- Two merge conflicts resolved: preserve upstream event-manager purge instead
  of polling in Engine destruction; retain upstream SDL 1.x timer fallback
  alongside optional Wii U probes. Other touched port files merged cleanly.
- Input/IR/native keyboard ASan/UBSan host mocks, renderer/timer, SCI/BUN IO,
  MM keymaps, Groovie buffering and actual SDL pixel-conversion checks passed.
  Full cross-build/revision relink, ELF/PPC/imports/no strong undefineds,
  embedded RPX equality, unchanged icon, ZIP integrity/data/licenses passed.
  Generated Xeen mm.dat 1.3 restored to both stages; official translations used.
- Artifacts: `work/build-scummvm-wiiu-2026.4.0/`:
  `ScummVM-wiiu-2026.4.0-aroma.zip` (SHA256
  `10e09be18d6f31731dce9659241f4948276229f5efb5aac8730eb269c7d08d0c`),
  separate experimental RPX ZIP, ELF/RPX/WUHB, BUILD-INFO and SHA256SUMS.
  Verifier: `work/package-upstream-2026.4.0.py` (pass full runtime revision).
  Notes: checkout `backends/platform/sdl/wiiu/UPSTREAM-2026.4.0.md`.
- On October 22 I reported a successful console test of this exact delivered
  candidate before requesting 0.7.1. No new log/scenario details were supplied.
  Earlier “Tudo certo” feedback belongs to d37ee3f4. RPX remains unverified.
- SDK files/default, previous packages and unrelated Android caches untouched.
  Existing r51 documentation edits remain separate. No push/publication or Wii U
  release tag; the official upstream tag was fetched locally for integration.
  Detailed evidence: plan section 43 and `work/logs/upstream-2026.4.0-*`.

## Shared SDK migration — 2026-10-20

- I now activate `/home/renan/Documentos/wiiu-dev/sdk/env.sh` for all projects.
  Shared default: `sdk/current` -> `versions/r50-wut1.9.1-sdl2.32.7-local`.
  `DEVKITPRO`, `DEVKITPPC`, WUT and portlibs resolve to that same profile.
- The old outer-workspace `work/sdk` is a compatibility symlink. The profile
  includes a byte-verified compiler copy; `/opt/devkitpro` was left intact.
  Library/tool bytes and all seven SDL patches are preserved. Only activation
  scripts and dependency metadata were relocated; no SDK library rebuild.
- Fresh NeptuneGX, CMake SDL smoke and minimal SCUMM + JPEG/MP3/Ogg/Vorbis
  builds produced ELF/RPX/WUHB. This is cross-build/package verification only,
  not console testing or a new full 0.7.0 release build.
- Migration logs and report: `../sdk/validation/20261020/REPORT.md` relative to
  this outer workspace. 0.6.0, 0.7.0 and Remote packages still pass checksums.
  Existing build caches/releases were not rewritten; use fresh build dirs.
- Migration retains devkitPPC r50. Any r51 upgrade must use another profile,
  preserve r50 and validate before switching the shared default. Do not
  install SDK changes directly into a profile used by running builds.

## Shared instructions and SDK source cleanup — 2026-10-20

- Shared SDK/validation/provenance/cleanup rules are in the enclosing
  `wiiu-dev/AGENTS.md`; SDK maintenance has `sdk/AGENTS.md`. ScummVM's outer
  instructions now focus on SDL2 architecture and project build/release rules;
  the original bootstrap-heavy text is preserved as an audit snapshot.
- SDK sources/archives were moved to `../sdk/sources/`, platform smoke source
  to `../sdk/tests/sdl-smoke`, backups to `../sdk/backups/scummvm-sdk`, and
  old reference checkouts to `../sdk/references/`. All earlier `work/` paths
  remain compatibility links. The old SDK bootstrap is archived under SDK
  `legacy/`; do not run it against the active profile.
- Source/Git local modifications and files were preserved byte-for-byte;
  active SDK manifest and prepared release packages remain unchanged.
  SDK imports used by host mocks and the seven portable SDL patches in this
  checkout remain useful project files, not redundant SDK installations.
- Audit: `../sdk/audits/20261020-repository-cleanup/REPORT.md`. No emulator or
  console test was performed by this cleanup. Existing source build caches
  moved as historical data; use a new directory before rebuilding dependencies.

## Historical prepared 0.7.0 state

- Branch at verification: `wiiu-aroma-port`. Release binaries were built from
  **`bc0452e70e33061948e48cd5f7c6292f52639f44`**. Later documentation commits
  do not change that binary revision; do not retag a release to current HEAD.
- **0.7.0 is prepared locally. Publication is not verified.** No local v0.7.0
  tag was present at the 2026-10-20 check. This does not prove remote absence.
  Ask or check the remote read-only if publication status matters.
- `meu-github` points to `https://github.com/Rrhapsod/scummvm-wiiu.git`;
  `origin` points to upstream `https://github.com/scummvm/scummvm.git`.
  Verify before any authorized publication. No push/tag is implied by resuming.
- Both installation ZIPs, release notes and build info passed their existing
  SHA256SUMS on 2026-10-20. Keep them unchanged; a documentation handoff is not
  a request to regenerate the release.
- Pre-existing untracked `dists/android/.gradle/` and `dists/android/build/`
  belong to other work. Do not stage, remove or include them in packages.

## Wii Remote 2 / Nunchuk C update — 2026-10-21

- Runtime source/binary revision **`d37ee3f4bc2184b70f80b30d6caac3f84cb69da1`**.
  Remote **2 -> GamePad Y** (Escape/back through the keymap), **Nunchuk C ->
  GamePad R** (slow analog pointer). GamePad/Pro/Classic retain their mappings.
  Aggregation preserves shared held states across equivalent buttons.
- I authorized the new r51 SDK. A fresh full build uses the existing shared
  `r51-wut1.9.1-sdl2.32.7-local` profile, without modifying SDK files/defaults.
  Host ASan/UBSan input/IR and native keyboard regressions, full cross-build,
  revision relink and package checks passed. I subsequently reported “Tudo certo”
  for this mapping/r51 candidate before requesting 2026.4.0; no additional
  log or scenarios were supplied. This report does not cover the new base.
- Artifacts: `work/build-scummvm-wiiu-remote-20261021-escape/`, including
  `ScummVM-wiiu-remote-20261021-escape-aroma.zip` (SHA256
  `0549cf88ef01486386e5802e4514f48d61ec96c43877ad71fdb02807203483a1`),
  separate RPX ZIP, ELF/RPX/WUHB, BUILD-INFO and SHA256SUMS.
  Verifier: `work/package-remote-escape-20261021.py` (pass runtime revision).
  Runtime: `2026.3.1gitd37ee3f4`, port candidate `20261021 (2 = Escape)`.
- Pre-existing uncommitted r51 SDK documentation edits in README.WIIU and this
  handoff were preserved separately from this task's commits. The package build
  records the documentation-only diff against its runtime revision. Android
  caches, SDK profiles and previous release/candidate packages remain untouched.

## Original Wii Remote IR candidate — 2026-10-20

- Source/binary revision **`180f0769abc95e5ed1b13520a82a6805acaa25b5`**.
  Later handoff-only commits do not change it. Runtime identifies
  `2026.3.1git180f0769`, port `0.7.0 + Wii Remote IR candidate 20261020`.
- SDL reads IR from its existing KPAD samples, requests the external Sensor
  Bar and emits absolute mouse movement plus A/B clicks/drags. The Wii U
  adapter keeps all devices open, including a late GamePad, aggregates shared
  joystick buttons and selects the strongest axis. Touch/sticks yield idle IR.
  Disconnect/native keyboard/foreground paths release IR clicks. See the test
  guide for fixed A/B mouse behavior, Nunchuk and remaining keymapper buttons.
- On October 21 I reported that this build worked beautifully on my Wii U.
  My supplied log identifies `180f0769`, October 20 20:53:13–20:55:40, with
  Remote #1 IR movement, two keyboard cancellations and clean shutdown. It
  does not identify a game, Remote model or Sensor Bar type. Preserve this as
  a user hardware report, not a complete model/scenario matrix or r51 test.
  Evidence: `work/logs/remote-success-180f0769-20261021.log`. Non-Aroma remains
  unverified, as do specific combinations not identified by that report.
- Artifacts: `work/build-scummvm-wiiu-remote-20261020/`, with
  `ScummVM-wiiu-remote-20261020-aroma.zip` (SHA256
  `0b5974bdc708f8339f791f2b02ad1c16c862f87656becd1d9762f8e8b74372ba`),
  separate RPX ZIP, ELF/RPX/WUHB, `BUILD-INFO.txt` and `SHA256SUMS.txt`.
  Packaging verifier: `work/package-remote-20261020.py`.
- All seven SDL patches reconstruct the current modified files, retaining
  the documented KPADError adjustment. Host ASan/UBSan checks cover actual
  pointer/adapter code, hotplug/held release, native keyboard input/suspend,
  renderer lifetime and timer behavior. RPX/imports/undefineds, embedded RPX,
  unchanged icon/data, ZIP layout/licenses and Xeen 1.3 passed local checks.
- Prepared 0.7.0 packages retain their original hashes. No tag/push/publication.

## What has actually been tested

I reported successful hardware scenarios for Sam & Max, Last Crusade,
Fate of Atlantis return/reopen, Riven, Blade Runner, Curse, Xeen and QFG4
DOS/Windows. I also tested MP3, MT-32 and existing-save-name editing.
These are scenario reports, not full playthrough or all-edition guarantees.

The repeated-exit renderer fix was confirmed in the reported scenario.
Curse audio and Curse/QFG4 delays improved with the timer/IO changes.
On October 18, I reported good 11th Hour video/audio with candidate **66ba8c9a**.
Its log shows about 29–31 ms drawing/frame instead of 48 ms and normal teardown.
Some late frames and empty-queue observations remain: do not claim universal
Groovie fixes or constant 30 fps.

Still pending in the recorded evidence:

- Separate console execution of the versioned **0.7.1 ZIP**; the matching
  2026.4.0 predecessor has a success report with no scenario breakdown.
- Pro-only keyboard, late GamePad and HOME scenario.
- Console decoding with specifically identified Vorbis assets (host tests pass).
- Non-Aroma RPX compatibility and representative Myst/7th Guest reports.
- Community Clandestiny/Tender Loving Care/Uncle Henry reports; no reproducing
  files/logs were supplied for those cases. Do not invent detection hashes.

## Build and package invariants

- Native ScummVM -> existing SDL2 backend -> WUT. No RetroArch/custom replacement
  backend. Preserve the ScummVM name and existing upstream icon.
- 0.7.1 includes SCUMM v0–v8 (no HE), MM/Xeen (no MM1), SCI/SCI32, Mohawk
  Myst/MystME/Riven, Blade Runner and Groovie/Groovie2; MP3/Vorbis/MT-32/JPEG.
  The desktop compatibility list is not a Wii U support guarantee.
- Release helper: checkout `backends/platform/sdl/wiiu/build-release.sh`.
  Read SDK notes before building. Activate shared `../sdk/env.sh`; legacy
  `work/sdk/env.sh` forwards to it. Existing build caches may have stale paths.
  Fresh SDL (`build-wiiu-remote-20261020`) and ScummVM Remote builds now pass
  at the new location. WUT/codecs themselves were not rebuilt in this task.
- Prepared 0.7.1 uses **seven** incremental SDL patches in README.WIIU
  order; pointer precision adds patch 8 and adaptive stabilization adds
  patch 9 in separate candidate SDK profiles.
  Prepared 0.7.0 binaries used six. Preserved shared-default r51 SDL SHA256:
  `81db9275bbd57dd5fb7bd95d62a4c8da2f22f44db9c3ce3d6b30e00077c08d34`.
  The original October 20 Remote binary used the preserved r50 archive SHA256
  `d29ce16f335358b0327412a14faa993e7718736225f0afc6d9db1bf4c291113c`.
  The previous library (SHA256 `52f44bdc...982d`) and input source backups are
  in `work/sdk-backups/wiimote-before-20261020/`. Seventh patch:
  `sdl2-wiimote-pointer.patch`; it includes the Sensor Bar declaration already
  supported by WUT 1.9.1's imports. Do not rebuild WUT just to add that declaration.
- Xeen requires **mm.dat 1.3**. The 2026.4.0 bundled data also reports 1.3;
  older pre-upgrade bundles reported 1.2. Keep the source-generated archive
  restored by the helper in both stages; a plain staging make can replace it.
  Expected generated SHA256:
  `dae04efe8ce7032b12ba96539cbc0e8e49361edce4f6f66628225377d6286178`.
- Use one build process per build directory. Do not edit a shell build helper
  while it is running. Keep diagnostic candidates separate from final releases.
- Record exact source revision, SDK provenance, enabled features, checksums
  and hardware status. Check embedded RPX equality, ZIP integrity/data/licenses.
  Never bundle games, ROMs, saves, configuration or user logs.
- Current release ZIPs are in `work/releases/0.7.1/`. Older 0.7.0, Remote
  candidates and the user-confirmed upstream-update packages remain separate.

## Communication and next handoff

Speak Portuguese with me. Write public README/release text in English, using
first person for my authorship/tests, not "the maintainer." Keep generic audio
instructions and button/touch guides. MT-32 needs my own matching
`MT32_CONTROL.ROM` and `MT32_PCM.ROM`; do not distribute them.

Normal `SD:/scummvm/scummvm.log` is overwritten on relaunch; the compatibility
diagnostic log appends and may contain several builds. Identify the relevant
session/version before conclusions. Copy evidence before another console run.

When an authorized task changes the project or a handoff is requested, update
this short document with verified state, evidence and pending tests. Append
substantial history to the outer plan when available. A read-only question is
not an instruction to edit files. Do not auto-start old tests/builds/publication
from this document. The 2026.4.0 predecessor has my success report; 0.7.1 is
prepared locally with full controls documentation. Follow its exact runtime
commit for any future authorized publication, not the newest handoff HEAD.
