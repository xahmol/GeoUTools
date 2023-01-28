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
#include "time_common.h"
#include "interface.h"

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

static struct menu menuSetting = {
    // Time
        { 15, 15+4*15, 35, 135 },
        4 | VERTICAL,
          {
            { "Enable NTP", MENU_ACTION, settingNTPOn },
            { "Hostname", MENU_ACTION, settingHostname },
            { "UTC offset", MENU_ACTION, settingUTCOffset },
            { "Verbose", MENU_ACTION, settingVerbose },
          }
 };

static struct menu menuInformation = {
    // Information
        { 15, 15+1*15, 70, 120 },
        1 | VERTICAL,
          {
            { "Credits", MENU_ACTION, informationCredits },
          }
 };

static struct menu menuMain = {
    // Main menu
        { 0, 15, 0, 150 },
        3 | HORIZONTAL,
          {
            { "GEOS", SUB_MENU, &menuGEOS},
            { "Time", SUB_MENU, &menuSetting},
            { "Information", SUB_MENU, &menuInformation }
          }
 };

// Menu functions

void geosSwitch4080 (void) {
// Switch between 40 and 80 column mode

    SetNewMode();
    ReinitScreen(appname);
    GotoFirstMenu();
    DoMenu(&menuMain);
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

    sprintf(buffer,"Present value is: %s",(ntpon)?"Yes":"No");

    if(DlgBoxYesNo("Do you want to enable NTP query?",buffer) == YES) {
        ntpon = 1;
    }
    else
    {
        ntpon = 0;
    }

   ConfigSave();

return;
}

void settingHostname (void) {
// Change hostname setting by user input

    ReDoMenu();

    if(DlgBoxGetString(host,80,"Enter NTP server hostname","(Cancel to keep present}") != CANCEL) {
        ConfigSave();
    };
    return;
}

void settingUTCOffset (void) {
// Change UTC offset setting by user input

    char offsetinput[10] = "";
    char* ptrend;

    ReDoMenu();

    sprintf(offsetinput,"%ld",secondsfromutc);
    if(DlgBoxGetString(offsetinput,9,"Enter UTC offset in seconds","(Cancel to keep present}") != CANCEL) {
        secondsfromutc = strtol(offsetinput,&ptrend,10);
        ConfigSave();
    }
    return;
}

void settingVerbose () {
// Change verbosity setting

    ReDoMenu();

    sprintf(buffer,"Present value is: %s",(verbose)?"Yes":"No");

    if(DlgBoxYesNo("Do you want to enable verbosity?",buffer) == YES) {
        verbose = 1;
    }
    else
    {
        verbose = 0;
    }

   ConfigSave();

    return;
}

void informationCredits (void) {
// Show credits
    
    unsigned char xcoord;

    ReDoMenu();

    // Create dialogue window
    xcoord = CreateWindow();

    // Print credits
    PutString(CBOLDON "GeoUTime - Config program" CPLAINTEXT,69,xcoord);
    PutString("Settings configuration utility for GeoUTime",79,xcoord);
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

    // Load config file
    ConfigLoad();

    // Initialize the screen after program startup
    ReinitScreen(appname);
    DoMenu(&menuMain);
    DoIcons(icons);
    
    // Never returns    
    MainLoop();
}