# MinUI — Miyoo My-355 Fork

> **Unofficial fork** of [MinUI by shauninman](https://github.com/shauninman/MinUI), focused on the **Miyoo My-355** platform.  
> Original project does not accept pull requests; changes here are maintained independently.

## What's different in this fork

| Change | Details |
| -- | -- |
| **Save & Quit** | New option in the in-game pause menu that saves state and returns to the launcher in one step |
| **Menu layout fix** | Pause menu items are now evenly centered between the title bar and button hints with equal margins |
| **ARM64 cross-compilation** | Added `immintrin.h` stub and Docker toolchain fixes for building on x86_64 hosts |

## Installation (already have MinUI?)

If you already have MinUI installed on your Miyoo My-355, you only need to replace one file — no full reinstall required.

### Step 1 — Download the binary

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-My355/releases) and download `minarch.elf`.

### Step 2 — Locate the file on your SD card

Insert your SD card into your PC. Navigate to:

```
.system/my355/bin/minarch.elf
```

> **Note:** `.system` is a hidden folder. On Windows, make sure "Show hidden items" is enabled in File Explorer (View → Show → Hidden items).

### Step 3 — Replace the file

1. Make a backup copy of the original `minarch.elf` (rename it to `minarch.elf.bak`)
2. Copy the downloaded `minarch.elf` into `.system/my355/bin/`
3. Safely eject the SD card and reinsert it into the device

### Step 4 — Verify

Open any game and press **MENU** to open the pause menu. You should now see **Save & Quit** between Options and Quit.

To revert, delete the new file and rename `minarch.elf.bak` back to `minarch.elf`.

---

# About MinUI

MinUI is a focused, custom launcher and libretro frontend for retro handhelds, created by [shauninman](https://github.com/shauninman/MinUI).

<img src="github/minui-main.png" width=320 /> <img src="github/minui-menu-gbc.png" width=320 />

## Features

- Simple launcher, simple SD card
- No settings or configuration
- No boxart, themes, or distractions
- Automatically hides hidden files
  and extension and region/version
  cruft in display names
- Consistent in-emulator menu with
  quick access to save states, disc
  changing, and emulator options
- Automatically sleeps after 30 seconds
  or press POWER to sleep (and wake)
- Automatically powers off while asleep
  after two minutes or hold POWER for
  one second
- Automatically resumes right where
  you left off if powered off while
  in-game, manually or while asleep
- Resume from manually created, last
  used save state by pressing X in
  the launcher instead of A
- Streamlined emulator frontend
  (minarch + libretro cores)

## Supported consoles

Base:

- Game Boy
- Game Boy Color
- Game Boy Advance
- Nintendo Entertainment System
- Super Nintendo Entertainment System
- Sega Genesis
- PlayStation

Extras:

- Neo Geo Pocket (and Color)
- Pico-8
- Pokémon mini
- Sega Game Gear
- Sega Master System
- Super Game Boy
- TurboGrafx-16 (and TurboGrafx-CD)
- Virtual Boy
