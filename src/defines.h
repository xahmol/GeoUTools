// defines.h - GeoUTools: shared definitions.

#ifndef DEFINES_H
#define DEFINES_H

// Version string from the Makefile (-dVERSION), e.g. "v2.0.0-20261024-1200"
#ifndef VERSION
#define VERSION "v2.0.0-dev"
#endif

// GEOS drive type table (driveType, $848E), one byte per drive A-D
#define DRIVETYPES          0x848e

#define MOUSE_BTN_DOWN      128

// Shared scratch text buffer (time_common.c / interface.c)
extern char buffer[81];

#endif
