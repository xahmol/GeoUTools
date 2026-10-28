# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

GeoUTools: three GEOS applications (C64 and C128, GEOS 2.0 / MegaPatch / Wheels) that use the
Ultimate II+ / Ultimate 64 **Ultimate Command Interface (UCI)**:

- **GeoUMount** (`src/mount.c`): file browser to mount/swap disk images on the Ultimate's
  emulated drives, including GEOS RAM drives in REU (firmware 3.10f+).
- **GeoUTime** (`src/time_synch.c`): auto-exec app that sets GEOS time (and the Ultimate RTC)
  from an NTP server.
- **GeoUConfig** (`src/config.c`): settings editor for both of the above.

Toolchain is **CC65** (`-t geos-cbm`), not Oscar64. The Oscar64 `ultimate-uci-oscar64`
submodule convention does not apply; the UCI library here is a local CC65 copy of Scott
Hutter's `ultimateii-dos-lib` (`src/ultimate_*_lib.c`, `include/ultimate_*_lib.h`).

## Build

```
make            # builds the 3 .bin files, GeoUTools.d64, GeoUTools.d81 and a timestamped release ZIP
make clean      # removes *.bin and *.map (not the disk images or ZIPs)
make deploy     # wput GeoUTools.d81 to the Ultimate's FTP server (ULTHOST, hard-coded in the Makefile)
```

Requirements (from the Makefile): `cl65` in PATH (or `CC65_HOME`), GEOSBuild at
`/home/xahmol/geostools/GEOSBuild` (`GBUILD`), VICE `c1541` in PATH, `zip`, `wput`.

Notes:
- Every `make` produces a new `GeoUTools-v11-<date-time>.zip` in the repo root, and these are
  committed to git as release history (the README links to a specific ZIP). Don't create one
  unintentionally; untracked extra ZIPs from test builds are usually just noise.
- The ZIP version prefix (`v11`) in the Makefile and `VERSION_MAJOR`/`VERSION_MINOR` in
  `include/defines.h` must be bumped together. The build date/time in the version string comes
  from `__DATE__`/`__TIME__`.
- There are no tests; verification is on real hardware or emulation with an Ultimate
  (the `c64bridge` MCP server can drive an Ultimate/VICE if available).

## Build pipeline per application

Each app (`geoumount`, `geoutime`, `geouconfig`) has the same set of files in the root:

- `<app>.grc`: GEOS resource file (application header: name, author, info text, icon).
- `src/<app>Hdr.s`: hand-written GEOS file header (segment `FILEHEADER`), linked to `<app>Hdr.bin`.
- `<app>_cc65config.cfg`: custom ld65 config (based on CC65's geos-cbm config) that emits
  `<app>.bin` (VLIR0, sequential app at `$0400`) and `<app>Hdr.bin` separately.
- `<app>d64.gbuild` / `<app>d81.gbuild`: GEOSBuild scripts combining header + bin into a GEOS
  sequential file on the disk image.

After GEOSBuild, `c1541 -geoswrite` adds the geoWrite manuals (`gEOutOOLS dOCuk.cvt`,
`gEOutOOLS dOKde.cvt`). Icons are 64-byte sprite-format `*.bin` files (sources in `assets/`,
made with SpritePad; toolbar icons are `.gbm` files from GEOS Image Editor).

## Source architecture

Source files are shared between apps by listing them in several `*SRC` variables in the
Makefile, not via a library. When adding a file, add it to every app that uses it:

| Module | Used by | Role |
|---|---|---|
| `interface.c` | Mount, Config | Window/dialog helpers, `get_ostype()`-based screen init (`ReinitScreen`), 40/80-column handling |
| `mount_common.c` | Mount, Config | Drive target detection via UCI (`SetValidDrives`), mount config |
| `time_common.c` | Time, Config | NTP/time config file load/save |
| `uultimate_geosassembly.s` | all | `enableIO()` / `restoreIO()` |
| `ultimate_common_lib.c` + `_dos_` / `_time_` / `_network_lib.c` | per app | UCI command library |

Key constraints:

- **I/O banking**: GEOS runs with I/O banked out. Every UCI access (registers at `$DF1C-$DF1F`)
  must be wrapped in `enableIO(); ... restoreIO();`, which also disables interrupts. Do not
  call GEOS routines (dialogs, `EnterDeskTop`, disk I/O) between the two: call `restoreIO()`
  first, as `mount_common.c:SetValidDrives` does.
- **C128 / 80 columns**: code checks `osType` (`GEOS64`/`GEOS128`) and `graphMode & 0x80`, and
  widens X coordinates with `DOUBLE_W | ADD1_W` on GEOS128. New drawing code must do the same.
- **Configuration storage** differs per app:
  - GeoUTime settings live in the data file `GeoUTimeDat` (95-byte buffer: host string at 0,
    UTC offset as decimal text at 80, ASCII-digit flags at 92 (verbose), 93 (NTP on),
    94 (`configfileversion`)). Written by GeoUTime/GeoUConfig, created with defaults if absent.
  - GeoUMount settings live in GeoUMount's **own GEOS file header**, bytes 40-45 of the
    `author` field (`ReadConfigdata()`); GeoUConfig rewrites that header.
- **Firmware fallbacks**: `Checkcommandsupport()` detects UCI commands missing on older firmware
  (`firmwareflag`); without device info, drive targets come from the manual override in the
  config. Keep these paths working (see README "Firmware and setting prerequisites").
- Memory is tight (app at `$0400` up to the stack below `$6000` back buffer); the directory
  browser is capped at roughly 250 entries for that reason.

## Documentation

User manuals are `README.md` (English) and `README_de.md` (German), with PDF exports
(`README.pdf`, `README_de.pdf`, packed into the release ZIP) and geoWrite `.cvt` versions on
the disk images. A feature change needs all of them updated, plus the version history in both
READMEs.

Code borrowed from others is credited in the header comment of each source file (Scott Hutter's
UCI lib, ntp2ultimate by MaxPlap, Cenbe's I/O banking code, Heurekus' GEOS examples); keep
that up to date.
