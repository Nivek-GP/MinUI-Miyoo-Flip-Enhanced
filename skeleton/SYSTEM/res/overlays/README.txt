Screen overlays  —  per-console bezel / border art
==================================================

Each subfolder is named after a MinUI console tag (GBA, GB, PS, SFC, ...). When
you run a game, minarch lists the .png+.cfg overlay pairs found in that console's
folder under Options > Frontend > Overlay. "None" is always first.

  <tag>/<name>.png   full-screen 640x480 RGBA art
  <tag>/<name>.cfg   RetroArch-style descriptor + optional minui_viewport_* rect

See the README.txt inside any subfolder for the .cfg format and how to work out
the viewport rectangle. _RetroArch_pack_notes.txt holds the original pack author's
per-system notes (including the Game Boy palette recommendations).

This folder ships to  SDCARD/.system/res/overlays/  and is only wired up on
devices whose platform supports overlay compositing (currently the Miyoo Flip).
