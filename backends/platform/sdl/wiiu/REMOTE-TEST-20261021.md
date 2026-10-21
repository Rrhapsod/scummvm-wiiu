# Wii Remote button 2 / Escape update — October 21, 2026

I reported that the first Wii Remote IR candidate worked beautifully on my
Wii U and requested Escape on Remote button 2. My supplied log identifies
`2026.3.1git180f0769`, port `0.7.0 + Wii Remote IR candidate 20261020`, from
October 20, 20:53:13–20:55:40. It records Wii Remote #1, IR mouse movement,
two native keyboard cancellations and a clean shutdown. It does not identify
a game, Remote model, Sensor Bar type or a complete hardware test matrix.

This update makes **Wii Remote 2 the same logical Y input as GamePad Y**.
ScummVM's current keymap supplies Escape/back/cancel or the game-specific Y
action. Existing custom Y remaps apply to both. Previously 2 was the right
shoulder binding, normally slowing the analog virtual mouse; it was not Escape.
I requested swapping 2 with Nunchuk C: C now shares GamePad R, normally slowing
the analog virtual mouse for precision. Holding GamePad Y and Remote 2 together,
or GamePad R and Nunchuk C, keeps the shared action held until both are released.

| Input | Default behavior |
| --- | --- |
| Wii Remote IR | Absolute mouse movement |
| Wii Remote A / B | Left / right mouse click; hold A to drag |
| Wii Remote 2 | GamePad Y action: Escape/back/cancel where mapped |
| Wii Remote 1 | Left shoulder action (game menu/F5 where mapped) |
| Wii Remote Plus / Minus | Global menu / native keyboard |
| Nunchuk stick / Z / C | Left stick / X / right shoulder (slow analog pointer) |
| GamePad R | Existing slow analog pointer action |

The runtime identifier is
`0.7.0 + Wii Remote IR candidate 20261021 (2 = Escape)`.
I still need to check this new mapping on my console; success reported for
the earlier binary does not validate the updated one. I also authorized using
the new shared r51 SDK (GCC 16.2.0), with its rebuilt libraries and the same
seven SDL patches. This build uses a fresh directory; SDK profiles and earlier
packages remain intact. Neither the remapping nor this r51 binary has a console
test yet.

## Install and test

Extract the Aroma ZIP onto the SD card, merge directories and replace
`wiiu/apps/ScummVM.wuhb`. Keep the previous WUHB and preserve games, saves and
configuration. The original 0.7.0 and October 20 Remote packages are unchanged.
The separate RPX ZIP remains experimental outside Aroma.

1. Open a launcher settings dialog and press Remote 2: it should perform the
   same back/cancel action as GamePad Y.
2. In a game, compare Remote 2 with GamePad Y for Escape/skip/cancel. Engine
   mappings and custom remaps may differ; both buttons should behave alike.
3. Check that pressing and releasing 2 sends one action without a stuck key.
   If available, also hold GamePad Y while pressing/releasing 2. Nunchuk C should
   slow the Nunchuk analog pointer, and share its held state with GamePad R.
4. Check IR movement, A/B clicks, button 1, Plus/Minus and the native keyboard.
   GamePad R should retain its previous slow analog pointer function.
5. Return to the launcher and Quit. Copy `SD:/scummvm/scummvm.log` before the
   next launch, which overwrites the normal log.

For broader IR regression checks, also test the four screen edges, a 4:3 game,
losing/reacquiring IR, Remote disconnection while holding A and HOME return.
Remote model and Sensor Bar type were not identified in my success report.

## Build

Activate an explicit SDK and run `build-remote-test.sh` from this backend.
The helper defaults to a new `build-scummvm-wiiu-remote-20261021-escape`
directory, retaining the complete 0.7.0 engine/codec profile, bounded IR logs,
seven existing SDL patches and generated Xeen `mm.dat` 1.3.
