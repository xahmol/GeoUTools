/*
GeoUTime - Common routines
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
#include "time_common.h"

// Global variables
char buffer[81];
long secondsfromutc = 3600; 
char host[81] = "pool.ntp.org";
unsigned char verbose = 0;
unsigned char ntpon = 1;
char configbuffer[95];
char filename[13] = "GeoUTimeDat";

// Declare config file header
struct fileheader configfileHdr = {
    {0,0},
    {3, 21, 63 | 0x80 },
    {   0xC0,0x3C,0x00,0xF0,0x3C,0x00,0xF0,0x3C,0x00,0xF0,0x3C,0x00,0x00,0x03,0xE0,0xF0,
        0x03,0xC0,0x00,0x03,0x00,0xF0,0x00,0x00,0x00,0x03,0x00,0xF0,0x03,0xC0,0x00,0x03,
        0xE0,0xF0,0x3C,0x00,0xF0,0x3C,0x38,0xF0,0x3C,0x44,0xF8,0x3C,0x92,0xFF,0xFD,0x11,
        0x7F,0xFD,0x1D,0x3F,0xFD,0x01,0x0F,0xFC,0x82,0x00,0x00,0x44,0x00,0x00,0x38},
    SEQ,
    APPL_DATA,
    SEQUENTIAL,
    0,
    0,
    0,
    {'G','e','o','U','T','i','m','e',' ',' ',' ',' ','V','1','.','0',0},
    0x40,
    {'X','a','n','d','e','r',' ','M','o','l',0,0,0,0,0,0,0,0,0,0,
     'G','e','o','U','T','i','m','e',' ',' ',' ',' ','V','1','.','0',0,0,0,0},
    {"Config file for GeoUTime."}
};

// Config save and load routines

void ConfigSave() {
// Save configuration file

    unsigned char error;

    // Clear configbuffer'
    memset(configbuffer,0,95);

    // Copy host to savebuffer
    CopyString(configbuffer, host);

    // Copy UTC offset to savebuffer
    sprintf(configbuffer+80,"%ld",secondsfromutc);

    // Copy verbose flag to savebuffer
    configbuffer[92] = verbose+48;

    // Copy NTP on flag to savebuffer
    configbuffer[93] = ntpon+48;

    // Copy config file version
    configbuffer[94] = configfileversion + 48;

    // Set fileheader for save file
    POKEW((int)&configfileHdr.n_block,(int)&filename);
    configfileHdr.load_address = (int)configbuffer;
    configfileHdr.end_address= (int)configbuffer + 95;

    // Delete old file
    DeleteFile(filename);

    // Save file to disk
    error = SaveFile(0,&configfileHdr);

    if(error) {
        sprintf(buffer,"Error: %d",error);
        DlgBoxOk("Error saving config file",buffer);
    }
}

void ConfigLoad() {
// Load configuration file

    unsigned char error;
    char* ptrend;

    // Load config file
    error = GetFile(1,filename,configbuffer,NULL,NULL);

    // Error handling
    if(error)
    {
        // If file does not exist, create one with default values
        if(error=FILE_NOT_FOUND) {
            ConfigSave();
        }
        // Print message on other errors
        else {
            sprintf(buffer,"Error: %d",error);
            DlgBoxOk("Error loading config file",buffer);
        }
        return;
    }

    // Reading verbose flag
    verbose = configbuffer[92]-48;

    // Reading ntp on flag
    ntpon = configbuffer[93]-48;

    // Reading hostname
    CopyFString(80, host, configbuffer);

    // Reading UTC offset
    secondsfromutc = strtol(configbuffer+80,&ptrend,10);
}