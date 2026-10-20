# Wii Remote IR candidate — October 20, 2026

I prepared this separate candidate to add Wii Remote pointing to my native
ScummVM Wii U/Aroma port. It retains the 0.7.0 engines, codecs and existing
renderer/audio/IO/keyboard fixes. The WUHB menu name and icon remain ScummVM.
I have not yet tested the new Wii Remote support on my console. Host mocks
and PowerPC compilation cannot validate Bluetooth, the Sensor Bar or IR accuracy.

## Installation and controls

Keep a copy of the working WUHB. Extract the contents of the matching Aroma ZIP
onto the SD card, merging directories. Replace `wiiu/apps/ScummVM.wuhb` for this
test; preserve existing `scummvm` games, configuration and saves. The separate
RPX package is experimental and has not been validated through non-Aroma loaders.

Pair the Wii Remote using the console's normal controller synchronization.
Position the external Sensor Bar above/below the TV as in the console settings.
This candidate requests the console-powered bar through `WPADSetSensorBar` and
IR through `KPADEnableDPD`; actual bar power and alignment need console testing.
A separately powered bar is useful for diagnosing missing IR. GamePad Sensor
Bar control and gyro-based aiming are not part of this candidate.

| Input | Action |
| --- | --- |
| Wii Remote IR | Absolute mouse movement, with a small jitter threshold |
| Wii Remote A / B | Left / right mouse button; hold A to drag |
| Wii Remote Plus / Minus | Standard Start / Back bindings (normally menu / native keyboard) |
| Wii Remote D-pad | Standard directional bindings |
| Wii Remote 1 / 2 | Standard left / right shoulder bindings |
| Nunchuk stick / Z / C | Left stick / X / Y bindings |
| GamePad / Pro / Classic | Existing button/keymapper and stick controls |
| GamePad touchscreen | Existing direct touch handling |
| HOME | Existing system/ProcUI behavior; test return and clean exit |

All detected Wii U devices stay open, including a GamePad connected after a
Remote. Controller indices no longer select the sole input device. Shared
joystick buttons stay held until all contributing devices release them; the
strongest stick deflection wins. The Remote's A/B mouse events bypass duplicate
joystick clicks. Other buttons continue through ScummVM's keymapper.

Touch/sticks temporarily yield the cursor away from idle IR. Deliberate Remote
movement or a fresh A/B press can reclaim it; an active IR drag keeps ownership.
Missing/invalid IR leaves the cursor where it was, while button releases still
work. Remote disconnects, extension changes, native keyboard input and foreground
loss release IR mouse buttons. Held native keyboard confirmations are masked
until release, as in the previous keyboard fixes.

## Console test sequence

1. Start with GamePad + a paired bare Remote. Check whether the original
   console-powered Sensor Bar illuminates (a phone camera may show the IR LEDs).
   Check hover and both clicks in the launcher, including all four screen edges.
2. Repeat with Remote Plus and Remote + Nunchuk if available. Check directions,
   Plus, Minus and 1/2, including user remapping. Check the Nunchuk stick and Z/C.
3. Open a 4:3 game such as Sam & Max; check precise pointing, both clicks and
   dragging. Compare the launcher and 640x480/high-resolution game coordinates.
   Record the TV output resolution and Sensor Bar position if alignment is wrong.
4. Point away from the bar and back. The cursor should remain still without
   valid IR, resume when pointing returns, and release A/B even while IR is lost.
5. Use touch and the GamePad stick, then return to IR. An idle Remote should not
   immediately pull the cursor back. Check Pro/Classic input if available.
6. Open the native keyboard with Minus, confirm/cancel, and check for unwanted
   clicks in the underlying dialog. Test existing-save-name editing again.
7. Disconnect the Remote while holding A, reconnect it, and change its extension.
   Check that no click/stick remains held. Also start with Remote only and connect
   the GamePad later; check that touch and the keyboard become available.
8. Press HOME during normal play and during a drag/keyboard; return, reopen a
   game, and Quit. Check audio/video, saving and loading after the input changes.

The diagnostic identifier is `0.7.0 + Wii Remote IR candidate 20261020`.
Copy `SD:/scummvm/scummvm.log` immediately after testing, before launching again:
the normal log is overwritten on the next launch. It records controller instance
names, up to 64 IR tracking transitions and the first 32 IR mouse events. Report
Remote model/extension, Sensor Bar type/position, TV resolution, game/scene and
whether GamePad/Pro/Classic were connected. Hardware calibration remains pending.

## Reproducing the candidate

Apply the seven SDL patches in README.WIIU order and rebuild/install SDL. The
last patch uses the WUT 1.9.1 Sensor Bar import already present in `libwut.a`, with
the declaration missing from that SDK's header. The original 0.7.0 SDL archive
and source files should be preserved before installing the updated SDK library.
Activate the SDK and run `bash backends/platform/sdl/wiiu/build-remote-test.sh`.
Use a fresh build directory after moving the workspace; old CMake/configure
caches may contain the previous absolute path. Generate Xeen `mm.dat` 1.3 through
the helper; do not replace it with the bundled upstream 1.2 file.

The SDL pointer extension is opt-in through `SDL_WIIU_REMOTE_POINTER=1`.
Mouse IDs `0x57494900..03` identify WPAD channels. Diagnostic `SDL_USEREVENT`
codes `0x57495200..07` encode `(channel * 2 + trackingValid)` and contain no
borrowed data pointers. ScummVM consumes those diagnostics and handles the
mouse events through its existing viewport/scale conversion.
