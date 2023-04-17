/*
GeoUMount
Disk mounter for the Ultimate II+ in GEOS
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

-   markusC64 (https://github.com/markusC64)
    for code rveiew, testiing and adapting UII+ firmware to enableRAM disk functionality.

-   mkslack ( https://github.com/mkslack ) for code review and testing

-   Forum64.de forum thread for testing and suggestions
    ( https://www.forum64.de/index.php?thread/133681-news-zur-ultimate-1541-ii-und-geos/ )

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
//#include <conio.h>
#include "defines.h"
#include "interface.h"
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

// Global variables
char buffer[81];
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
unsigned char clickflag;

// Directory entry struct
struct DirElement {
    unsigned char type; // Type: 1=dir, 2=D64, 3=D71, 4=D81
	char filename[21];
    struct DirElement* next;
    struct DirElement* prev;
};
struct DirElement *presentdirelement;

struct Directory {
    struct DirElement* firstelement;
    struct DirElement* lastelement;
    struct DirElement* firstprint;
    struct DirElement* lastprint;
};
struct Directory presentdir;

// Icons
// Select drive icons: two cards wide (16 pxiels) amd 16 lines
char iconA[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x83,0x81,0x83,0x83,0xC3,0x83,0xC3,0x86,
    0x63,0x86,0x63,0x8F,0xF3,0x8C,0x33,0x8C,0x33,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconB[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x8F,0xC3,0x8C,0x63,0x8C,0x63,0x8F,0xC3,0x8C,
    0x63,0x8C,0x63,0x8C,0x63,0x8C,0x63,0x8F,0xC3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconC[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x87,0xC3,0x8C,0x63,0x8C,0x03,0x8C,0x03,0x8C,
    0x03,0x8C,0x03,0x8C,0x03,0x8C,0x63,0x87,0xC3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconD[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x8F,0x83,0x8C,0xC3,0x8C,0x63,0x8C,0x63,0x8C,
    0x63,0x8C,0x63,0x8C,0x63,0x8C,0xC3,0x8F,0x83,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

// Scroll arrow icons of 1 card by 8 lines
char iconArrowUp[] = {
    0x88,0xFF,0x88,0x9C,0xBE,0x80,0xBE,0x80,0xFF
};

char iconArrowDown[] = {
    0x88,0xFF,0x80,0xBE,0x80,0xBE,0x9C,0x88,0xFF
};

// Dir travel icons of 2 cards by 16 lines
char iconBack[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x03,0x83,0x03,0x87,0x03,0x8F,0xF3,0x9F,
    0xF3,0x8F,0xF3,0x87,0x03,0x83,0x03,0x81,0x03,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconRoot[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x80,0x33,0x80,0x73,0x80,0xE3,0x81,0xC3,0x83,
    0x83,0x87,0x03,0x8E,0x03,0x9C,0x03,0x98,0x03,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconHome[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x03,0x82,0x83,0x84,0x43,0x88,0x23,0x90,
    0x13,0x97,0x53,0x95,0x53,0x95,0x13,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

char iconTop[] = {
    0x94,0xFF,0xFE,0x80,0x03,0x80,0x03,0x9F,0xF3,0x9F,0xF3,0x80,0x03,0x81,0x03,0x83,
    0x83,0x87,0xC3,0x8F,0xE3,0x04,0x83,0x88,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,0xFF
};

char iconBottom[] = {
    0x86,0xFF,0xFE,0x80,0x03,0x80,0x03,0x04,0x83,0x96,0x8F,0xE3,0x87,0xC3,0x83,0x83,
    0x81,0x03,0x80,0x03,0x9F,0xF3,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,0xFF
};

char iconSave[] = {
    0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x9F,0xE3,0x97,0x53,0x97,0x53,0x97,0x53,0x97,
    0xD3,0x90,0x13,0x97,0xD3,0x98,0x33,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
    0xFF
};

// Declare interface functions
void DriveselectA (void);
void DriveselectB (void);
void DriveselectC (void);
void DriveselectD (void);
void DirBack (void);
void DirRoot (void);
void DirHome (void);
void DirTop (void);
void DirBottom (void);
void SaveImage (void);
void ScrollUp (void);
void ScrollDown (void);

// Interface icontab
struct icontab vic_mainicons = {
    12,
    { 0,0 },
    {
        { iconA, 28, 50, 2, 16, (int)DriveselectA },
        { iconB, 28, 70, 2, 16, (int)DriveselectB },
        { iconC, 28, 90, 2, 16, (int)DriveselectC },
        { iconD, 28, 110, 2, 16, (int)DriveselectD },
        { iconBack, 28, 130, 2, 16, (int)DirBack },
        { iconRoot, 28, 150, 2, 16, (int)DirRoot },
        { iconHome, 28, 170, 2, 16, (int)DirHome },
        { iconTop, 31, 130, 2, 16, (int)DirTop },
        { iconBottom, 31, 150, 2, 16, (int)DirBottom },
        { iconSave, 31,170, 2, 16,(int)SaveImage},
        { iconArrowUp, 25, 45, 1, 8, (int)ScrollUp },
        { iconArrowDown, 25, 180, 1, 8, (int)ScrollDown }
    }
};

struct icontab vdc_mainicons = {
    12,
    { 0,0 },
    {
        { iconA, 56, 50, 2 | DOUBLE_B, 16, (int)DriveselectA },
        { iconB, 56, 70, 2 | DOUBLE_B, 16, (int)DriveselectB },
        { iconC, 56, 90, 2 | DOUBLE_B, 16, (int)DriveselectC },
        { iconD, 56, 110, 2 | DOUBLE_B, 16, (int)DriveselectD },
        { iconBack, 56, 130, 2 | DOUBLE_B, 16, (int)DirBack },
        { iconRoot, 56, 150, 2 | DOUBLE_B, 16, (int)DirRoot },
        { iconHome, 56, 170, 2 | DOUBLE_B, 16, (int)DirHome },
        { iconTop, 61, 130, 2 | DOUBLE_B, 16, (int)DirTop },
        { iconBottom, 61, 150, 2 | DOUBLE_B, 16, (int)DirBottom },
        { iconSave, 61,170, 2 | DOUBLE_B, 16,(int)SaveImage},
        { iconArrowUp, 50, 45, 1, 8, (int)ScrollUp },
        { iconArrowDown, 50, 180, 1, 8, (int)ScrollDown },
    }
};

// Interface coords for handling 40 and 80 column
struct intCoords {
    unsigned int filelist_xstart;
    unsigned int filelist_xend;
    unsigned int scroll_xend;
    unsigned int righttab_xstart;
};

struct intCoords *interfaceCoords;

struct intCoords vic_intCoords = { 5,200,208,224 };
struct intCoords vdc_intCoords = { 5,400,408,464 };

// Declare functions prototypes that are called when menu items are 
// clicked on and are used in the structs that defines the menus below.
void geosSwitch4080(void);
void geosExit (void);
void informationCredits (void);

// Menu structures with pointers to the menu handlers above
static struct menu menuGEOS = {
    // GEOS
        { 15, 15+2*15, 0, 100 },
        2 | VERTICAL,
          {
            { "Switch 40/80", MENU_ACTION, geosSwitch4080 },
            { "Exit", MENU_ACTION, geosExit },
          }
 };

static struct menu menuMain = {
    // Main menu
        { 0, 15, 0, 95 },
        2 | HORIZONTAL,
          {
            { "GEOS", SUB_MENU, &menuGEOS},
            { "Credits", MENU_ACTION, informationCredits }
          }
 };

// Declare scroll routines
void vdc_write_reg(void);
void vdc_copyline(unsigned char srchi, unsigned char srclo, unsigned char desthi, unsigned char destlo);

// Application routines
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

void SetValidDrives() {
// Initialise variables to defines which drive IDs are valid targets

    unsigned char drive;

    targetdrive = 0;

    // Get GEOS drive types
    CopyFString(4,drivetypeID,(char *) (DRIVETYPES));

    // Initialize DOS target of Ultimate Command Interface
    enableIO();
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
    //else
    //{
    //    // Check if DEVINFO is known (which is the case on older firmwares)
    //    DlgBoxOk("Old firmware detected","Click OK to abort");
    //    EnterDeskTop();
    //}

    // Get device info from UCI
    enableIO();
    uii_get_deviceinfo();
	restoreIO();    
    if(Checkcommandsupport()) {
        // Check if DEVINFO is known (which is the case on older firmwares)
        DlgBoxOk("Old firmware detected","Click OK to abort");
        EnterDeskTop();
    }
    
    for(drive=0;drive<4;drive++)
    {
        validdrive[drive]=0;

        // Check if drive ID is an Ulimate emulated drive
        if( ( uii_data[2] == (drive + 8) && uii_data[3] ) ||
            ( uii_data[5] == (drive+8) && uii_data[4]) ) {
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

void Freedir() {
// Free memory of presently loaded dir

    struct DirElement* next = 0;
    struct DirElement* present;

    present = presentdir.firstelement;
    
    do
    {
        presentdirelement = present;
        next = presentdirelement->next;
        free(present);
        present = next;

        //SetPattern(0);
        //SetRectangleCoords(150,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
        //Rectangle();
        //gotoxy(1,20);
        //cprintf("%d %d",present,next);
    } while (next);
}

void Readdir() {
// Read present dir of UCI DOS target

    struct DirElement* previous = 0;
    struct DirElement* present = 0;
    unsigned char presenttype;
    unsigned char datalength,maxlength;

    // Free memory of previous dir if any
    if(presentdir.firstelement) { Freedir(); }

    // Clear directory values
    presentdir.firstelement = 0;
    presentdir.firstprint = 0;
    presentdir.firstprint = 0;
    presentdir.lastprint = 0;

    // Initialise reading dir
    enableIO();
    uii_open_dir();
    if(!uii_success()) {
        uii_abort();
        restoreIO();
        return;
    }
    uii_get_dir();

    // Loop while dir data is available or memory is full
    while(uii_isdataavailable())
	{
        // Reset entry type
        presenttype = 0;

        // Get next dir entry
		uii_readdata();
		uii_accept();

        // Quick fix for when uii_isdataavailable() does not work correctly
        // (works for me on GEOS128, not on GEOS64)
        // Check on second byte of data that is set 0 in last iteration
        // If still zero and not overwritten, no new data is received
        // Abort on receiving empty data
        //if(!uii_data[1]) {
        //        uii_abort();
        //        restoreIO();
        //        return;
        //}

        datalength = strlen(uii_data);

        // Check if entry is a dir by checking if bit 4 of first byte is set
        if(uii_data[0]&0x10) { presenttype=1; }

        // Check if file is a matching image type
        if(!presenttype && datalength>4) {

            // Check for filename extension of a disk image (D64/G64, D71/G71 or D81)

            // First check for D or G as first letter of extension after a dot
            if( (   uii_data[datalength-4] == '.') && 
                (   uii_data[datalength-3] == 'd' || uii_data[datalength-3] == 'D' ||
                    uii_data[datalength-3] == 'g' || uii_data[datalength-3] == 'G' ) ) {

                // Check for D64/G64
                if( (uii_data[datalength-2] == '6') && (uii_data[datalength-1] == '4') ) {
                // Allow D64/G64 on 1541, 1571 and RAM 1541
                    if (validdrive[targetdrive-1] == 1 ||
                        validdrive[targetdrive-1] == 2 ||
                        validdrive[targetdrive-1] == 4) {
                            presenttype = 2; }
                }

                // Check for D71/G71
                if( (uii_data[datalength-2] == '7') && (uii_data[datalength-1] == '1') ) {
                // Allow D71/G71 on 1571 and RAM 1571
                    if (validdrive[targetdrive-1] == 2 ||
                        validdrive[targetdrive-1] == 5) {
                            presenttype = 3; }
                }

                // Check for D81
                if( (uii_data[datalength-2] == '8') && (uii_data[datalength-1] == '1') ) {
                // Allow D81 on 1581 and RAM 1581
                    if (validdrive[targetdrive-1] == 3 ||
                        validdrive[targetdrive-1] == 6) {
                            presenttype = 4; }
                }

                // Check for DNP
                if( ((uii_data[datalength-2] == 'n') && (uii_data[datalength-1] == 'p')) ||
                    ((uii_data[datalength-2] == 'N') && (uii_data[datalength-1] == 'P')) ) {
                // Allow DNP on RAM DNP
                    if (validdrive[targetdrive-1] == 7 ) {
                            presenttype = 5; }
                }
            }
        }

        //SetPattern(0);
        //SetRectangleCoords(150,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
        //Rectangle();
        //gotoxy(1,20);
        //cprintf("%d %d %d %d %d",presenttype,uii_data[0],uii_data[1],uii_data[2],uii_isdataavailable());
        //gotoxy(1,21);
        //cprintf("%s",uii_data+1);

        if(presenttype) {
            // Get file or dir name to buffer
            maxlength = datalength-1; // Minus 1 for first attribute byte
            if(maxlength>20) {maxlength=20; presenttype=6; }    // Truncate for max 20
            memset(buffer,0,21);
            CopyFString(maxlength,buffer,uii_data+1);

            // Allocate memory for dir entry
            present = calloc(1, 26);

            // Break loop if memory is full
            if(!present ||  _heapmemavail()<MIN_HEAP_SIZE) {
                // Abort UCI dir reading
                uii_abort();
                restoreIO();
                return;
            }

            // Set direntry data
            presentdirelement = present;
            CopyString(presentdirelement->filename,buffer);
            presentdirelement->type = presenttype;

            //gotoxy(1,21);
            //cprintf("%d %4x %s %s %d",maxlength,present,buffer,presentdirelement->filename,presentdirelement->type);

            // Set direntry pointers
            presentdir.lastelement = present;       // Update dir last element
            if(!previous) { presentdir.firstelement = present; presentdir.firstprint = present; previous=present; }
            else {
                presentdirelement->prev = previous;     // Set prev in new entry
                presentdirelement = previous;           // Load previous element
                presentdirelement->next = present;      // Set next in this prev element
                previous=present;                       // Update previous pointer
            }
            //gotoxy(1,22);
            //cprintf("%4x %4x %4x %4x",previous,presentdirelement->next,presentdir.firstelement,presentdir.lastelement);
        }

        // Clear second byte for quick fix on data available test
        //uii_data[1]=0;
        //cgetc();
	}

    // Switch out IO again and restore original GEOS state
    restoreIO();
}

// Screen functions

void DrawIDandPath(unsigned char refresh) {
// Draw UCI ID and pathname. Clear previous if refresh flag set

    unsigned int maxlength = interfaceCoords->scroll_xend-11;
    unsigned char maxchar, nowchar;

    // Clear previous text if refresh flag is set
    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(21,44,interfaceCoords->filelist_xstart+1,interfaceCoords->scroll_xend-1);
        Rectangle();
    }

    // Get ID from UCI and print
    enableIO();
    uii_identify();
	restoreIO();

    sprintf(buffer,"ID: %s",uii_data);
    PutString(buffer,29,interfaceCoords->filelist_xstart+5);

    // Get present path from UCI and print
    enableIO();
    uii_get_path();
	restoreIO();

    // Get width of pathname
    CopyString(buffer,uii_data);

    // Check if it fits and continue to shorten till it fits
    nowchar = strlen(buffer);
    maxchar = nowchar-1;

    while (StringLength(buffer) > maxlength)
    {
        CopyString(buffer,uii_data+(nowchar-maxchar--));
    }

    PutString(buffer,39,interfaceCoords->filelist_xstart+5);
}

void DrawDrivetypes() {
// Draw the drive types of the targets

    unsigned char drive;

    for(drive=0;drive<4;drive++)
    {
        PutString(drivetypes[validdrive[drive]],58+(20*drive),interfaceCoords->righttab_xstart+20);
    }    
}

void DrawTargetdrive(unsigned char refresh) {
// Draw presently selected target. Clear area first on refresh flag.

//  unsigned char drive;

    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(20,30,interfaceCoords->righttab_xstart+1,screen_pixel_width-2);
        Rectangle();
    }

    if(targetdrive)
    {
        sprintf(buffer,"Target is drive %c",targetdrive-1+'A');
    }
    else
    {
        sprintf(buffer,"No valid target");
    }
    PutString(buffer,29,interfaceCoords->righttab_xstart);

    //for(drive=0;drive<8;drive++)
    //{
    //    sprintf(buffer,"%2X",ramdiskID[drive]);
    //    PutString(buffer,39,interfaceCoords->righttab_xstart+16*drive);
    //}
}

void PrintDirEntry(unsigned char printpos) {
// Print entry

    unsigned char type = presentdirelement->type;

    if(type == 5) {
        // Check if DNP is correct size
        enableIO();
        uii_loadIntoRamDisk(targetdrive-1,presentdirelement->filename,1);
        if(!uii_success()) {
            uii_abort();
            type=7;
            presentdirelement->type = type;
        }
        restoreIO();
    }
    PutString(presentdirelement->filename,printpos,interfaceCoords->filelist_xstart+5);
    PutString(entrytypes[type-1],printpos,interfaceCoords->filelist_xstart+165);
}

void DrawDir(refresh) {
// Draw the dirlisting

    unsigned char printpos = 59;
    struct DirElement* present;

    // Clear area on request
    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(46,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
        Rectangle();
    }

    // Read directory contents
    if(refresh!=2) { Readdir(); }

    // Print no data if no valid entries in dir are found
    if(!presentdir.firstprint) {
        PutString("No data.",59,interfaceCoords->filelist_xstart+5);
    }
    // Print entries until area is filled or last item is reached
    else
    {
        // Get direlement
        present = presentdir.firstprint;

        // Loop while area is not full and further direntries are still present
        do
        {
            // Get new element
            presentdirelement = present;

            // Print entry
            PrintDirEntry(printpos);

            //SetPattern(0);
            //SetRectangleCoords(150,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
            //Rectangle();
            //gotoxy(1,20);
            //cprintf("%4x %d",present,type);

            // Update lastprinted and printpos
            presentdir.lastprint = present;
            printpos += 10;

            // Check if next dir entry is present, if no: break. If yes: update present pointer
            if(!presentdirelement->next) { break; }
            else { present = presentdirelement->next; }

            //gotoxy(1,21);
            //cprintf("%4x %4x %4x %d",present,presentdir.firstprint,presentdir.lastprint,printpos);
            //cgetc();

        } while (printpos<187);
    }
}

void DrawFilebrowser() {
// Draw file browser interface

    // Set x coords based on 40 or 80
    if(vdc) {
        interfaceCoords = &vdc_intCoords;
    }
    else {
        interfaceCoords = &vic_intCoords;
    }

    // Draw interface
    SetRectangleCoords(20,187,interfaceCoords->filelist_xstart,interfaceCoords->scroll_xend);
    FrameRectangle(255);
    HorizontalLine(255,45,interfaceCoords->filelist_xstart,interfaceCoords->filelist_xend);
    HorizontalLine(255,116,interfaceCoords->filelist_xend,interfaceCoords->scroll_xend);
    VerticalLine(255,45,187,interfaceCoords->filelist_xend);

    // Print ID and present path
    DrawIDandPath(0);

    // Draw target section
    DrawTargetdrive(0);
    DrawDrivetypes();
    DrawDir(0);
}

void scroll_down() {
// Scroll one position down

	unsigned char t = 0;
    struct DirElement* present;

    // Get next element
    present = presentdir.lastprint;
    presentdirelement = present;
    present = presentdirelement->next;
    if(!present) { return; }
    presentdirelement = present;

    // Update printed pointers
    presentdir.lastprint = present;
    present = presentdir.firstprint;
    presentdirelement = present;
    presentdir.firstprint = presentdirelement->next;
	
    // Check for VDC
	if(vdc == 0)
	{
        // Redraw dir
        DrawDir(2);
	}
	else
	{
        // Do scroll for VDC backscreen for the upper 100 scanlines
        for(t=60;t<100;t++)
        {
            // Move line row up	
            MoveData(   BACK_SCR_BASE + ((t-10)*80) + 1,
                        BACK_SCR_BASE + (t*80) + 1,
                        49);
        }
        // Do scroll for VDC backscreen for the lines 100-110
        for(t=0;t<10;t++)
        {
            // Move line row up	
            MoveData(   BACK_SCR_BASE + ((t+90)*80) + 1,
                        SCREEN_BASE + (t*80) + 65,
                        49);
        }
        // Do scroll for VDC backscreen for the lower 90 scanlines
        for(t=10;t<82;t++)
        {
            // Move line row up	
            MoveData(   SCREEN_BASE + ((t-10)*80) + 65,
                        SCREEN_BASE + (t*80) + 65,
                        49);
        }

        // Copy from back screen
        SetRectangleCoords(50,180,8,interfaceCoords->filelist_xend-1);
        RecoverRectangle();

        // Clear upper area to avoid leftovers
        SetPattern(0);
        SetRectangleCoords(46,49,8,interfaceCoords->filelist_xend-1);
        Rectangle();
    
        // Clear lower area to avoid leftovers
        SetRectangleCoords(172,186,8,interfaceCoords->filelist_xend-1);
        Rectangle();

        // Print last element afgain
        present = presentdir.lastprint;
        presentdirelement = present;
        PrintDirEntry(179);
	}
}

void scroll_up() {
// Scroll one position up

	signed char t = 0;
    struct DirElement* present;

    // Get previous element
    present = presentdir.firstprint;
    presentdirelement = present;
    present = presentdirelement->prev;
    if(!present) { return; }
    presentdirelement = present;

    // Update printed pointers
    presentdir.firstprint = present;
    present = presentdir.lastprint;
    presentdirelement = present;
    presentdir.lastprint = presentdirelement->prev;
	
    // Check for VDC
	if(vdc == 0)
	{
        // Redraw dir
        DrawDir(2);
	}
	else
	{
        // Do scroll for VDC backscreen for the lower 90 scanlines
        for(t=69;t>-1;t--)
        {
            // Move line row up	
            MoveData(   SCREEN_BASE + ((t+10)*80) + 65,
                        SCREEN_BASE + (t*80) + 65,
                        49);
        }
        // Do scroll for VDC backscreen for the lines 100-110
        for(t=9;t>-1;t--)
        {
            // Move line row up	
            MoveData(   SCREEN_BASE + (t*80) + 65,
                        BACK_SCR_BASE + ((t+90)*80) + 1,
                        49);
        }
        // Do scroll for VDC backscreen for the upper 100 scanlines
        for(t=89;t>47;t--)
        {
            // Move line row up	
            MoveData(   BACK_SCR_BASE + ((t+10)*80) + 1,
                        BACK_SCR_BASE + (t*80) + 1,
                        49);
        }

        // Copy from back screen
        SetRectangleCoords(50,180,8,interfaceCoords->filelist_xend-1);
        RecoverRectangle();

        // Clear upper area to avoid leftovers
        SetPattern(0);
        SetRectangleCoords(46,59,8,interfaceCoords->filelist_xend-1);
        Rectangle();
    
        // Clear lower area to avoid leftovers
        SetRectangleCoords(182,186,8,interfaceCoords->filelist_xend-1);
        Rectangle();

        // Print last element afgain
        present = presentdir.firstprint;
        presentdirelement = present;
        PrintDirEntry(59);
	}
}

void page_down() {
// Perform a page down

    struct DirElement* present;

    // Get next element
    present = presentdir.lastprint;
    presentdirelement = present;
    present = presentdirelement->next;
    if(!present) { return; }
    presentdirelement = present;
    
    // Update first print pointer
    presentdir.firstprint = present;

    // Redraw dir
    DrawDir(2);
}

void page_up() {
// Perform a page up

    struct DirElement* present;
    unsigned char count;

    // Set first printed as start point
    present = presentdir.firstprint;
    presentdirelement = present;
    
    // Go to begin of previous page
    for(count=0;count<13;count++)
    {
        present = presentdirelement->prev;
        if(!present) { break; }
        presentdirelement = present;

        // Update first print pointer
        presentdir.firstprint = present;
    }

    // Redraw dir
    DrawDir(2);
}

// Icon handlers
void TargetChange(unsigned char newdrive) {
// Change target drive

    unsigned char oldtype = validdrive[targetdrive-1];
    targetdrive = newdrive;
    DrawTargetdrive(1);
    if(oldtype != validdrive[newdrive-1]) { DrawDir(1); }
}

void DriveselectA() {
// Select drive A as target if valid as target

    if(validdrive[0])
    {
        TargetChange(1);
    }
}

void DriveselectB() {
// Select drive B as target if valid as target

    if(validdrive[1])
    {
        TargetChange(2);
    }
}

void DriveselectC() {
// Select drive C as target if valid as target

    if(validdrive[2])
    {
        TargetChange(3);
    }
}

void DriveselectD() {
// Select drive A as target if valid as target

    if(validdrive[3])
    {
        TargetChange(4);
    }
}

void DirBack() {
// Go back to parent dir

    enableIO();
    uii_change_dir("..");
	restoreIO();
    DrawIDandPath(1);
    DrawDir(1);
}

void DirRoot() {
// Go back to root dir

    enableIO();
    uii_change_dir("/");
	restoreIO();
    DrawIDandPath(1);
    DrawDir(1);
}

void DirHome() {
// Go to default home dir

    enableIO();
    uii_change_dir_home();
	restoreIO();
    DrawIDandPath(1);
    DrawDir(1);
}

void DirTop() {
// Go to top of filelist

    // Check if first printed is also the first item available. If yes, return
    if(presentdir.firstprint == presentdir.firstelement) { return; }

    // Set first printed at first element
    presentdir.firstprint = presentdir.firstelement;

    // Redraw dir
    DrawDir(2);
}

void DirBottom() {
// Go to bottom of filelist

    struct DirElement* present;
    unsigned char count;

    // Check if last printed is also the last item available. If yes, return
    if(presentdir.lastprint == presentdir.lastelement) { return; }

    // Set first printed as start point
    present = presentdir.lastelement;
    presentdirelement = present;

    // Get first print element
    for(count=0;count<12;count++)
    {
        present = presentdirelement->prev;
        if(!present) { break; }
        presentdirelement = present;
    }

    // Update first print pointer
    presentdir.firstprint = present;

    // Redraw dir
    DrawDir(2);
}

void SaveImage() {
// Save RAM disk image back to an image file
    
    char imagename[21] = "ramdisk";
    unsigned char namelen = 0;

    // Check if targetdrive is a valid and supported RAM disk
    if(validdrive[targetdrive-1]<4) {return; }

    // Ask for filename
    if( DlgBoxGetString(imagename,16,"Enter filename for the image","(or press Cancel)")
        == CANCEL) { return ; }

    // Add extention based on RAM disk type
    namelen = strlen(imagename);
    imagename[namelen] = '.';
    CopyString(imagename+1+namelen,entrytypes[validdrive[targetdrive-1]-3]);

    // Check if file exists
    enableIO();
    uii_open_file(1,imagename);
    if(uii_success()) {
        uii_close_file();
        restoreIO();
        if( DlgBoxYesNo("File exists.","Overwrite file?") == NO ) {
            // Exit function on NO on overwrite
            return;
        } else {
            // Delete existing file if YES on overwrite
            enableIO();
            uii_delete_file(imagename);
        }
    } else {
        uii_abort();
    }

    // Save image
    uii_saveRamDisk(targetdrive-1,imagename);
    restoreIO();
    if( !CheckStatus() ) { 
        sprintf(buffer,"ID %d to %s",targetdrive+7,imagename);
        DlgBoxOk("Image saved.",buffer);
        DrawDir(1);
    }
}

void ScrollUp() {
// Handler for clicking arrow up  

    // Check if first printed is also the first item available. If yes, return
    if(presentdir.firstprint == presentdir.firstelement) { return; }

    // Do scroll
    scroll_up();
}

void ScrollDown() {
// Handler for clicking arrow down  

    // Check if last printed is also the last item available. If yes, return
    if(presentdir.lastprint == presentdir.lastelement) { return; }

    // Do scroll
    scroll_down();
}

// Mouse handler functions

void MountSelected(unsigned char filepos) {
// Check if there is a file at the click location and if yes, mount the file

    unsigned char count;
    struct DirElement* present;

    present = presentdir.firstprint;

    // Return on empty dir
    if(!present) { return; }

    // Move to selected direlement
    presentdirelement = present;
    // If position is higher than first item printed, loop till selected item
    if(filepos>0) {
        for(count=0;count<filepos;count++) {
            // Get next element
            present = presentdirelement->next;
            // Return if filepos is larger than last printed
            if(!present) { return; }
            // Set next element
            presentdirelement = present;
        }
    }

    // Give visual clue of clicking and wait to button is released
    SetRectangleCoords(50+filepos*10,59+filepos*10,5,interfaceCoords->filelist_xend);
    InvertRectangle();
    InvertRectangle();

    // If type is 'too long' or 'incorrect size', return
    if(presentdirelement->type > 5) { return; }

    // If type is dir, change dir
    if(presentdirelement->type == 1) {
        // Change dir
        enableIO();
        uii_change_dir(presentdirelement->filename);
        restoreIO();

        // Redraw filebrowser
        DrawIDandPath(1);
        DrawDir(1);
        return;
    }

    // Mount disk
    enableIO();
    if(!ramdiskID[(targetdrive-1)*2]) {
        // No RAM disk
        uii_mount_disk(targetdrive+7,presentdirelement->filename);
    } else {
        // RAM disk
        uii_loadIntoRamDisk(targetdrive-1,presentdirelement->filename,0);
    }
    restoreIO();

    // Error handling
    if(CheckStatus()) { return; }

    // Confirm and ask if program should be exited
    sprintf(buffer,"%s mounted on ID %d",presentdirelement->filename,targetdrive+7);
    if(DlgBoxYesNo(buffer,"Exit program?") == YES)
    {
        Freedir();
        EnterDeskTop();
    }
}

void OtherMousePress() {
// Handler for mouse clicks on other areas than the icons

    unsigned int xpos = mouseXPos;
    unsigned int ypos = mouseYPos;
    unsigned char filepos;

    // Ignore if window active
    if(windowflag) { return; }

    // Don't act on mouse button release
    if ((mouseData & MOUSE_BTN_DOWN) != 0) { return; }

    // Check if mouse was pressed in filebrowser area
    if(xpos>5 && xpos<interfaceCoords->filelist_xend && ypos>50 && ypos<186)
    {
        filepos = (ypos-50)/10;
        MountSelected(filepos);
        return;
    }

    // Check if mouse was pressed in upper scroll bar
    if(xpos>interfaceCoords->filelist_xend && xpos<interfaceCoords->scroll_xend && ypos>52 && ypos<116)
    {
        // Check if first printed is also the first item available. If yes, return
        if(presentdir.firstprint == presentdir.firstelement) { return; }

        // Perform page up
        page_up();
        return;
    }

    // Check if mouse was pressed in lower scroll bar
    if(xpos>interfaceCoords->filelist_xend && xpos<interfaceCoords->scroll_xend && ypos>116 && ypos<180)
    {
        // Check if last printed is also the last item available. If yes, return
        if(presentdir.lastprint == presentdir.lastelement) { return; }

        // Perform page down
        page_down();
    }
}

// Menu functions

void geosSwitch4080 (void) {
// Switch between 40 and 80 column mode

    SetNewMode();
    ReinitScreen(appname);
    DrawFilebrowser();
    GotoFirstMenu();
    DoMenu(&menuMain);
    DoIcons(icons);
    return;
}

void geosExit (void) {
// Exit to desktop

    ReDoMenu();
    // Ask confirnation
    if (DlgBoxOkCancel("Exit to desktop", "Are you sure?") == OK)
    {
        if(presentdir.firstelement) { Freedir(); }
        EnterDeskTop();
    }
    else
    {
        return;
    }  
}

void informationCredits (void) {
// Show credits
    
    unsigned char xcoord;

    ReDoMenu();

    // Create dialogue window
    xcoord = CreateWindow();

    // Print credits
    PutString(CBOLDON "GeoUMount" CPLAINTEXT,69,xcoord);
    PutString("Disk mounter for the Ultimate II+ in GEOS",79,xcoord);
    sprintf(buffer,"Version: %s",version);
    PutString(buffer,99,xcoord);
    PutString("Written by Xander Mol, 2023",109,xcoord);
    PutString("For documentation, source, license and credits, see",119,xcoord);
    PutString("https://github.com/xahmol/GeoUTools",129,xcoord);
    PutString("https://www.idreamtin8bits.com/",139,xcoord);

    // Show OK icon
    WinOKButton();
    return;
}

// Main

void main (void)
{
    // Set appname
    CopyString(appname,"   GeoUMount   ");

    // Set version number in string variable
    sprintf(version,
            "v%2i.%2i - %c%c%c%c%c%c%c%c-%c%c%c%c",
            VERSION_MAJOR, VERSION_MINOR,
            BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
            BUILD_MONTH_CH0, BUILD_MONTH_CH1, BUILD_DAY_CH0, BUILD_DAY_CH1,
            BUILD_HOUR_CH0, BUILD_HOUR_CH1, BUILD_MIN_CH0, BUILD_MIN_CH1);

    // Get host OS type
    osType = get_ostype();

    // Set presentdir pointer at zero
    presentdir.firstelement = 0;

    // Check if UCI is detected, else abort.
    enableIO();
    if(!uii_detect()) { restoreIO(); DlgBoxOk("No Ultimate Command Interface","Press OK to abort."); EnterDeskTop(); }
    restoreIO();

    // Get valid UII+ drives
    SetValidDrives();

    // Initialize the screen after program startup
    ReinitScreen(appname);
    DoMenu(&menuMain);
    DoIcons(icons);
    DrawFilebrowser();

    // Set mouse handler vector
    otherPressVec = OtherMousePress;
    
    // Never returns    
    MainLoop();
}