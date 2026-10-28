// uci_io.c - GeoUTools: I/O banking for UCI access under GEOS.
//
// Based on Cenbe's GEOS I/O banking routine
// (https://www.lyonlabs.org/commodore/onrequest/geos/geos-prog-tips.html),
// as used in GeoUTools 1.x (src/uultimate_geosassembly.s).
// Adapted: Oscar64 C functions with inline assembler bodies (callable from
// C and asm), state in memory instead of in the code.

#include "uci_io.h"
#include <ultimate_common_lib.h>

static char io_save_config, io_save_flags;

// Save the I flag and memory configuration, disable interrupts and bank in
// I/O ($01 = %xxxxx101).
__noinline void enableIO(void)
{
	__asm volatile {
		php
		sei
		pla
		sta	io_save_flags
		lda	$01
		sta	io_save_config
		and	#$f8
		ora	#$05
		sta	$01
	}
}

// Restore the memory configuration and the I flag.
__noinline void restoreIO(void)
{
	__asm volatile {
		lda	io_save_config
		sta	$01
		lda	io_save_flags
		pha
		plp
	}
}

// Not uii_wait_for_uci(): that times its wait with CIA 1's time of day
// clock and resets its seconds, which is the GEOS system clock. This waits
// up to about 2 seconds by counting raster frames instead.
char uci_detect(void)
{
	char ok, frames;

	enableIO();
	ok = uii_detect();
	if (!ok) {
		uii_enable();		// firmware 3.15+ unlock; ignored by older firmware
		for (frames = 0; frames < 100 && !ok; frames++) {
			while (*(volatile char *)0xd012 != 0)	// wait for raster line 0
				;
			while (*(volatile char *)0xd012 == 0)
				;
			ok = uii_detect();
		}
	}
	restoreIO();
	return ok;
}
