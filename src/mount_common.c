/*
GeoUMount - Common routines
Utility to synch GEOS system time with NTP server and UII+ RTC clock
Written in 2023 by Xander Mol

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <geos.h>
#include <peekpoke.h>
#include "defines.h"
#include "mount_common.h"
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

// Global variables
unsigned char validdrive[4] = {0,0,0,0};
char drivetypes[8][14] = {
    "No target",
    "Emulated 1541",
    "Emulated 1571",
    "Emulated 1581",
    "RAM 1541",
    "RAM 1571",
    "RAM 1581",
    "RAM DNP"
};
char entrytypes[7][4] = {
    "DIR",
    "D64",
    "D71",
    "D81",
    "DNP",
    "!TL",
    "!IS"
};
char drivetypeID[4];
char ramdiskID[8];
unsigned char targetdrive;
struct mconfig mountconfig;
unsigned char firmwareflag;

// Common routines for GeoUMount and GeoUConfig
unsigned char CheckStatus() {
// Function to check UII+ status and print error box if applicable

    if (!uii_success()) {
        DlgBoxOk("Ultimate Command Interface error!",(const char *)uii_status);
        return 1;
    }
    return 0;
}

unsigned char Checkcommandsupport() {
// Function to check if command is supported in present firmware

    if(uii_status[0] == '2' && uii_status[1] =='1') {
        return 1;
    } else {
        return 0;
    }
}

void ReadConfigdata() {
// Read configuration data from the application file header
// Application data area is at byte 40 to 63 from the author section of the fileheader Struct

    mountconfig.auto_override   = fileHeader.author[40];
    mountconfig.valid[0]        = fileHeader.author[41];
    mountconfig.valid[1]        = fileHeader.author[42];
    mountconfig.valid[2]        = fileHeader.author[43];
    mountconfig.valid[3]        = fileHeader.author[44];
    mountconfig.target          = fileHeader.author[45];
}

void SetValidDrives() {
// Initialise variables to defines which drive IDs are valid targets

    unsigned char drive;

    targetdrive = 0;
    firmwareflag = 0;

    // Get GEOS drive types
    CopyFString(4,drivetypeID,(char *) (DRIVETYPES));

    // Initialize DOS target of Ultimate Command Interface
    enableIO();

    if(!uii_detect()) { restoreIO(); DlgBoxOk("No Ultimate Command Interface","Press OK to abort."); EnterDeskTop(); }

	uii_settarget(TARGET_DOS1);
    if(uii_isdataavailable())
	{
		uii_abort();
	}

    // Set dir at home dir
    uii_change_dir_home();

    // Get RAM disk info from UCI
    memset(ramdiskID,0,8);
    uii_get_ramdisk_info();
    restoreIO();
    if(!Checkcommandsupport()) {
        // Only set RAMdisk info if command is supported in firmware
        CopyFString(8,ramdiskID,uii_data);
    }
    else
    {
        firmwareflag = 1;
    }

    // Get device info from UCI
    enableIO();
    uii_get_deviceinfo();
	restoreIO();    
    if(Checkcommandsupport()) {
        mountconfig.auto_override = 1;
        firmwareflag = 2;
    }

    if(mountconfig.auto_override) {
    // Set valid drives with the override settings in the config data
        
        for(drive=0;drive<4;drive++) {
            validdrive[drive] = mountconfig.valid[drive];
        }

    } else {
    // Set valid drives with auto detection

        for(drive=0;drive<4;drive++)
        {
            validdrive[drive]=0;

            // Check if drive ID is an Ulimate emulated drive
            if( ( uii_data[2] == (drive + 8) && uii_data[3] ) ||
                ( uii_data[5] == (drive+8) && uii_data[6]) ) {
                if(drivetypeID[drive]<4) {
                    validdrive[drive] = drivetypeID[drive];
                    if(!targetdrive) { targetdrive = drive+1; }
                }
            }

            // Check if drive ID is a supported RAM drive
            if(ramdiskID[2*drive]) {
                switch (ramdiskID[2*drive])
                {
                case 0x41:
                    validdrive[drive]=4;
                    break;

                case 0x71:
                    validdrive[drive]=5;
                    break;

                case 0x81:
                    validdrive[drive]=6;
                    break;

                case 0xdd:
                    validdrive[drive]=7;
                    break;

                default:
                    break;
                }
                if(!targetdrive) { targetdrive = drive+1; }
            }
        }
    }

    // Set target drive to override if set and valid
    if(mountconfig.target && validdrive[mountconfig.target-1]) { targetdrive = mountconfig.target; }
}