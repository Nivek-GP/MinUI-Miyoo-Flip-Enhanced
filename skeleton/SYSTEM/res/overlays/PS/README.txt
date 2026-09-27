Sony PlayStation  —  overlay folder for console tag "PS"
Native resolution: 320x240

An overlay is a full-screen PNG (transparent where the game shows through) drawn
over the running game on the Miyoo Flip's 640x480 screen. Enable one in-game via
Options > Frontend > Overlay. The list here is per-console: only files in this
folder appear when running a "PS" game.

REQUIREMENTS
  - Each overlay is a PAIR: <name>.png AND <name>.cfg with the same <name>.
  - PNG: 640x480, 32-bit RGBA. Opaque pixels cover the game; alpha shows it.
  - An overlay with no matching .cfg is ignored.

THE .cfg FORMAT (RetroArch-compatible; extra keys below are MinUI-only)
  overlay0_overlay     = <name>.png
  overlay0_full_screen = "true"
  overlay0_descs        = "0"
  overlay0_name         = "overlay0"
  overlay0_normalized   = "true"
  overlays              = "1"

  # MinUI extension: where the game image (and Screen Effect, if on) is drawn,
  # in 640x480 logical pixels. Use this when the art has a "window" the game
  # must sit inside (a handheld shell, an off-centre bezel). Omit it, or set
  # w/h to 0, for a plain full-screen overlay (CRT bezels, scanline art) that
  # leaves the image wherever Screen Scaling puts it.
  minui_viewport_x = 0
  minui_viewport_y = 0
  minui_viewport_w = 640
  minui_viewport_h = 480

FINDING THE VIEWPORT NUMBERS
  Open the PNG in an image editor and read the bounding box of the transparent
  window in pixels. That rectangle is x / y / w / h.

NOTES
  - Overlays are suppressed while HDMI is connected.
  - The image only moves if minui_viewport_w and _h are both > 0.
