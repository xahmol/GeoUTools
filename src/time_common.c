/*
GeoUTime - Common routines
Utility to synch GEOS system time with NTP server and UII+ RTC clock
Written in 2022 by Xander Mol

https://github.com/xahmol/GeoUTools
https://www.idreamtin8bits.com/

Code and resources from others used:

-   CC65 cross compiler:
    https://cc65.github.io/

-   Daniel England (Daniel England) - GEOSBuild:
    https://github.com/M3wP/GEOSBuild
    (buildtool for GEOS disks)

-   Hitchhikers Guide to GEOS v2020
    Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2020

-   Samole sequential application header of GeoProgrammer
    Berkeley Softworks

-   Scott Hutter - Ultimate II+ libraru:
    https://github.com/xlar54/ultimateii-dos-lib/tree/master/src/samples
   
-   Tested using real hardware (C128D and C128DCR) using GEOS128 2.0

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
char configbuffer[91];
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
    {'G','e','o','U','T','i','m','e',' ',' ',' ',' ','V','0','.','1',0},
    0x40,
    {'X','a','n','d','e','r',' ','M','o','l',0,0,0,0,0,0,0,0,0,0,
     'G','e','o','U','T','i','m','e',' ',' ',' ',' ','V','0','.','1',0,0,0,0},
    {"Config file for GeoUTime."}
};

// Config save and load routines

void ConfigSave() {
// Save configuration file

    unsigned char error;

    // Clear configbuffer'
    memset(configbuffer,0,91);

    // Copy host to savebuffer
    CopyString(configbuffer, host);

    // Copy UTC offset to savebuffer
    sprintf(configbuffer+80,"%ld",secondsfromutc);

    // Set fileheader for save file
    POKEW((int)&configfileHdr.n_block,(int)&filename);
    configfileHdr.load_address = (int)configbuffer;
    configfileHdr.end_address= (int)configbuffer + 90;

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

    // Reading hostname
    CopyFString(80, host, configbuffer);

    // Reading UTC offset
    secondsfromutc = strtol(configbuffer+80,&ptrend,10);
}