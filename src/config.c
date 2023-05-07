/*
GeoUTime - Config program
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
#include "defines.h"
#include "mount_common.h"
#include "time_common.h"
#include "interface.h"
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

// Global variables
unsigned char mount_notfound;

// Interface icontab
struct icontab vic_mainicons = {
    1,
    { 0,0 },
    { 0, 0, 0, 1, 1, 0 }
};

struct icontab vdc_mainicons = {
    1,
    { 0,0 },
    { 0, 0, 0, 1, 1, 0 }
};

// Declare functions prototypes that are called when menu items are 
// clicked on and are used in the structs that defines the menus below.
void geosSwitch4080(void);
void geosExit (void);
void settingNTPOn (void);
void settingHostname (void);
void settingUTCOffset (void);
void settingVerbose (void);
void settingOverride (void);
void settingTargetAuto (void);
void settingTargetA (void);
void settingTargetB (void);
void settingTargetC (void);
void settingTargetD (void);
void settingDriveA (void);
void settingDriveB (void);
void settingDriveC (void);
void settingDriveD (void);
void informationCredits (void);

// Menu structures with pointers to the menu handlers above
static struct menu menuGEOS = {
    // GEOS
        { 15, 15+3*14, 0, 100 },
        3 | VERTICAL,
          {
            { "Switch 40/80", MENU_ACTION, geosSwitch4080 },
            { "Credits", MENU_ACTION, informationCredits },
            { "Exit", MENU_ACTION, geosExit }
          }
 };

static struct menu menuTime = {
    // Time
        { 15, 15+4*14, 35, 135 },
        4 | VERTICAL,
          {
            { "Enable NTP", MENU_ACTION, settingNTPOn },
            { "Hostname", MENU_ACTION, settingHostname },
            { "UTC offset", MENU_ACTION, settingUTCOffset },
            { "Verbose", MENU_ACTION, settingVerbose }
          }
 };

static struct menu menuDrives = {
    // Time
        { 15, 15+4*14, 170, 225 },
        4 | VERTICAL,
          {
            { "Drive A", MENU_ACTION, settingDriveA },
            { "Drive B", MENU_ACTION, settingDriveB },
            { "Drive C", MENU_ACTION, settingDriveC },
            { "Drive D", MENU_ACTION, settingDriveD }
          }
 };

static struct menu menuTarget = {
    // Time
        { 15, 15+5*14, 170, 225 },
        5 | VERTICAL,
          {
            { "Auto", MENU_ACTION, settingTargetAuto },
            { "Drive A", MENU_ACTION, settingTargetA },
            { "Drive B", MENU_ACTION, settingTargetB },
            { "Drive C", MENU_ACTION, settingTargetC },
            { "Drive D", MENU_ACTION, settingTargetD }
          }
 };

static struct menu menuMount = {
    // Time
        { 15, 15+3*14, 70, 170 },
        3 | VERTICAL,
          {
            { "Enable override", MENU_ACTION, settingOverride },
            { "Set drive", SUB_MENU, &menuDrives},
            { "Set target", SUB_MENU, &menuTarget},
          }
 };

static struct menu menuMain = {
    // Main menu
        { 0, 15, 0, 125 },
        3 | HORIZONTAL,
          {
            { "GEOS", SUB_MENU, &menuGEOS},
            { "Time", SUB_MENU, &menuTime},
            { "Mount", SUB_MENU, &menuMount}
          }
 };

// Application functions

unsigned char LoadFileHeader() {
// Load GeoUMount file header

    // Search for application file
    if(FindFile("GeoUMount") == FILE_NOT_FOUND) {
        return 1;
    }

    // Load file header information to memory
    if(GetFHdrInfo(&dirEntryBuf)) {
        return 1;
    }
    return 0;
}

unsigned char SaveFileHeader() {
// Save the fileheader again with updated config data

    // First load file header again
    if(LoadFileHeader()) {
        return 1;
    }

    // Place updated config in fileheader memory
    fileHeader.author[40] = mountconfig.auto_override;
    fileHeader.author[41] = mountconfig.valid[0];
    fileHeader.author[42] = mountconfig.valid[1];
    fileHeader.author[43] = mountconfig.valid[2];
    fileHeader.author[44] = mountconfig.valid[3];
    fileHeader.author[45] = mountconfig.target;

    // Save fileheader block back to disk
    // char PutBlock (struct tr_se *myTrSe, char *buffer)
    if(PutBlock(&dirEntryBuf.header,(char*)&fileHeader)) {
        return 1;
    }

    return 0;
}

void PrintConfigdata(unsigned char refresh) {
// Print the present config data

    unsigned char drive;

    // Clear previous text if refresh flag is set
    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(21,169,5,screen_pixel_width-1);
        Rectangle();
    }

    // Print GeoUMount config data
    if(mount_notfound) {
        PutString("GeoUMount application not found.",29,5);
    } else {
        PutString(CBOLDON "GeoUMount config data:" CPLAINTEXT,29,5);
        sprintf(buffer,"Detection override: %s",mountconfig.auto_override?"Yes":"No");
        PutString(buffer,39,5);

        switch (firmwareflag)
        {
        case 0:
            sprintf(buffer,"Autodetection of valid drives succeeded.");
            break;

        case 1:
            sprintf(buffer,"Autodetection might be incorrect.");
            break;

        case 2:
            sprintf(buffer,"Autodetection not supported in firmware.");
            break;
        
        default:
            break;
        }
        PutString(buffer,49,5);

        for(drive=0;drive<4;drive++) {
            sprintf(buffer,"Drive %c: %s",drive+'A',drivetypes[validdrive[drive]]);
            PutString(buffer,59+(10*drive),5);
        }

        if(targetdrive) {
            sprintf(buffer,"Target drive: %c",targetdrive-1+'A');
        } else {
            sprintf(buffer,"No valid target drive set");
        }

        PutString(buffer,99,5);
    }

    // Print GeoUTime config data
    PutString(CBOLDON "GeoUTime Config data:" CPLAINTEXT,119,5);
    sprintf(buffer,"Hostname: %s",host);
    PutString(buffer,129,5);
    sprintf(buffer,"UTC offset: %ld",secondsfromutc);
    PutString(buffer,139,5);
    sprintf(buffer,"Update from NTP: %sabled",ntpon?"En":"Dis");
    PutString(buffer,149,5);
    sprintf(buffer,"Verbose: %sabled",verbose?"En":"Dis");
    PutString(buffer,159,5);
}

// Menu functions

void geosSwitch4080 (void) {
// Switch between 40 and 80 column mode

    SetNewMode();
    ReinitScreen(appname);
    GotoFirstMenu();
    DoMenu(&menuMain);
    PrintConfigdata(0);
    return;
}

void geosExit (void) {
// Exit to desktop

    ReDoMenu();
    // Ask confirnation
    if (DlgBoxOkCancel("Exit to desktop", "Are you sure?") == OK)
    {
        EnterDeskTop();
    }
    else
    {
        return;
    }  
}

void settingNTPOn() {
// Set flag for enabling NYP query

    ReDoMenu();
    GotoFirstMenu();

    sprintf(buffer,"Present value is: %s",(ntpon)?"Yes":"No");

    if(DlgBoxYesNo("Do you want to enable NTP query?",buffer) == YES) {
        ntpon = 1;
    }
    else
    {
        ntpon = 0;
    }

   ConfigSave();
   PrintConfigdata(1);

    return;
}

void settingHostname (void) {
// Change hostname setting by user input

    ReDoMenu();
    GotoFirstMenu();

    if(DlgBoxGetString(host,80,"Enter NTP server hostname","(Cancel to keep present}") != CANCEL) {
        ConfigSave();
        PrintConfigdata(1);
    };
    return;
}

void settingUTCOffset (void) {
// Change UTC offset setting by user input

    char offsetinput[10] = "";
    char* ptrend;

    ReDoMenu();
    GotoFirstMenu();

    sprintf(offsetinput,"%ld",secondsfromutc);
    if(DlgBoxGetString(offsetinput,9,"Enter UTC offset in seconds","(Cancel to keep present}") != CANCEL) {
        secondsfromutc = strtol(offsetinput,&ptrend,10);
        ConfigSave();
        PrintConfigdata(1);
    }
    return;
}

void settingVerbose () {
// Change verbosity setting

    ReDoMenu();
    GotoFirstMenu();

    sprintf(buffer,"Present value is: %s",(verbose)?"Yes":"No");

    if(DlgBoxYesNo("Do you want to enable verbosity?",buffer) == YES) {
        verbose = 1;
    }
    else
    {
        verbose = 0;
    }

    ConfigSave();
    PrintConfigdata(1);

    return;
}

void settingOverride() {
// Set flag for enabling override of automatic drive detection

    ReDoMenu();
    GotoFirstMenu();

    sprintf(buffer,"Present value is: %s",(mountconfig.auto_override)?"Yes":"No");

    if(DlgBoxYesNo("Do you want to enable override?",buffer) == YES) {
        mountconfig.auto_override = 1;
    }
    else
    {
        mountconfig.auto_override = 0;
    }

    SaveFileHeader();
    SetValidDrives();
    PrintConfigdata(1);

    return;

}

void SetTargetGeneric() {
// Generic handling of setting drive target

    ReDoMenu();
    GotoFirstMenu();

    if(mountconfig.target) {
        if(!validdrive[mountconfig.target-1]) {
            DlgBoxOk("Drive is no valid target","Press OK.");
            mountconfig.target = targetdrive;
            return;
        }
    }

    SaveFileHeader();
    SetValidDrives();
    PrintConfigdata(1);

    return;
}

void settingTargetAuto() {
// Let target be auto detected as first valid drive

    mountconfig.target = 1;
    SetTargetGeneric();

}

void settingTargetA() {
// Set target as drive A

    mountconfig.target = 1;
    SetTargetGeneric();
}

void settingTargetB() {
// Set target as drive B

    mountconfig.target = 2;
    SetTargetGeneric();

}

void settingTargetC() {
// Set target as drive C

    mountconfig.target = 3;
    SetTargetGeneric();
}

void settingTargetD() {
// Set target as drive D

    mountconfig.target = 4;
    SetTargetGeneric();
}

void SetDrivesGeneric(unsigned char drive) {
// Generic handling of setting drive target

    ReDoMenu();
    GotoFirstMenu();

    sprintf(buffer,"Set drive %c as valid?",'A'+drive);
    if(DlgBoxYesNo(buffer,"Click Yes or No.") == YES) {
        mountconfig.valid[drive] = drivetypeID[drive];
    } else {
        mountconfig.valid[drive] = 0;
    }

    if(mountconfig.valid[drive]>3) {
        switch (mountconfig.valid[drive])
        {
        case 129:
            mountconfig.valid[drive] = 4;
            break;
        
        case 130:
            mountconfig.valid[drive] = 5;
            break;

        case 131:
            mountconfig.valid[drive] = 6;
            break;

        case 132:
            mountconfig.valid[drive] = 7;
            break;
        
        default:
            mountconfig.valid[drive] = 0;
            break;
        }
    }

    SaveFileHeader();
    SetValidDrives();
    PrintConfigdata(1);

    return;
}

void settingDriveA() {
// Toggle drive A as target

    SetDrivesGeneric(0);
}

void settingDriveB() {
// Toggle drive A as target

    SetDrivesGeneric(1);
}

void settingDriveC() {
// Toggle drive A as target

    SetDrivesGeneric(2);
}

void settingDriveD() {
// Toggle drive A as target

    SetDrivesGeneric(3);

}

void informationCredits (void) {
// Show credits
    
    unsigned char xcoord;

    ReDoMenu();
    GotoFirstMenu();

    // Create dialogue window
    xcoord = CreateWindow();

    // Print credits
    PutString(CBOLDON "GeoUConfig" CPLAINTEXT,69,xcoord);
    PutString("Settings configuration utility for GeoUTools",79,xcoord);
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
    CopyString(appname,"   GeoUTime Cfg   ");

    // Set version number in string variable
    sprintf(version,
            "v%2i.%2i - %c%c%c%c%c%c%c%c-%c%c%c%c",
            VERSION_MAJOR, VERSION_MINOR,
            BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
            BUILD_MONTH_CH0, BUILD_MONTH_CH1, BUILD_DAY_CH0, BUILD_DAY_CH1,
            BUILD_HOUR_CH0, BUILD_HOUR_CH1, BUILD_MIN_CH0, BUILD_MIN_CH1);

    // Get host OS type
    osType = get_ostype();

    // Read config data GeoUmount
    mount_notfound = LoadFileHeader();
    if(!mount_notfound) { ReadConfigdata(); }

    // Load config file GeoUTime
    ConfigLoad();

    // Attempt auto drive detection
    SetValidDrives();

    // Initialize the screen after program startup
    ReinitScreen(appname);
    DoMenu(&menuMain);
    DoIcons(icons);

    PrintConfigdata(0);

    
    // Never returns    
    MainLoop();
}