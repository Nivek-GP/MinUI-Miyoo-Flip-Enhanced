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
| **Smart text overflow** | Long game names no longer overlap cover art, and long cheat names no longer cover the On/Off toggle. Unselected items truncate cleanly with `…`; the selected item scrolls horizontally so the full name is always readable. |
| **Delete cheats in-game** | Press **Y** on any cheat in the Cheats menu to delete it from the `.cht` file. A confirmation dialog shows the full cheat name (wrapping across lines if needed) before removing it. Useful for cleaning up cheats that don't work without having to edit files on a PC. |
| **Collections Manager** | On-device tool to create, rename, and delete game collections, and add or remove games from them — all without a PC. Launches from **Extras → Tools → Collections**. |

## Download

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download the files you need:

| File | What it is |
| --- | --- |
| `minui.elf` | Main launcher binary |
| `minarch.elf` | Emulator core binary |
| `libmsettings.so` | Settings library (brightness, volume, etc.) |
| `Collections.pak.zip` | On-device collections manager tool |

## Installation

If you already have MinUI installed on your Miyoo Flip, you only need to replace a few files — no full reinstall required.

### Step 1 — Download

Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download `minui.elf`, `minarch.elf`, and `libmsettings.so`.

### Step 2 — Locate the files on your SD card

Insert your SD card into your PC. Navigate to:

```
.system/my355/bin/    ← for minui.elf and minarch.elf
.system/my355/lib/    ← for libmsettings.so
```

> **Note:** `.system` is a hidden folder. On Windows, enable "Show hidden items" in File Explorer (View → Show → Hidden items).

### Step 3 — Replace the files

1. Make a backup of each original file (rename to `.bak`)
2. Copy `minui.elf` and `minarch.elf` into `.system/my355/bin/`
3. Copy `libmsettings.so` into `.system/my355/lib/`
4. Safely eject the SD card and reinsert it into the device

### Step 4 — Verify

- **Game list**: navigate to a folder with cover art — long game names should no longer overlap the artwork.
- **Cheats**: open a game → **Menu → Options → Cheats** — long cheat names scroll instead of overlapping the On/Off toggle.

To revert, delete the new files and rename the `.bak` files back to their original names.

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
3. Toggle individual cheats On/Off with **left/right** — changes apply immediately
4. Press **Y** on any cheat to delete it from the file (a confirmation dialog will appear)
5. Cheats are re-applied automatically when loading a save state
6. On/Off state and any deletions are saved back to the `.cht` file when you exit the Cheats menu

## Game art

MinUI displays cover art next to the selected game in the list. Art is loaded from `.png` files placed in a hidden `.res` folder alongside your ROMs.

> Use [ArtSync](https://github.com/Nivek-GP/ArtSync) to automatically download boxart for your entire ROM collection.

### File placement

Art files live in a `.res` folder inside the same directory as the ROM, named after the ROM file with `.png` appended:

```
/Roms/{system}/
├── Castlevania - Symphony of the Night (USA).bin
└── .res/
    └── Castlevania - Symphony of the Night (USA).bin.png
```

| Game type | ROM file | Art filename |
| --- | --- | --- |
| Single disc | `Crash Bandicoot (USA).bin` | `.res/Crash Bandicoot (USA).bin.png` |
| Multi-disc (M3U) | `Resident Evil 2 (Spain).m3u` | `.res/Resident Evil 2 (Spain).m3u.png` |
| Folder | `Castlevania/` | `.res/Castlevania.png` |

> **Note:** `.res` is a hidden folder. On Windows, enable "Show hidden items" in File Explorer to see it.

### Image requirements

- Format: PNG
- Maximum size: `273 × 273` pixels (images are displayed as-is, no scaling)
- Smaller images are centered within the art area

### Usage

1. Place the `.png` file in the `.res` folder next to the ROM
2. Navigate to that game in the list — the art appears automatically on the right side of the screen

## Collections Manager

An on-device tool for managing MinUI collections without a PC. Launch it from **Extras → Tools → Collections**.

### Installation

1. Go to the [Releases page](https://github.com/Nivek-GP/MinUI-Miyoo-Flip-Enhanced/releases) and download `Collections.pak.zip`
2. Extract and copy the `Collections.pak/` folder to `EXTRAS/Tools/my355/` on your SD card

### Usage

Collections are stored as `.txt` files in `/Collections/` on the SD card (one game path per line). The tool creates this folder automatically on first launch.

| Screen | Button | Action |
| --- | --- | --- |
| Collection list | **A** | Open collection / create new |
| Collection list | **X** | Rename collection |
| Collection list | **Y** | Delete collection (confirmation required) |
| Collection list | **B** | Exit |
| Inside a collection | **A** on `[+ Add Game]` | Browse Roms and add a game |
| Inside a collection | **Y** | Remove game (confirmation required) |
| Inside a collection | **B** | Back to collection list |

## Building from source

The toolchain uses Docker for ARM64 cross-compilation on x86_64 hosts. See `toolchains/my355-toolchain/Dockerfile` for the build environment. The key dependency is libsamplerate 0.2.2, built as a static ARM64 library inside the container.

## Related

- [CheatSync](https://github.com/Nivek-GP/CheatSync) — desktop app to auto-download `.cht` cheat files for your ROM collection
- [ArtSync](https://github.com/Nivek-GP/ArtSync) — desktop app to auto-download boxart for your ROM collection
- [minui-portmaster-flip](https://github.com/Nivek-GP/minui-portmaster-flip) — PortMaster PAK for Miyoo Flip
- [MinUI](https://github.com/shauninman/MinUI) — the original project by shauninman
- [libretro-database](https://github.com/libretro/libretro-database) — source of all cheat files
