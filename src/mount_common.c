/*
GeoUTools - drive detection and GeoUMount settings (GeoUMount, GeoUConfig)
Written in 2023 by Xander Mol, Oscar64 version 2026

https://github.com/xahmol/GeoUTools

https://www.idreamtin8bits.com/

Code and resources from others used:

-   Scott Hutter - Ultimate II+ library and GeoUTerm:
    https://github.com/xlar54/ultimateii-dos-lib/
    (library for the UII+ UCI functions. And GeoUTerm as GEOS sample application using this)

-   CC65 cross compiler:
    https://cc65.github.io/

-   Daniel England (Daniel England) - GEOSBuild:
    https://github.com/M3wP/GEOSBuild
    (buildtool for GEOS disks)

-   David Lee - GEOS Image Editor
    https://www.facebook.com/groups/704637082964003/permalink/5839146806179646
    (editor for GEOS icons)

-   Subchrist Software - SpritePad Pro
    https://subchristsoftware.itch.io/spritepad-c64-pro
    (editor used for the file icons)

-   Hitchhikers Guide to GEOS v2020
    Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2020

-   Sample sequential application header of GeoProgrammer
    Berkeley Softworks

-   ntp2ultimate by MaxPlap: code for obtaining time via NTP
    https://github.com/MaxPlap/ntp2ultimate

-   Heureks: GEOS64 Programming Examples
    https://codeberg.org/heurekus/C64-Geos-Programming
    (sample code using C in CC65 for writing applications in GEOS)

-   Lyonlabs / Cenbe GEOS resources page:
    https://www.lyonlabs.org/commodore/onrequest/geos/index.html

-   Bo Zimmerman GEOS resources page:
    http://www.zimmers.net/geos/

-   CVM Files GEOS disk images
    https://cbmfiles.com/geos/geos-13.php

-   GEOS - Wheels - GeoWorks - MegaPatch - gateWay - BreadBox Facebook Group
    https://www.facebook.com/groups/704637082964003/permalink/5839146806179646

-   Bart van Leeuwen for testing and providing the Device Manager ROM and GEOS RAM Boot

-   Gideon Zweijtzer for creating the Ultimate II+ cartridge and the Ultimate64, and the Ultimate Command Interface enabling this software.
   
-   Tested using real hardware (C128D, C128DCR, Ultimate 64) using GEOS128 2.0, GEOS64 2.0 and Megapatch3 128.

Licensed under the GNU General Public License v3.0

The code can be used freely as long as you retain
a notice describing original source and author.

THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL,
BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
*/

#include <string.h>
#include <geos.h>
#include "defines.h"
#include "uci_io.h"
#include "mount_common.h"
#include <ultimate_common_lib.h>
#include <ultimate_dos_lib.h>

char validdrive[4];
const char drivetypes[8][14] = {
	"No target",
	"Emulated 1541",
	"Emulated 1571",
	"Emulated 1581",
	"RAM 1541",
	"RAM 1571",
	"RAM 1581",
	"RAM DNP"
};
const char entrytypes[7][4] = { "DIR", "D64", "D71", "D81", "DNP", "!TL", "!IS" };
char drivetypeID[4];
char ramdiskID[8];
char targetdrive;
struct mconfig mountconfig;
char firmwareflag;

// UCI status check with an error box.
char CheckStatus(void)
{
	if (!UII_SUCCESS) {
		DlgBoxOk("Ultimate Command Interface error!", uii_status);
		return 1;
	}
	return 0;
}

// 1 if the firmware does not know the last command (status "21").
char Checkcommandsupport(void)
{
	return uii_status[0] == '2' && uii_status[1] == '1';
}

// Settings from GeoUMount's file header, which GEOS loaded into fileHeader
// ($8100) when it started the application.
void ReadConfigdata(void)
{
	memcpy(&mountconfig, (const char *)&fileHeader.author[MCONFIG_OFFSET], sizeof(mountconfig));
}

// Determine which GEOS drives (A-D) can take a mounted image: Ultimate
// emulated drives and GEOS RAM drives in the REU (firmware 3.10f+), or the
// manual settings.
void SetValidDrives(void)
{
	char drive, ok;

	targetdrive = 0;
	firmwareflag = 0;

	memcpy(drivetypeID, (const char *)DRIVETYPES, 4);

	ok = uci_detect();
	if (!ok) {
		DlgBoxOk("No Ultimate Command Interface", "Press OK to abort.");
		EnterDeskTop();
	}

	enableIO();
	uii_settarget(TARGET_DOS1);
	if (uii_isdataavailable())
		uii_abort();
	uii_change_dir_home();

	// RAM disk information (firmware 3.10f+)
	memset(ramdiskID, 0, 8);
	uii_get_ramdisk_info();
	restoreIO();
	if (!Checkcommandsupport())
		memcpy(ramdiskID, uii_data, 8);
	else
		firmwareflag = 1;

	// Emulated drive information
	enableIO();
	uii_get_deviceinfo();
	restoreIO();
	if (Checkcommandsupport()) {
		mountconfig.auto_override = 1;
		firmwareflag = 2;
	}

	if (mountconfig.auto_override) {
		for (drive = 0; drive < 4; drive++)
			validdrive[drive] = mountconfig.valid[drive];
	} else {
		for (drive = 0; drive < 4; drive++) {
			validdrive[drive] = 0;
			// Ultimate emulated drive A or B with this device number
			if ((uii_data[2] == drive + 8 && uii_data[3]) ||
			    (uii_data[5] == drive + 8 && uii_data[6])) {
				if (drivetypeID[drive] < 4) {
					validdrive[drive] = drivetypeID[drive];
					if (!targetdrive)
						targetdrive = drive + 1;
				}
			}
			// GEOS RAM drive known to the Ultimate
			if (ramdiskID[2 * drive]) {
				switch (ramdiskID[2 * drive]) {
				case 0x41: validdrive[drive] = 4; break;
				case 0x71: validdrive[drive] = 5; break;
				case 0x81: validdrive[drive] = 6; break;
				case 0xdd: validdrive[drive] = 7; break;
				}
				if (!targetdrive)
					targetdrive = drive + 1;
			}
		}
	}

	if (mountconfig.target && validdrive[mountconfig.target - 1])
		targetdrive = mountconfig.target;
}
