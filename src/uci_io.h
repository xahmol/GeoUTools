// uci_io.h - GeoUTools: bank I/O in and out around Ultimate Command
// Interface access under GEOS (GEOS runs with I/O banked out).
//
// Between enableIO() and restoreIO() interrupts are off and the I/O area
// ($D000-$DFFF) is visible: call only UCI library functions and plain C
// there, never GEOS routines.

#ifndef UCI_IO_H
#define UCI_IO_H

__noinline void enableIO(void);
__noinline void restoreIO(void);

// Detect the Ultimate Command Interface; if it is off and the firmware is
// 3.15 or newer, switch it on with the unlock sequence first. Banks I/O
// in and out itself. Returns 1 if the UCI answers.
char uci_detect(void);

#pragma compile("uci_io.c")

#endif
