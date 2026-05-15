<div align="center">
  <h1>MinUI — Miyoo Flip Enhanced</h1>
  <p>Unofficial MinUI fork for the <strong>Miyoo Flip</strong> — reduced input lag, cheat support, and quality-of-life improvements.</p>
  <img src="https://img.shields.io/badge/device-Miyoo%20Flip-blue" alt="Device">
  <img src="https://img.shields.io/badge/based%20on-MinUI%20by%20shauninman-grey" alt="Based on MinUI">
</div>

> **Unofficial fork** of [MinUI by shauninman](https://github.com/shauninman/MinUI) for the **Miyoo Flip** (platform: `my355`).  
> The original project does not accept pull requests; improvements here are maintained independently.

This project was born out of love for the Miyoo Flip — a console that deserves the best possible experience. Every improvement here is driven by the desire to make it feel just a little more right.

## Improvements

| Feature | Description |
| --- | --- |
| **Reduced input lag** | Audio no longer throttles the emulator. Uses dynamic resampling (libsamplerate) to keep audio in sync without blocking the core. Controls feel noticeably more responsive on PS1, GBA, and SNES. |
| **Cheat codes** | Per-game cheat support via `.cht` files (RetroArch / libretro-database format). Toggle cheats from the in-game Options menu; state persists across sessions. |
| **Save & Quit** | New option in the in-game pause menu — saves your progress and returns to the launcher in one step, without losing your place. |

## Download

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download `minarch.elf`.

## Installation

If you already have MinUI installed on your Miyoo Flip, you only need to replace one file — no full reinstall required.

### Step 1 — Download

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download `minarch.elf`.

### Step 2 — Locate the file on your SD card

Insert your SD card into your PC. Navigate to:

```
.system/my355/bin/minarch.elf
```

> **Note:** `.system` is a hidden folder. On Windows, enable "Show hidden items" in File Explorer (View → Show → Hidden items).

### Step 3 — Replace the file

1. Make a backup copy of the original `minarch.elf` (rename it to `minarch.elf.bak`)
2. Copy the downloaded `minarch.elf` into `.system/my355/bin/`
3. Safely eject the SD card and reinsert it into the device

### Step 4 — Verify

Open any game and press **MENU** to open the pause menu. You should now see **Save & Quit** between Options and Quit.

To revert, delete the new file and rename `minarch.elf.bak` back to `minarch.elf`.

## Cheat codes

Cheats are loaded from `.cht` files placed on the SD card. The format is compatible with [libretro-database](https://github.com/libretro/libretro-database/tree/master/cht).

> Use [CheatSync](https://github.com/Nivek-GP/CheatSync) to automatically download `.cht` files for your entire ROM collection.

### File placement

```
/Cheats/{core}/
```

| Game type | Example filename |
| --- | --- |
| Single disc | `Crash Bandicoot (USA).bin.cht` |
| Multi-disc (M3U) | `Resident Evil 2 (Spain).m3u.cht` |

The core tag matches the system folder name on your SD card (`PS`, `GBA`, `SFC`, etc.).

If no `.cht` file is found, the Cheats menu shows the exact path where to place it.

### Usage

1. Copy a `.cht` file to `/Cheats/{core}/` on the SD card, named after the ROM file (including extension)
2. Launch the game → **Menu → Options → Cheats**
3. Toggle individual cheats On/Off — changes apply immediately
4. Cheats are re-applied automatically when loading a save state
5. On/Off state is saved back to the `.cht` file when you exit the Cheats menu

## Building from source

The toolchain uses Docker for ARM64 cross-compilation on x86_64 hosts. See `toolchains/my355-toolchain/Dockerfile` for the build environment. The key dependency is libsamplerate 0.2.2, built as a static ARM64 library inside the container.

## Related

- [CheatSync](https://github.com/Nivek-GP/CheatSync) — desktop app to auto-download `.cht` cheat files for your ROM collection
- [MinUI](https://github.com/shauninman/MinUI) — the original project by shauninman
- [libretro-database](https://github.com/libretro/libretro-database) — source of all cheat files
