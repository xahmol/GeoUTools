/*
GeoUTime - Config program
Utility to synch GEOS system time with NTP server and UII+ RTC clock
Written in 2023 by Xander Mol

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
#include "defines.h"
#include "time_common.h"
#include "interface.h"

// Declare functions prototypes that are called when menu items are 
// clicked on and are used in the structs that defines the menus below.
void geosSwitch4080(void);
void geosExit (void);
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
        { 15, 15+3*15, 35, 135 },
        3 | VERTICAL,
          {
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
    PutString("https://github.com/xahmol/GeoUMount",129,xcoord);
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
    mainicons = &noicons;
    icons = mainicons;
    ReinitScreen(appname);
    DoMenu(&menuMain);
    DoIcons(icons);
    
    // Never returns    
    MainLoop();
}