# Wii U port — start here in a new chat

Last verified locally: **2026-10-22**. This is a compact handoff, not the full
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
| Current 0.7.1 release/base update | checkout `backends/platform/sdl/wiiu/RELEASE-0.7.1.md` |
| Build/SDK and all seven SDL patches | checkout `backends/platform/sdl/wiiu/README.WIIU` |
| Full GamePad / Remote + Nunchuk controls | checkout `README.md`, Controls |
| Current prepared release changes | checkout `backends/platform/sdl/wiiu/RELEASE-0.7.1.md` |
| Binary provenance/packages/checksums | `work/releases/0.7.1/BUILD-INFO.txt`, `SHA256SUMS.txt`, `PUBLISH.md` |
| Detailed history/evidence | `docs/WIIU_PORT_PLAN.md`, relevant complete sections |
| Native input and regression tests | checkout `backends/platform/sdl/wiiu/tests/` |
| SDK source/install and archived logs | shared `../sdk/sources/`, `../sdk/current/`, `../sdk/backups/`; project `work/logs/`; old SDK/source paths are compatibility links |

Plan sections: 26–27 renderer freeze; 28 Xeen/data/compatibility; 29–32 Curse
audio and QFG4 loading; 33 text editing/Vorbis; 34 controller keyboard;
35–36 Groovie FMV and 0.7.0; 38 original Wii Remote IR candidate; 42 button 2/C
swap and hardware feedback; 43 upstream 2026.4.0; 44 release 0.7.1.
Read the whole plan for an architectural audit or
when explicitly requested, not by default for every small follow-up.
`WIIU_IMPLEMENTATION_STATUS.md` describes the historical first integration,
not the current release. Missing outer files in a standalone clone mean local
evidence is unavailable; they do not imply the source port is absent.

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
- Current source requires all **seven** incremental SDL patches in README.WIIU
  order; the prepared 0.7.0 binaries used six. Current shared r51 SDL SHA256:
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
