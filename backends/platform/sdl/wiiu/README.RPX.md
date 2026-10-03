# ScummVM Wii U - experimental RPX package

**For community testing outside Aroma. Compatibility has NOT been verified.**

This package contains the same RPX executable embedded in the matching
Aroma WUHB. The corrected diagnostic build has positive Aroma hardware reports;
the final 0.6.0 binaries still need a console smoke test. Non-Aroma remains unverified.
It does not include runtime changes specifically for other loaders.
Packaging an RPX is not proof that it works with Tiramisu or any other setup.
Startup, filesystem access, native keyboard, audio and application lifecycle
may differ between environments. Please report failures as well as successes.

## Choose one package

- **Aroma users:** use the Aroma ZIP containing `wiiu/apps/ScummVM.wuhb`.
- **Non-Aroma testers:** use the RPX experimental ZIP with an already
  configured Wii U Homebrew Launcher and loader capable of launching RPX files.

This is a native **Wii U** application, not a Wii/vWii Homebrew Channel app.
It is not a WUP installer or a channel package. Do not rename the RPX to ELF.
Do not change your console setup just to run this test.

## Installation

1. Back up `SD:/scummvm/scummvm.ini` and `SD:/scummvm/saves/`, if present.
   The RPX and WUHB share these paths and can modify the same files.
2. Extract the ZIP contents to the root of the SD card, merging directories.
   Do not replace or delete an existing `scummvm` directory.
3. Confirm this layout:

```text
SD:/
├── README-RPX.md
├── wiiu/apps/scummvm/
│   ├── ScummVM.rpx
│   └── meta.xml
└── scummvm/
    ├── data/
    └── doc/
```

4. From your existing Wii U Homebrew Launcher, select **ScummVM (RPX test)**.
5. If the ScummVM launcher opens, choose **Add Game** and browse to your own
   game files. See `scummvm/doc/README.md` for the engines in this build;
   games and MT-32 ROMs are not included.

Keep the supplied `scummvm/data` directory; do not distribute only the RPX.
The file browser uses `/vol/external01/` for the SD card. The optional HBL
`icon.png` is not included; the tested Aroma WUHB still has its ScummVM icon.

## Quick control guide

| Input | Default action |
| --- | --- |
| Touchscreen / short tap | Position pointer / left click |
| Left stick | Move pointer |
| A / B | Left click / right click in games |
| Hold R | Slow pointer movement |
| L | Game menu / F5 where supported |
| + | ScummVM global menu |
| Minus | Open native keyboard |
| X / Y | Skip dialogue / Escape where supported |

In launcher dialogs, A clicks and Y closes/cancels. Mappings may vary by game.
Keyboard OK inserts text but does not activate Save. Wait until the full name
appears before saving. Keyboard Cancel should close only the keyboard.

## Test checklist

Use a new save slot and a short session before testing longer gameplay.

1. **Startup:** does the launcher appear on both TV and GamePad?
2. **SD:** can Add Game browse the SD and find the game?
3. **Input:** test touch, stick, A/B clicks and R precision movement.
4. **Keyboard:** test automatic opening and Minus, confirm a multi-letter name,
   cancel another entry, and reopen. Check that buttons still work afterward.
5. **Audio:** verify music, effects and speech available in your game edition.
6. **Saves:** create a new save and reload it. Do not overwrite important saves.
7. **Exit/relaunch:** use the launcher's Quit button. Record whether it returns
   to HBL, the Wii U Menu, or hangs; do not assume a particular destination.
8. **HOME:** after the basics work, test leaving and returning. Record any
   freeze, loss of video/audio or input. Lifecycle handling is unverified here.
9. Relaunch and check whether configuration and saves persisted.

## Report results

Open an issue at https://github.com/Rrhapsod/scummvm-wiiu/issues and include:

- Package filename and ScummVM version displayed by the launcher or log.
- Wii U firmware and exact homebrew environment, including its version.
- Homebrew Launcher version and how you launched it; confirm Aroma was not active.
- Game title, edition/platform and language.
- Results for each checklist item, especially keyboard, HOME and Quit.
- A photo/video of any failure and `SD:/scummvm/scummvm.log`, if created.

**Copy the log before relaunching; it is overwritten.** If startup fails before
the log is created, report that fact. Review logs for private information and
do not attach copyrighted game data. A successful test on one setup does not
establish support for every modified Wii U.

## Format references

The upstream [Homebrew Launcher release notes](https://github.com/dimok789/homebrew_launcher/releases/tag/1.4)
document RPX support and the need for a suitable loader/payload. Its
[README](https://github.com/dimok789/homebrew_launcher) documents the SD app
directory and optional metadata. These references describe the loader, not
validation of this ScummVM build.
