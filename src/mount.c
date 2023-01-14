/*
GeoUMount
Disk mounter for the Ultimate II+ in GEOS
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
#include <peekpoke.h>
#include "defines.h"
#include "interface.h"
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

// Global variables
char buffer[81];

// Interface icontab
struct icontab interfaceicons = {
    1,
    { 0,0 },
    { 0, 0, 0, 1, 1, 0 }
};

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

// Screen functions

unsigned char CheckStatus() {
// Function to check UII+ status and print error box if applicable

    if (!uii_success()) {
        DlgBoxOk("Ultimate Command Interface error!",(const char *)uii_status);
        return 1;
    }
    return 0;
}

void DrawIDandPath(unsigned char refresh) {
// Draw UCI ID and pathname. Clear previous if refresh flag set

    // Clear previous text if refresh flag is set
    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(20,40,5,screen_pixel_width-5);
        Rectangle();
    }
    
    // Get ID from UCI and print
    InitForIO();
    uii_identify();
	DoneWithIO();

    sprintf(buffer,"ID: %s",uii_data);
    PutString(buffer,29,5);

    // Get present path from UCI and print
    InitForIO();
    uii_get_path();
	DoneWithIO();

    sprintf(buffer,"Path: %s",uii_data);
    PutString(buffer,39,5);
}

void DrawDir() {
// Draw the dirlisting

    InitForIO();
    uii_open_dir();

    if(uii_success()) {
        
    }
}

void DrawFilebrowser() {
// Draw file browser interface
    
    // Initialize DOS target of Ultimate Command Interface
    InitForIO();
	uii_settarget(TARGET_DOS1);
    if(uii_isdataavailable())
	{
		uii_abort();
	}
	DoneWithIO();

    // Print ID and present path
    DrawIDandPath(0);
}

// Application routines


// Menu functions

void geosSwitch4080 (void) {
// Switch between 40 and 80 column mode

    SetNewMode();
    ReinitScreen(appname);
    DrawFilebrowser();
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

    // Initialize the screen after program startup
    mainicons = &noicons;
    icons = mainicons;
    ReinitScreen(appname);
    DoMenu(&menuMain);
    DoIcons(icons);
    DrawFilebrowser();
    
    // Never returns    
    MainLoop();
}