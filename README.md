# MinUI — Miyoo Flip Fork

> **Unofficial fork** of [MinUI by shauninman](https://github.com/shauninman/MinUI), focused on the **Miyoo Flip** (platform: `my355`).  
> Original project does not accept pull requests; changes here are maintained independently.

## What's different in this fork

| Change | Details |
| -- | -- |
| **Save & Quit** | New option in the in-game pause menu that saves state and returns to the launcher in one step |
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
