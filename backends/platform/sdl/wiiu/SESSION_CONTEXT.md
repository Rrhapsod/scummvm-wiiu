# Wii U port — start here in a new chat

Last verified locally: **2026-10-20**. This is a compact handoff, not the full
conversation or proof that every earlier test still applies. Recheck Git/files
before acting; the current user request takes precedence over old next steps.

## Workspace and reading map

The local workspace is now `/home/renan/Documentos/scummvm`, with the Git
checkout at `work/scummvm`. The former path below `VisualStudio` no longer
exists. Paths in historical build logs and publication guides may still refer
to that location; do not blindly execute them or rewrite preserved evidence.

Paths below are relative to the **outer workspace** unless marked checkout:

| Need | Read |
| --- | --- |
| Durable architecture/work rules | `AGENTS.md` and checkout `AGENTS.md` |
| Current user features/controls/audio | checkout `README.md` |
| Build/SDK and all six SDL patches | checkout `backends/platform/sdl/wiiu/README.WIIU` |
| Current prepared release changes | checkout `backends/platform/sdl/wiiu/RELEASE-0.7.0.md` |
| Binary provenance/packages/checksums | `work/releases/0.7.0/BUILD-INFO.txt`, `SHA256SUMS.txt`, `PUBLISH.md` |
| Detailed history/evidence | `docs/WIIU_PORT_PLAN.md`, relevant complete sections |
| Native input and regression tests | checkout `backends/platform/sdl/wiiu/tests/` |
| SDK source/install and archived logs | `work/SDL-devkitpro/`, `work/sdk/`, `work/logs/` |

Plan sections: 26–27 renderer freeze; 28 Xeen/data/compatibility; 29–32 Curse
audio and QFG4 loading; 33 text editing/Vorbis; 34 controller keyboard;
35–36 Groovie FMV and 0.7.0. Read the whole plan for an architectural audit or
when explicitly requested, not by default for every small follow-up.
`WIIU_IMPLEMENTATION_STATUS.md` describes the historical first integration,
not the current release. Missing outer files in a standalone clone mean local
evidence is unavailable; they do not imply the source port is absent.

## Current recorded state

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

- A brief hardware test of the exact **diagnostics-disabled 0.7.0 ZIP**:
  movie/audio, return/reopen, save-name editing and Quit.
- Pro-only keyboard, late GamePad and HOME scenario.
- Console decoding with specifically identified Vorbis assets (host tests pass).
- Non-Aroma RPX compatibility and representative Myst/7th Guest reports.
- Community Clandestiny/Tender Loving Care/Uncle Henry reports; no reproducing
  files/logs were supplied for those cases. Do not invent detection hashes.

## Build and package invariants

- Native ScummVM -> existing SDL2 backend -> WUT. No RetroArch/custom replacement
  backend. Preserve the ScummVM name and existing upstream icon.
- 0.7.0 includes SCUMM v0–v8 (no HE), MM/Xeen (no MM1), SCI/SCI32, Mohawk
  Myst/MystME/Riven, Blade Runner and Groovie/Groovie2; MP3/Vorbis/MT-32/JPEG.
  The desktop compatibility list is not a Wii U support guarantee.
- Release helper: checkout `backends/platform/sdl/wiiu/build-release.sh`.
  Read SDK notes before building. Inspect `work/sdk/env.sh` and actual tool
  paths after the workspace move; existing build caches may have stale paths.
  No toolchain rebuild has been validated at the new location by this handoff.
- Keep all six incremental SDL patches in README.WIIU order. Installed
  `work/sdk/portlibs/wiiu/lib/libSDL2.a` SHA256, rechecked 2026-10-20:
  `52f44bdc56f7d9d760050fad10ef7b60c34edda145b3e9f6fd0e42f8f687982d`.
- Xeen requires generated **mm.dat 1.3**, not bundled upstream 1.2. The release
  helper restores the generated file to both stages; a plain staging make can
  overwrite it. Expected generated SHA256:
  `dae04efe8ce7032b12ba96539cbc0e8e49361edce4f6f66628225377d6286178`.
- Use one build process per build directory. Do not edit a shell build helper
  while it is running. Keep diagnostic candidates separate from final releases.
- Record exact source revision, SDK provenance, enabled features, checksums
  and hardware status. Check embedded RPX equality, ZIP integrity/data/licenses.
  Never bundle games, ROMs, saves, configuration or user logs.
- Existing ZIPs are under `work/releases/0.7.0/`, named
  `ScummVM-wiiu-0.7.0-aroma.zip` and
  `ScummVM-wiiu-0.7.0-rpx-experimental.zip`.

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
from this document. No next feature has been selected by this handoff.
