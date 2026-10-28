---
name: geos-reference
description: GEOS 64/128 technical reference for writing GEOS applications in C (Oscar64/cc65) - memory and zero-page maps, application lifecycle, MainLoop/IRQ, menus/icons/dialogs, processes, text/graphics, file system, VLIR, file header, CVT, REU, GEOS 128 80-column, Wheels/MegaPatch3/GDOS64 differences, detection, gotchas
metadata:
  type: reference
---

# GEOS technical reference

Curated reference for writing GEOS applications (C64 and C128, GEOS 2.0,
Wheels, MegaPatch3, GDOS64) in C with Oscar64 or cc65. Built 2026-10-24 for
the GeoUTools Oscar64 port and the `geos-oscar64` library. The per-routine
API (address, parameters, returns, destroys for all ~200 jump-table
routines) is in the companion `GEOS_API_REFERENCE.md`
(`~/.claude/geos_api_reference.md`), generated from HG2G.

Sources, in order of authority for this document:

- **HG2G2022**: *The Hitchhiker's Guide to GEOS v2022* (Berkeley Softworks
  1988; digitised, merged with the Official GEOS Programmer's Reference
  Guide and extended with Wheels 4.4 by Paul B Murdaugh 2020-2022).
  https://github.com/geos64128/HG2G2022 (PDF, freely redistributable
  without fee, see its licence page). Text extraction:
  `mutool draw -F txt -o hg2g.txt HG2G-v2022.220411.pdf`. API entries are
  findable by searching `Name:` (with colon).
- **mist64/geos**: the GEOS 2.0 / GEOS 128 2.0 / Wheels kernal, reassembled
  as buildable ca65 source by Maciej Witkowiak and Michael Steil.
  https://github.com/mist64/geos. `inc/geossym.inc` (variables),
  `inc/const.inc` (constants), `inc/jumptab.inc` (jump table),
  `kernal/` (code), `drv/` (disk drivers).
- **area6510 (Markus Kanet)**: MegaPatch3 V3.3r12 and GDOS64 V1.03
  sources. https://bitbucket.org/mkgit64/area6510 `releases/geos-megapatch/src`
  and `releases/geos-gdos64/src`. The sources are MegaAssembler files in
  geoWrite VLIR format on D81 images; extract with
  `~/.claude/tools/geos/geosextract.py image.d81 outdir` (German comments,
  umlauts as `{ | }`).
- **cc65 GEOS library**: `libsrc/geos-common`, `libsrc/geos-cbm`,
  `include/geos/*.h` (zlib licence), https://github.com/cc65/cc65 and
  https://cc65.github.io/doc/geos.html. The C API naming convention
  (`Rectangle()`, `DoMenu(&menu)`, `struct fileheader`) comes from here.
- Lyonlabs/Cenbe GEOS pages, https://www.lyonlabs.org/commodore/onrequest/geos/
  (I/O banking idiom, programming tips).

Abbreviations: **r0..r15** GEOS pseudo-registers (zero page words),
**a0..a9** application pseudo-registers, **DA** desk accessory,
**HG2G 20-31** = HG2G page 31 of chapter 20.

---

## 1. Platforms and version detection

GEOS variants in use today, all with the same jump table at `$C100`:

| System | Machine | Notes |
|---|---|---|
| GEOS 64 2.0 (Berkeley) | C64 | Reference behaviour. 1541/1571/1581, REU up to 2 MB, RAM 1541/1571/1581 drives. |
| GEOS 128 2.0 | C128 | 40 and 80 columns, apps run in front RAM (bank 1), extra jump table entries. |
| Wheels 64 / 128 4.4 | C64 / C128 | Maurice Randall. Native-mode partitions, CMD support, extra jump table (HG2G ch. 21). `version` = `$44`. |
| MegaPatch3 64/128 V3.3 | C64 / C128 | Markus Kanet. Patches GEOS 2.0: CMD FD/HD/RL, SD2IEC, native RAM drives (REU, GeoRAM, SuperRAM, C=REU), TaskManager, spooler. "MP" at `MP3_CODE` (`$C014`). |
| GDOS64 V1.0x | C64 | Successor of MP3 64 (Markus Kanet, 2024+). bootName "GDOS64-V3". GeoDesk64 desktop. |
| Gateway | C64/C128 | Alternative desktop/kernal (not covered). |

Kernal information bytes (`$C000`-`$C02F`, preserved in all variants):

| Address | Name | Content |
|---|---|---|
| `$C000` | BootGEOS / SystemReBoot | `jmp` reboot GEOS |
| `$C003` | ResetHandle | internal vector |
| `$C006` | bootName | "GEOS BOOT" (GEOS/MP3), "GDOS64-V3" (GDOS64) |
| `$C00F` | version | `$20` = 2.0 (also MP3), `$44` = Wheels 4.4 |
| `$C010` | nationality | 0 = English, 1 = German |
| `$C011` | sysVersion | MP3/GDOS internal |
| `$C012` | sysFlgCopy | copy of sysRAMFlg |
| `$C013` | c128Flag | bit 7 set = GEOS 128 |
| `$C014` | MP3_CODE | "MP" when MegaPatch3 is installed |
| `$C018` | dateCopy | MP3: copy of the date |

C check (cc65's `get_ostype()` does the equivalent):

```c
#define c128Flag (*(volatile unsigned char *)0xC013)
#define geosVersion (*(volatile unsigned char *)0xC00F)
bool is128 = c128Flag & 0x80;
bool isMP3 = PEEK(0xC014) == 'M' && PEEK(0xC015) == 'P';
```

`graphMode` (`$3F`, GEOS 128 only): bit 7 set = 80 columns (`GR_80`).

---

## 2. Memory map (GEOS 64)

| Range | Use |
|---|---|
| `$0000`-`$0001` | 6510 port. GEOS runs with `$01` = `$30` (all RAM); I/O is banked in only inside GEOS routines and interrupts. |
| `$0002`-`$00FF` | zero page, see section 3 |
| `$0100`-`$01FF` | 6502 stack. `StartAppl` resets SP to `$FF`. |
| `$0200`-`$03FF` | C64 Kernal RAM (`APP_LVAR` `$0200`): free when the Kernal is not used, but disk drivers using Kernal IEC (MP3/GDOS CMD/SD2IEC/IEC-bus drivers) need `$0200`-`$03FF` parts; keep app use to buffers that are not live across disk calls. `$0314`-`$0319` IRQ/BRK/NMI vectors. |
| `$0400`-`$5FFF` | **application code and data** (`APP_RAM`), 23 KB |
| `$6000`-`$7F3F` | background screen buffer (`BACK_SCR_BASE`). Free for the application if it never sets `ST_WR_BACK` and does not use RecoverRectangle/menus/dialogs that restore from it (cc65: `__BACKBUFSIZE__`). |
| `$7900`-`$7F3F` | printer driver (`PRINTBASE`) when loaded (inside the back buffer) |
| `$7F40`-`$7FFF` | application RAM, 192 bytes |
| `$8000`-`$84FF` | `OS_VARS`: `diskBlkBuf` `$8000`, `fileHeader` `$8100`, `curDirHead` `$8200`, `fileTrScTab` `$8300`, `dirEntryBuf` `$8400`, GEOS variables `$841E`-`$8697` |
| `$8A00` | sprite pictures (`SPRITE_PICS`) |
| `$8C00` | colour matrix (`COLOR_MATRIX`) |
| `$9000`-`$9D7F` | disk driver (`DISK_BASE`, `$0D80` bytes) |
| `$A000`-`$BF3F` | foreground screen bitmap (`SCREEN_BASE`), VIC bank 2 |
| `$BF40`-`$FFFF` | GEOS kernal (jump table `$C100`), I/O at `$D000` when banked in, input driver at `$FE80` (`MOUSE_BASE`) |

AUTO_EXEC applications running during boot must not touch `$5000`-`$5FFF`:
the kernal boot code is still there (HG2G G: Special Notes).

GEOS 128: the application runs in front RAM (bank 1) `$0400`-`$5FFF` like on
the C64; the background buffer, desk-accessory save area and soft sprites
live in "backRAM" (bank 0). Input drivers sit at `$FD00` (`MOUSE_BASE_128`,
file type `INPUT_128`). Desk accessories are limited to 24 KB.

Important GEOS variables (`inc/geossym.inc`):

| Address | Name | Notes |
|---|---|---|
| `$8489` | curDrive | current drive 8-11. Never write directly: use `SetDevice`. |
| `$848D` | NUMDRV | number of drives |
| `$848E` | driveType[4] | per drive A-D; bit 7 = RAM drive, bit 6 = shadowed, low nibble DRV_1541=1/1571=2/1581=3, MP3/Wheels add native (4) |
| `$841E`/`$8430` | DrACurDkNm/DrBCurDkNm | current disk names |
| `$8442`/`$8453` | dataFileName/dataDiskName | passed by deskTop on open/print |
| `$849B` | appMain | vector called every MainLoop pass |
| `$849D`/`$849F` | intTopVector/intBotVector | IRQ hooks (default InterruptMain / NULL) |
| `$84A1` | mouseVector | called on mouse press |
| `$84A3` | keyVector | called on key press (key in `keyData` `$8504`) |
| `$84A5` | inputVector | input device change |
| `$84A7` | mouseFaultVec | pointer left the `mouseTop..mouseRight` box |
| `$84A9` | otherPressVec | press not on menu/icon |
| `$84AB` | StringFaultVec | text input fault |
| `$84AD` | alarmTmtVector | alarm |
| `$84AF` | BRKVector | BRK handler |
| `$84B1` | RecoverVector | menu background recover routine |
| `$8504`/`$8505` | keyData/mouseData | last key / button state (bit 7 = released) |
| `$8516`-`$851B` | year month day hour minutes seconds | system clock (year = 2 digits) |
| `$851F` | dlgBoxRamBuf | 417-byte system state save area of dialogs and DAs |

---

## 3. Zero page map, and what it means for C compilers

| Range | Owner |
|---|---|
| `$00`-`$01` | 6510 port |
| `$02`-`$21` | **r0-r15** pseudo-registers: parameters/results of GEOS routines; saved and restored by the GEOS IRQ handler |
| `$22`-`$3F` (`$48` on GEOS 128) | GEOS kernal variables: `curPattern $22`, `string $24`, `baselineOffset $26`, `curSetWidth $27`, `curHeight $29`, `curIndexTable $2A`, `cardDataPntr $2C`, `currentMode $2E`, `dispBufferOn $2F`, `mouseOn $30`, `msePicPtr $31`, `windowTop $33`, `windowBottom $34`, `leftMargin $35`, `rightMargin $37`, `pressFlag $39`, `mouseXPos $3A` (word), `mouseYPos $3C`, `returnAddress $3D`, `graphMode $3F` (128), internal vectors `$3F`-`$48` (IconDescVec, CallRLo/Hi, DBoxDesc) |
| `$49`-`$6F` | officially "GEOS and application", in practice unused by the kernal; cc65 puts its 32-byte runtime ZP at `$58`-`$77` |
| `$70`-`$7F` | **a2-a9**: application only |
| `$80`-`$FA` | C64 Kernal/BASIC area ("swappable Kernal I/O / application zpage space", `APP_ZIO`). GEOS itself uses: `$8B`-`$8F` disk driver work/TurboDOS flags (`d0`-`d2`, `TURBO_DD00`), `$90` STATUS, `$BA` curDevice. MP3/GDOS drivers that call Kernal IEC routines also use `$90`-`$95`, `$A3`-`$A5`, `$B7`-`$BC`; MP3 ToBasic and TaskMan use `$C6`, `$D3`, `$D6`. |
| `$FB`-`$FE` | **a0-a1**: application only |
| `$FF` | Kernal |

What the GEOS IRQ handler saves (mist64 `kernal/irq/irq.s`): A/X/Y,
`CallRLo`, `returnAddress` and **r0-r15**. It does not touch `$80`-`$FA`;
neither do InterruptMain, the mouse/keyboard scan or the standard input
drivers.

### C compiler implications

- **cc65** needs only 26+6 bytes of runtime ZP (`sp`, `sreg`, `regsave`,
  `ptr1-4`, `tmp1-4`, `regbank`) because locals live on a software stack in
  RAM. Its linker places them; the geos-cbm config uses `$58`-`$77`.
- **Oscar64** uses zero page as a register file: about 86-98 bytes at
  addresses fixed in the compiler per target (`oscar64/MachineTypes.cpp`,
  `Compiler.cpp`). Default C64 layout `$02`-`$63` collides with r0-r15 and
  the kernal variables. `-tm=atari` puts it at `$80`-`$D5`
  (WORK_Y `$80`, WORK `$81`, FPARAMS `$8B`-`$96`, IP `$97`, ACCU `$99`,
  ADDR `$9D`, STACK `$A1`, LOCALS `$A3`, TMP `$A5`, TMP_SAVED `$C5`-`$D4`)
  and the `zeropage` section at `$E0`-`$FF`. Combined with `-tf=bin` and a
  custom runtime (`-rt=`), nothing else Atari-specific applies (only the
  `__ATARI__` define and default regions, both overridden).
- `$80`-`$D5` overlaps the GEOS-owned `$8B`-`$90` and `$BA`, so a C runtime
  there must keep a **shadow copy of GEOS's `$80`-`$D5`** and exchange it:
  on every entry from GEOS into C (callback trampolines) and around every
  GEOS call that can reach a disk driver or the Kernal. The `geos-oscar64`
  library does this (I/O-class wrappers only by default,
  `-dGEOS_SWAP_ALL` for every call).
- Pointers passed to GEOS "zero page pointer" parameters (DBVARSTR,
  DBGETSTRING, `GetString`) must be r-registers or a0-a9, never a C
  register.

---

## 4. Application lifecycle

### Loading and start

1. The deskTop (or `GetFile`/`LdApplic`) loads the file at `O_GHST_ADDR`
   from its header (normally `$0400`).
2. `StartAppl` (mist64 `kernal/load/load1a.s`): `sei`, `cld`, **SP = `$FF`**,
   `InitMachine` (system state reset), `UseSystemFont`, then
   `CallRoutine(O_GHST_VEC)` from MainLoop entry (`_MNLP`): the
   initialisation routine is called with `jsr` and **must return with
   `rts`** to MainLoop.
3. GEOS 64/128 have cleared/initialised the screen; Apple GEOS did not
   (cc65 sets pattern 2 and `dispBufferOn = ST_WR_FORE|ST_WR_BACK` anyway).
4. Passed in: `r0L` = load options (`ST_LD_DATA $80` open data file,
   `ST_PR_DATA $40` print data file), `r2` -> `dataDiskName`, `r3` ->
   `dataFileName`.

Default state at start of a dialog box or auto-exec (HG2G 8): `currentMode`
plain text, `dispBufferOn` = fore|back, `mouseOn` = `%11100000`, arrow
pointer, window `0..199` x `0..319` (639 in 80 columns), `appMain` NULL,
`intTopVector` = InterruptMain, other vectors NULL.

### The event model

Applications never loop. After initialisation the application returns to
**MainLoop**, which repeatedly:

1. calls `appMain` if non-NULL (with Wheels: screensaver check first),
2. processes queued input: key presses (`keyVector`), mouse presses on
   menus, icons, or elsewhere (`otherPressVec`, `mouseVector`), input
   device changes (`inputVector`), mouse faults (`mouseFaultVec`),
3. runs expired processes (timers decremented by the IRQ) and sleeping
   routines (`Sleep`),
4. handles alarm, string input (`GetString`) and menu/dialog state.

Every application routine GEOS calls (menu item, icon handler, vector,
process event, dialog user routine) is entered by `jsr` from MainLoop and
must return with `rts` quickly. Long work must be split up (processes,
`appMain`) or the UI freezes. The C stack therefore only needs to survive
within one callback: at every callback entry the C runtime can start from
the same stack top.

Interrupt level (`IRQHandler`, every raster frame at line ~$FA): saves
r0-r15, scans keyboard and input driver (`ProcessMouse`), decrements
process timers and `Sleep` delays, double-click counter, alarm, random
number, then `intTopVector` and `intBotVector`. Code hooked into these
vectors runs with I/O banked in and must be IRQ-safe: a C function hooked
here would need the full C register block saved (not the shadow swap).

### Exiting

- `EnterDeskTop` (`$C22C`): reloads the deskTop, never returns. The
  normal way to quit.
- `ToBasic` (`$C241`): leaves GEOS to BASIC loading a program.
- `Panic` (`$C2C2`): system error box ("System error near $xxxx").
- A desk accessory ends with `RstrAppl` (`$C23E`), which restores the
  application, its screen and state from `dlgBoxRamBuf` and the swap
  file.

### File types that run

| Type | Value | Notes |
|---|---|---|
| APPLICATION | 6 | normal program, started by double click |
| AUTO_EXEC | 14 | run at boot by the kernal (before the deskTop), in directory order, each with `LdApplic`; also runnable by the user. Check `firstBoot` (`$88C5`, `FALSE` at boot): at boot do the boot job and exit with `EnterDeskTop` (the kernal patches `EnterDeskTop` during the boot loop to continue with the next auto-exec), otherwise act as a normal application. Only the joystick input driver is loaded at boot. No writes to `$5000`-`$5FFF` at boot. Using dialogs: back up `dlgBoxRamBuf` (417 bytes) around `DoDlgBox`. Full drive support only after Configure has run (place the file after CONFIGURE in the directory). |
| DESK_ACC | 5 | loaded on top of an application, which is saved to a swap file first; may only use the memory range in its header (`O_GHST_ADDR`..`O_GHEND_ADDR`); must not use the top 16 scanlines; must restore curDrive; cannot use dialogs unless it saves `dlgBoxRamBuf`. Ends with `RstrAppl`; can't abort from its init code directly: `appMain = RstrAppl`, then `rts`. |

Header flag `O_128_FLAGS` (header offset `$60`): bits 7/6 = `00` 40
columns only, `01` (`CF_40_80 $40`) 40 and 80, `10` not under GEOS 128,
`11` 80 columns only. GEOS 128's `LdApplic`/`LdDeskAcc` return
`INCOMPATIBLE` (14) if the flags don't allow the current `graphMode`.

---

## 5. Menus

`DoMenu` (`$C151`): `r0` = menu structure, `a` = item to centre the pointer
on. Only one menu tree is active; `DoMenu` replaces the previous one.

Menu structure (`OFF_MY_TOP` etc. in `const.inc`):

```
.byte top, bottom          ; y, pixels
.word left, right          ; x (| DOUBLE_W for 80 columns)
.byte type | count         ; HORIZONTAL ($00) or VERTICAL ($80) | CONSTRAINED ($40), item count (max 31)
; per item:
.word textPtr              ; null-terminated item text
.byte itemType             ; MENU_ACTION $00, DYN_SUB_MENU $40, SUB_MENU $80
.word pointer              ; routine (MENU_ACTION/DYN_SUB_MENU) or submenu structure
```

- `MENU_ACTION` routine: called with `a` = item number; it must call one of
  `ReDoMenu` (`$C193`, redraw the current level, stay in it),
  `GotoFirstMenu` (`$C1BD`, close all submenus, back to the main menu) or
  `DoPreviousMenu` (`$C190`) before returning, otherwise the menu stays
  "open". Typical: `GotoFirstMenu(); doTheThing(); return;`.
- `DYN_SUB_MENU` routine: called before the submenu opens, returns the
  submenu pointer in `r0` (used to change item texts, e.g. check marks with
  a leading `*`).
- `RecoverAllMenus` / `RecoverMenu` restore the screen under menus from
  the background buffer (or via `RecoverVector`).
- Menu text that needs to fit 80-column BSW 128 font: make `right` depend on
  `graphMode`.
- C API pattern (cc65 `struct menu`/`struct menuitem`): the routine pointer
  in an item is a function GEOS calls; with Oscar64 it must point to a
  callback trampoline, not directly to the C function.

---

## 6. Icons

`DoIcons` (`$C15A`): `r0` = icon table. **Every application must install at
least one icon**, even a dummy (1 scanline high, 1 byte wide, picture
pointer NULL), otherwise GEOS may misbehave (HG2G 13). Max 31 icons
(`MAX_ICONS`).

```
.byte count                 ; OFF_NM_ICNS
.word mouseX                ; OFF_IC_XMOUSE: pointer position after DoIcons
.byte mouseY                ; OFF_IC_YMOUSE
; per icon (8 bytes):
.word picture               ; OFF_PIC_ICON: compacted bitmap, NULL = disabled (not drawn, not clickable)
.byte x                     ; OFF_X_ICON_POS: in cards (8 pixels) (| DOUBLE_B for 80 columns)
.byte y                     ; OFF_Y_ICON_POS: pixels
.byte width                 ; OFF_WDTH_ICON: bytes (| DOUBLE_B)
.byte height                ; OFF_HEIGHT_ICON: pixels
.word service               ; OFF_SRV_RT_ICON: routine called on click, r0L = icon number
```

- `iconSelFlg` (`$84B5`): `ST_FLASH $80` flash the icon on selection,
  `ST_INVERT $40` leave it inverted.
- Double click: a second click while `dblClickCount` (`$8515`) is non-zero
  calls the routine again with `r0H` = TRUE.
- Icon pictures are in GEOS compacted bitmap format (section 9).

---

## 7. Dialog boxes

`DoDlgBox` (`$C256`): `r0` = dialog definition. Saves the system state to
`dlgBoxRamBuf`, runs its own MainLoop until an exit icon or
`RstrFrmDialog` ends it, restores the state, returns with `r0L` = the
command that ended it (OK=1, CANCEL=2, YES=3, NO=4, OPEN=5, DISK=6, or a
user value).

```
.byte DEF_DB_POS | pattern  ; $80 | shadow pattern: default position (32..127 x 64..255)
                            ; or SET_DB_POS ($00) | pattern followed by:
.byte top, bottom           ; .word left, right
; commands:
.byte OK, x_card, y         ; system icons OK..DISK: x in cards, y in pixels (DBI_X_0/1/2, DBI_Y_0/1/2)
.byte DBTXTSTR, x, y        ; .word text  - PutString at offset (baseline)
.byte DBVARSTR, x, y, zp    ; text pointer in zero-page register zp (r5-r10, r15, a0-a9)
.byte DBGETSTRING, x, y, zp, maxChars   ; input into buffer at (zp)
.byte DBSYSOPV              ; return on any press outside icons
.byte DBGRPHSTR             ; .word graphicsString  (must be the last command)
.byte DBGETFILES, x, y      ; file selector: r7L = GEOS type, r5 = 17-byte name buffer, r10 = class or NULL
.byte DBOPVEC               ; .word routine -> otherPressVec
.byte DBUSRICON, x, y       ; .word icon (pic, NULL, width, height, routine)
.byte DB_USR_ROUT           ; .word routine: called after the box is drawn, before the icons
.byte NULL                  ; end
```

Text constants: `TXT_LN_1_Y`..`TXT_LN_5_Y` = 16..80, `TXT_LN_X` = 16.
`BOLDON` (24), `PLAINTEXT` (27) etc. can be embedded in strings.

Library dialogs built on DoDlgBox (cc65 names): `DlgBoxOk`, `DlgBoxYesNo`,
`DlgBoxOkCancel`, `DlgBoxGetString`, `DlgBoxFileSelect`, `MessageBox`.

Notes: a dialog inside a dialog or a DA overwrites `dlgBoxRamBuf`;
`DBGETFILES` reads the disk (so for an Oscar64 runtime it is an I/O-class
call); `RstrFrmDialog` (`$C2BF`) ends a dialog from a user routine with
`sysDBData` (`$851D`) as result.

---

## 8. Processes, sleep and time

`InitProcesses` (`$C103`): `a` = count (max `MAX_PROCESSES` = 20), `r0` =
table of 4-byte entries `.word routine, .word timerReload` (timer in
frames, 60 Hz NTSC / 50 Hz PAL). All start frozen;
`RestartProcess(n)` starts one (reloads the timer).
`EnableProcess` forces it runnable now, `BlockProcess`/`UnblockProcess`
stop/allow the event call (timer keeps running), `FreezeProcess`/
`UnfreezeProcess` stop/continue the timer. Event routines run from
MainLoop, not from the IRQ.

`Sleep` (`$C199`): `r0` = frames; the code after the `jsr Sleep` is
resumed by MainLoop after the delay, the caller's return goes back to
MainLoop immediately. Hard to use from C (it captures the return address):
avoid, use a process instead.

System clock: `year`..`seconds` at `$8516`-`$851B`, updated by the kernal
from CIA1 TOD. Setting the GEOS time = write these bytes and the CIA TOD
(GeoUTime does both).

---

## 9. Graphics

Coordinates: x 0-319 (word), y 0-199 (byte). On GEOS 128 in 80 columns
x is 0-639: OR x and widths with `DOUBLE_W` ($8000) to have GEOS double
them in 80 columns (ignored in 40), add `ADD1_W` ($2000) to set bit 0 after
doubling (e.g. right edge `319|DOUBLE_W|ADD1_W`). Card x positions and byte
widths (icons, bitmaps) use `DOUBLE_B` ($80) / `ADD1_B` ($20).

`dispBufferOn` (`$2F`): `ST_WR_FORE $80` draw on screen, `ST_WR_BACK $40`
draw into the background buffer at `$6000`, both = normal. Both zero is an
undefined state. Applications that use the background buffer memory for
themselves must keep `ST_WR_BACK` clear and can't rely on
RecoverRectangle/menu recovery.

Main routines (registers: `r2L` top y, `r2H` bottom y, `r3` left x, `r4`
right x; `r11L` y for lines):

| Routine | Address | Does |
|---|---|---|
| SetPattern | `$C139` | `a` = pattern 0-33 (0 white, 1 black, 2 50% grey) |
| Rectangle / i_Rectangle | `$C124` / `$C19F` | fill with current pattern |
| FrameRectangle | `$C127` | `a` = bit pattern of the frame |
| InvertRectangle | `$C12A` | XOR |
| RecoverRectangle | `$C12D` | copy from background buffer |
| ImprintRectangle | `$C250` | copy screen to background buffer |
| HorizontalLine / VerticalLine | `$C118` / `$C121` | `a` = pattern byte |
| DrawLine / DrawPoint | `$C130` / `$C133` | carry/negative flags select draw/erase/recover |
| GraphicsString / i_GraphicsString | `$C136` / `$C1A8` | interpreted commands: MOVEPENTO 1, LINETO 2, RECTANGLETO 3, NEWPATTERN 5, ESC_PUTSTRING 6, FRAME_RECTO 7, PEN_X/Y/XY_DELTA 8-10, NULL end |
| BitmapUp / i_BitmapUp | `$C142` / `$C1AB` | draw compacted bitmap: `r0` data, `r1L` x card, `r1H` y, `r2L` width bytes, `r2H` height |
| BitmapClip, BitOtherClip | `$C2AA`, `$C2C5` | clipped / streamed bitmap |

Inline (`i_`) variants take their parameters from the bytes following the
`jsr`; unusable from C directly, the library uses the register versions.

**Compacted bitmap format** (icons, BitmapUp): sequence of records, count
byte first: `0-127` = repeat next byte n times; `128-220` = n-128 literal
bytes follow; `221-255` = "BIGCOUNT" pattern repeat: n-220 bytes of a
pattern follow, then a count byte, repeated. File icons are 3 bytes x 21
lines = 63 bytes: header byte `$BF` (128+63) then 63 literal bytes (the
same layout as a C64 sprite without the last byte).

Colour: GEOS 64 is hires monochrome with a colour matrix at `$8C00`
(`screencolors` `$851E` = default fore/background). MP3/GDOS/Wheels add
colour routines (`ColorRectangle`/`i_ColorRectangle` in GEOS 128 2.0 /
Wheels, `ColorCard`, MP3 colour schemes).

---

## 10. Text, fonts, keyboard

- `PutString` (`$C148`): `r0` = string, `r11` = x, `r1H` = y (baseline).
  Control codes inside strings: BACKSPACE 8, TAB 9, LF 10, HOME 11,
  UPLINE 12, CR 13, ULINEON/OFF 14/15, REV_ON/OFF 18/19, GOTOX 20 (+word),
  GOTOY 21 (+byte), GOTOXY 22 (+word, byte), NEWCARDSET 23, BOLDON 24,
  ITALICON 25, OUTLINEON 26, PLAINTEXT 27. ESC_GRAPHICS 16 (+graphics
  string) and ESC_RULER 17 are geoWrite-only.
- `PutChar` (`$C145`), `PutDecimal` (`$C184`, `a` = format
  `SET_LEFTJUST $80`/`SET_SURPRESS $40`, `r0` value), `GetCharWidth`
  (`$C1C9`), `SmallPutChar`.
- Text clipping: `windowTop/Bottom`, `leftMargin/rightMargin`; leaving
  them calls `StringFaultVec`.
- Fonts: `UseSystemFont` (`$C14B`, BSW 9 point), `LoadCharSet` (`$C1CC`,
  `r0` = font data), font files are VLIR, one record per point size.
  `currentMode` (`$2E`) bits: SET_UNDERLINE $80, SET_BOLD $40,
  SET_REVERSE $20, SET_ITALIC $10, SET_OUTLINE $08, SET_SUPERSCRIPT $04,
  SET_SUBSCRIPT $02.
- Character set is ASCII (not PETSCII): strings in C sources can stay
  plain ASCII. **Oscar64: do not include `petscii.h`** and do not pass
  `-psci`; check `#pragma charmap` is identity.
- Keyboard: `keyVector` is called per key press with the key in
  `keyData`; codes are ASCII plus KEY_F1..F8 (1-6, 14, 15), KEY_UP 16,
  KEY_DOWN 17, KEY_HOME 18, KEY_CLEAR 19, KEY_LARROW 20, KEY_UPARROW 21,
  KEY_STOP 22, KEY_RUN 23, KEY_LEFT = BACKSPACE 8, KEY_RIGHT 30,
  KEY_INSERT 28, KEY_DELETE 29 (C128 adds HELP 25, ALT 26, ESC 27, TAB,
  LF, ENTER). Commodore-key + letter = shortcut (`SHORTCUT` $80 | key) for
  menu items whose text contains it.
- `GetString` (`$C1BA`): `r0` buffer, `r1L` flags, `r2L` max length,
  `r11`/`r1H` position, `keyVector` = completion routine; runs
  asynchronously from MainLoop.

---

## 11. Mouse and input drivers

- `mouseXPos` (`$3A`, word), `mouseYPos` (`$3C`), `mouseData` (`$8505`,
  bit 7 = button up), `pressFlag` (`$39`: KEYPRESS_BIT 7, INPUT_BIT 6,
  MOUSE_BIT 5).
- `mouseOn` (`$30`): MOUSEON_BIT 7, MENUON_BIT 6, ICONSON_BIT 5.
- `StartMouseMode` (`$C14E`), `ClearMouseMode` (`$C19C`), `MouseOff`/
  `MouseUp` (`$C18D`/`$C18A`), `IsMseInRegion` (`$C2B3`), mouse limits
  `mouseTop/Bottom/Left/Right` (`$84B8`...).
- Input drivers are separate files (type INPUT_DEVICE 10 / INPUT_128 15)
  loaded at `$FE80` (64) / `$FD00` (128): joystick, 1351, SmartMouse,
  NEOS, KoalaPad, lightpen. Entry points: InitMouse, SlowMouse,
  UpdateMouse (called from the IRQ). The driver adds movement to
  `mouseXPos/YPos`: test tools can therefore position the pointer by
  writing these variables directly, and press the button through the
  joystick fire line with the joystick driver.

---

## 12. File system

### Disk calls and the current drive

`SetDevice(dev)` (`$C2B0`) selects drive 8-11 (sets `curDevice`,
`curDrive`, `curType`), `OpenDisk` (`$C2A1`) reads the BAM/dir header and
disk name, `NewDisk` (`$C1E1`) re-initialises the drive after a disk
change. Always use the drive driver routines (`GetDirHead`, `PutDirHead`,
`Get1stDirEntry`, `GetNxtDirEntry`): never assume the directory is at
18/1 (1581: 40/3, native: partition-specific).

High level:

| Routine | Address | In | Out |
|---|---|---|---|
| FindFile | `$C20B` | `r6` name | `x` error, `dirEntryBuf` |
| GetFile | `$C208` | `r6` name, `r0L` options, `r7` load address | runs apps/DAs, loads data files |
| SaveFile | `$C1ED` | `x`=0 / `r10L` dir page, `r9` header | `x` error |
| DeleteFile | `$C238` | `r0` name | `x` error |
| RenameFile | `$C259` | `r6` old, `r0` new | `x` error |
| GetFHdrInfo | `$C229` | `r9` dir entry | `fileHeader` loaded |
| FindFTypes | `$C23B` | `r6` buffer, `r7L` type, `r7H` max, `r10` class | names |
| ReadFile / WriteFile | `$C1FF` / `$C1F9` | track/sector chain | `x` error |
| GetBlock / PutBlock | `$C1E4` / `$C1E7` | `r1` track/sector, `r4` buffer | `x` error |
| CalcBlksFree | `$C1DB` | `r5` dir header | `r4` free blocks |

Error codes (`x` after the call, 0 = OK): NO_BLOCKS 1, INV_TRACK 2,
INSUFF_SPACE 3, FULL_DIRECTORY 4, FILE_NOT_FOUND 5, BAD_BAM 6,
UNOPENED_VLIR 7, INV_RECORD 8, OUT_OF_RECORDS 9, STRUCT_MISMAT 10,
BFR_OVERFLOW 11, CANCEL_ERR 12, DEV_NOT_FOUND 13, INCOMPATIBLE 14 (128);
drive errors $20-$2E (HDR_NOT_THERE, NO_SYNC, DBLK_NOT_THERE,
DAT_CHKSUM_ERR, WR_VER_ERR, WR_PR_ON, HDR_CHKSUM_ERR, DSK_ID_MISMAT,
BYTE_DEC_ERR), DOS_MISMATCH $73.

### Directory entry (30 bytes; `dirEntryBuf`, `OFF_*`)

| Offset | Content |
|---|---|
| 0 | CBM file type (bit 7 closed, bit 6 locked): DEL 0, SEQ 1, PRG 2, USR 3 |
| 1-2 | first data block T/S (VLIR: index block) |
| 3-18 | file name, padded with `$A0` |
| 19-20 | GEOS file header block T/S (`OFF_GHDR_PTR`) |
| 21 | structure: SEQUENTIAL 0, VLIR 1 (`OFF_GSTRUC_TYPE`) |
| 22 | GEOS file type (`OFF_GFILE_TYPE`): NOT_GEOS 0, BASIC 1, ASSEMBLY 2, DATA 3, SYSTEM 4, DESK_ACC 5, APPLICATION 6, APPL_DATA 7, FONT 8, PRINTER 9, INPUT_DEVICE 10, DISK_DEVICE 11, SYSTEM_BOOT 12, TEMPORARY 13, AUTO_EXEC 14, INPUT_128 15 |
| 23-27 | date: year, month, day, hour, minute |
| 28-29 | size in blocks |

On disk each entry is preceded by the 2 link bytes of its 32-byte slot.

### File header block (`fileHeader` `$8100`, `O_*` offsets)

| Offset | Constant | Content |
|---|---|---|
| `$00` | | `$00 $FF` (single-block chain) |
| `$02` | O_GHIC_WIDTH | 3 |
| `$03` | O_GHIC_HEIGHT | 21 |
| `$04` | O_GHIC_PIC | `$BF` + 63 bytes icon |
| `$44` | O_GHCMDR_TYPE | CBM type (e.g. `$83` USR closed) |
| `$45` | O_GHGEOS_TYPE | GEOS type |
| `$46` | O_GHSTR_TYPE | SEQUENTIAL / VLIR |
| `$47` | O_GHST_ADDR | load address |
| `$49` | O_GHEND_ADDR | end address (DA), else start-1 |
| `$4B` | O_GHST_VEC | init routine (executables) |
| `$4D` | O_GHFNAME | permanent name / class: 12 chars padded with spaces + "V1.0" + 3 NULs |
| `$60` | O_128_FLAGS | GEOS 128 40/80 flags |
| `$61` | O_GH_AUTHOR | author, 20 bytes, null-terminated (applications) |
| `$75` | O_GHP_FNAME | parent application permanent name (data files) |
| `$89` | O_GHAPDAT | **23 bytes for application use** |
| `$A0` | O_GHINFO_TXT | info text (shown by deskTop Info), null-terminated, to `$FF` |

Applications can keep settings in their own header: `GetFHdrInfo` (or
`LoadFileHeader`-style helper: FindFile + GetBlock of `OFF_GHDR_PTR`),
change `O_GHAPDAT`.., write it back with `PutBlock`. GeoUMount does this
(`fileHeader.author[40..45]` = header `$89`-`$8E`).

### VLIR files

Index block = first block (dir entry T/S): 127 record pointers (T/S pairs)
after the link bytes; `00 FF` = empty record, `00 00` = end of records.
API: `OpenRecordFile` (`$C274`), `CloseRecordFile` (`$C277`),
`NextRecord`, `PreviousRecord`, `PointRecord` (`$C280`, `a` = record),
`ReadRecord` (`$C28C`, `r7` buffer, `r2` max size), `WriteRecord`
(`$C28F`), `AppendRecord`, `InsertRecord`, `DeleteRecord`,
`UpdateRecordFile`. Only one VLIR file open at a time. VLIR applications
load record 0 as the resident part and overlay modules with ReadRecord.

geoWrite documents: VLIR, records 0-60 = pages, 61-63 = header,
footer and reserved, 64-126 = pictures (photo scraps referenced by
ESC_GRAPHICS); page text uses ESC_RULER (17 + 26 bytes), NEWCARDSET (23 + font
id word + style byte), ESC_GRAPHICS (16 + 4 bytes: width, height, record),
CR 13, PAGE_BREAK 12.

### CVT (Convert) file format

The interchange format for GEOS files on non-GEOS media (`c1541
-geoswrite`/`-geosread`, Convert 2.5, GEOS archives). Blocks of 254 bytes
(sector payload without link bytes):

1. **Signature block**: the 30-byte directory entry (as above; T/S fields
   meaningless), then the ASCII signature, zero padded to 254 bytes:
   `"SEQ formatted GEOS file V1.0"` for sequential files and
   `"PRG formatted GEOS file V1.0"` for VLIR files (as VICE c1541
   `-geosread` writes them; verified 2026-10-24).
2. **File header block**: 254 bytes (header from offset 2).
3. Sequential file: the data, sector by sector.
   VLIR file: a 254-byte **record table** replacing the index block: per
   record 2 bytes = number of blocks and the index of the last used byte
   in the record's last block (as in a sector link: data bytes = index - 1);
   `00 FF` = empty record, `00 00` = no more records. Then every record's
   data in order, each padded to whole 254-byte blocks, **except the last
   block of the file**, which is not padded.

Verified 2026-10-24 on `gEOutOOLS dOCuk.cvt` (geoWrite VLIR, 20 records:
137 blocks, file length = 3*254 + 136*254 + 63-1). Sequential files: the
data follows the header block directly, last block unpadded.

Format as implemented by Convert 2.5 and VICE c1541; verify any writer by
round-tripping through `c1541 -geoswrite` and `-geosread`.

---

## 13. REU and RAM routines

`ramExpSize` (`$88C3`, `sysRAMFlg` `$88C4`): number of 64 KB banks. `StashRAM` (`$C2C8`),
`FetchRAM` (`$C2CB`), `SwapRAM` (`$C2CE`), `VerifyRAM` (`$C2D1`),
`DoRAMOp` (`$C2D4`): `r0` C64 address, `r1` REU address, `r2` length,
`r3L` bank, `y` command. Bank 0 is reserved for the kernal (RAM reboot,
MoveData via REU); RAM drives occupy further banks (MP3/GDOS keep a bank
allocation table: allocate with their routines, never assume free banks).
MegaPatch3/GDOS64 can redirect these routines to GeoRAM, SuperRAM,
RAMLink or C=REU: on those systems "REU" calls are driver calls.

`MoveData` (`$C17E`, `r0` source, `r1` dest, `r2` length) may use the REU
(DMA) when the user enabled that in Configure. `FillRam` (`$C17B`),
`ClearRam` (`$C178`), `CopyString` (`$C265`), `CopyFString` (`$C268`),
`CmpString`/`CmpFString` (`$C26B`/`$C26E`).

---

## 14. GEOS 128 and 80 columns

- Same jump table plus 128-only routines: `SetNewMode` (`$C2DD`, apply
  `graphMode`), `TempHideMouse`, `SetMsePic`, `HideOnlyMouse`,
  `NormalizeX`, `MoveBData`/`SwapBData`/`VerifyBData`/`DoBOp` (bank 0/1
  copies), `AccessCache`, `ColorRectangle`, `ColorCard`...
- 40/80 switching: menu item toggles bit 7 of `graphMode`, calls
  `SetNewMode`, redraws. All coordinates written with
  `DOUBLE_W`/`DOUBLE_B` work in both modes unchanged.
- Fonts are wider in 80 columns (BSW 128): size menus and text areas with
  `graphMode`.
- Apps that call C64 Kernal/BASIC ROM routines are not 128-compatible.
- C128 GEOS 1.3 1571 driver bug: call `EnterTurbo` before `PutDirHead`
  (fixed in 2.0).

---

## 15. Wheels, MegaPatch3 and GDOS64 differences that matter to applications

- **Wheels**: version `$44`; extra jump table (HG2G ch. 21: partitions,
  `OpenPartition`, `GetPartInfo`, directory paths, `Wheels`-specific RAM
  allocation); the screensaver runs from MainLoop; `EnterDeskTop` loads
  Dashboard/Toolbox.
- **MegaPatch3** (`MP` at `$C014`): native-mode and CMD drivers (Kernal IEC
  calls inside disk routines), TaskManager (hot-key task switch: saves
  the whole application memory to RAM), print spooler, REU bank manager,
  colour schemes, MoveData/RAM ops via GeoRAM/SuperRAM/SCPU.
- **GDOS64**: MP3-derived, C64 only; bootName "GDOS64-V3"; GeoDesk64 as
  desktop; adds SD2IEC/IEC-bus and WiC64 support.
- All three keep the GEOS 2.0 jump table and variable layout: a program
  written against GEOS 2.0 and the rules in this document runs on them.
  The practical differences for applications are zero-page and Kernal
  usage inside disk calls (section 3) and that drivers may stay in turbo
  mode between calls (do not touch the serial bus behind GEOS's back; use
  `InitForIO`/`DoneWithIO` (`$C25C`/`$C25F`) around own I/O).

---

## 16. Doing hardware I/O from an application

GEOS runs with I/O banked out. To access VIC/SID/CIA/cartridge registers
(e.g. the Ultimate Command Interface at `$DF1C`-`$DF1F`):

```asm
php / sei            ; save I flag, block interrupts
lda $01 / save       ; save memory configuration
and #$F8 / ora #$05  ; RAM + I/O ($35)
sta $01
...                  ; access I/O
lda saved / sta $01  ; restore
plp
```

(Cenbe, lyonlabs.org GEOS programming tips.) Alternatively `InitForIO`
(`$C25C`) / `DoneWithIO` (`$C25F`): bank in I/O and the Kernal, disable
sprites/IRQs as for disk I/O, slower but driver-safe. Never call GEOS
routines between the two: they expect their own memory configuration.

---

## 17. Gotchas checklist

- Return to MainLoop with `rts`; never busy-wait for input.
- Install at least one icon (`DoIcons`) even if unused.
- Menu action routines must call `GotoFirstMenu`/`ReDoMenu`/
  `DoPreviousMenu`.
- Don't write `curDrive`/`curDevice`: `SetDevice`.
- Don't hard-code directory track/sector; use the driver routines.
- AUTO_EXEC at boot: no `$5000`-`$5FFF`, joystick only, back up
  `dlgBoxRamBuf` around dialogs, end with `EnterDeskTop`. `curPattern`
  is not valid (call `SetPattern` first) and the background buffer holds
  uninitialised memory: with `ST_WR_BACK` set, lines and rectangles merge
  their partial edge bytes through the background buffer
  (`LineCommon: and (r6),y`) and show garbage in the edge cards. Draw to
  the foreground only, or clear the screen with card-aligned rectangles
  first. (Found 2026-10-24 with the geos-oscar64 hello test.)
- Desk accessories: header end address bounds memory; restore curDrive;
  no top 16 lines; `RstrAppl` to exit.
- `dispBufferOn` must never be 0 in bits 6-7.
- Strings are ASCII, not PETSCII.
- Zero page: r0-r15 are clobbered by almost every GEOS call (see the
  "Destroys" lines in the API reference); C code must not keep values in
  them across calls. Compiler runtimes must stay out of `$02`-`$48` and
  protect GEOS's `$8B`-`$90`/`$BA` (section 3).
- GEOS 128: OR x/width with `DOUBLE_W`/`DOUBLE_B`; set `O_128_FLAGS`.
- The background buffer `$6000`-`$7F3F` is only free memory if the app
  never draws to it and never relies on recovery from it.

---

## 18. Tools

- **VICE c1541**: `-geoswrite file.cvt` writes a CVT as a GEOS file into a
  D64/D71/D81 (converting the disk to GEOS format if needed);
  `-geosread name file.cvt` exports one.
- **GEOSBuild** (Daniel England, https://github.com/M3wP/GEOSBuild,
  Lazarus/Pascal): builds GEOS files on images from header + binary
  (used by GeoUTools 1.x).
- **cc65** `grc65`: GEOS resource compiler (header, menus, icons from a
  `.grc` file).
- `~/.claude/tools/geos/geosextract.py`: geoWrite VLIR documents from
  D64/D71/D81 to text (for MP3/GDOS64 sources).
- `~/.claude/tools/geos/hg2gapi.py`: regenerates `GEOS_API_REFERENCE.md`
  from the HG2G text.
