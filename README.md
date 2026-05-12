# MinUI — Miyoo Flip Enhanced

> **Unofficial fork** of [MinUI by shauninman](https://github.com/shauninman/MinUI) for the **Miyoo Flip** (platform: `my355`).  
> The original project does not accept pull requests; improvements here are maintained independently.

This project was born out of love for the Miyoo Flip — a console that deserves the best possible experience. Every improvement here is driven by the desire to make it feel just a little more right.

## Improvements

| Feature | Description |
| -- | -- |
| **Reduced input lag** | Audio no longer throttles the emulator. Uses dynamic resampling (libsamplerate) to keep audio in sync without blocking the core. Controls feel noticeably more responsive on PS1, GBA, and SNES. |
| **Save & Quit** | New option in the in-game pause menu — saves your progress and returns to the launcher in one step, without losing your place. |
| **Audio Quality** | Per-emulator resampling quality setting (Fast / Good). Lets you trade CPU usage for audio fidelity on a per-system basis. |

## Installation (already have MinUI?)

If you already have MinUI installed on your Miyoo Flip, you only need to replace one file — no full reinstall required.

### Step 1 — Download the binary

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download `minarch.elf`.

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

## Building from source

The toolchain uses Docker for ARM64 cross-compilation on x86_64 hosts. See `toolchains/my355-toolchain/Dockerfile` for the build environment. The key dependency is libsamplerate 0.2.2, built as a static ARM64 library inside the container.

---

# About MinUI

MinUI is a focused, custom launcher and libretro frontend for retro handhelds, created by [shauninman](https://github.com/shauninman/MinUI).
