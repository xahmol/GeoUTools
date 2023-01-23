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

-   ntp2ultimate by MaxPlap: code for obtaining time via NTP
    https://github.com/MaxPlap/ntp2ultimate
   
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
//#include <conio.h>
#include "defines.h"
#include "interface.h"
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

// Global variables
char buffer[81];
unsigned char validdrive[4] = {0,0,0,0};
char drivetypes[4][14] = {
    "No target",
    "Emulated 1541",
    "Emulated 1571",
    "Emulated 1581"
};
char entrytypes[5][4] = {
    "DIR",
    "D64",
    "D71",
    "D81",
    "!TL"
};
char drivetypeID[4];
unsigned char targetdrive;

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

// Dir travel icons of 4 cards and 9 lines
char iconBack[] = {
    0x04,0xFF,0x9C,0x80,0x00,0x00,0x01,0xB8,0xC6,0x48,0x01,0xA5,0x29,0x50,0x01,0xB9,
    0xE8,0x60,0x01,0xA5,0x29,0x50,0x01,0xB9,0x26,0x48,0x01,0x80,0x00,0x00,0x01,0x04,
    0xFF
};

char iconRoot[] = {
    0x04,0xFF,0x9C,0x80,0x00,0x00,0x01,0xB8,0xC6,0x7C,0x01,0xA5,0x29,0x10,0x01,0xB9,
    0x29,0x10,0x01,0xA5,0x29,0x10,0x01,0xA4,0xC6,0x10,0x01,0x80,0x00,0x00,0x01,0x04,
    0xFF
};

char iconTop[] = {
    0x04,0xFF,0x9C,0x80,0x00,0x00,0x01,0xBE,0x67,0x00,0x01,0x88,0x94,0x80,0x01,0x88,
    0x97,0x00,0x01,0x88,0x94,0x00,0x01,0x88,0x64,0x00,0x01,0x80,0x00,0x00,0x01,0x04,
    0xFF
};

char iconBottom[] = {
    0x04,0xFF,0x9C,0x80,0x00,0x00,0x01,0xB8,0xCE,0xE6,0x45,0xA5,0x24,0x49,0x6D,0xB9,
    0x24,0x49,0x55,0xA5,0x24,0x49,0x45,0xB8,0xC4,0x46,0x45,0x80,0x00,0x00,0x01,0x04,
    0xFF
};

// Declare interface functions
void DriveselectA (void);
void DriveselectB (void);
void DriveselectC (void);
void DriveselectD (void);
void DirBack (void);
void DirRoot (void);
void DirTop (void);
void DirBottom (void);
void ScrollUp (void);
void ScrollDown (void);

// Interface icontab
struct icontab vic_mainicons = {
    10,
    { 0,0 },
    {
        { iconA, 28, 50, 2, 16, (int)DriveselectA },
        { iconB, 28, 70, 2, 16, (int)DriveselectB },
        { iconC, 28, 90, 2, 16, (int)DriveselectC },
        { iconD, 28, 110, 2, 16, (int)DriveselectD },
        { iconBack, 28, 130, 4, 9, (int)DirBack },
        { iconRoot, 28, 140, 4, 9, (int)DirRoot },
        { iconTop, 28, 150, 4, 9, (int)DirTop },
        { iconBottom, 28, 160, 4, 9, (int)DirBottom },
        { iconArrowUp, 25, 45, 1, 8, (int)ScrollUp },
        { iconArrowDown, 25, 180, 1, 8, (int)ScrollDown },
    }
};

struct icontab vdc_mainicons = {
    10,
    { 0,0 },
    {
        { iconA, 56, 50, 2, 16, (int)DriveselectA },
        { iconB, 56, 70, 2, 16, (int)DriveselectB },
        { iconC, 56, 90, 2, 16, (int)DriveselectC },
        { iconD, 56, 110, 2, 16, (int)DriveselectD },
        { iconBack, 56, 130, 4, 9, (int)DirBack },
        { iconRoot, 56, 140, 4, 9, (int)DirRoot },
        { iconTop, 56, 150, 4, 9, (int)DirTop },
        { iconBottom, 56, 160, 4, 9, (int)DirBottom },
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
struct intCoords vdc_intCoords = { 5,400,408,448 };

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

// Application routines
void SetValidDrives() {
// Initialise variables to defines which drive IDs are valid targets

    unsigned char drive;

    targetdrive = 0;

    // Get GEOS drive types
    CopyString(drivetypeID,(char *) (DRIVETYPES));

    // Initialize DOS target of Ultimate Command Interface
    InitForIO();
	uii_settarget(TARGET_DOS1);
    if(uii_isdataavailable())
	{
		uii_abort();
	}

    // Set dir at home dir
    uii_change_dir_home();

    // Get device info from UCI
    uii_get_deviceinfo();
	DoneWithIO(); 

    for(drive=0;drive<4;drive++)
    {
        validdrive[drive]=0;

        // Check if drive ID is an Ulimate emulated drive
        if(uii_data[2] == (drive + 8) || uii_data[5] == (drive+8) )
        {
            if(drivetypeID[drive]<4) {
                validdrive[drive] = drivetypeID[drive];
                if(!targetdrive) { targetdrive = drive+1; }
            }
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
    InitForIO();
    uii_open_dir();
    uii_get_dir();
    DoneWithIO();

    // Loop while dir data is available or memory is full
    while(uii_isdataavailable())
	{
        // Reset entry type
        presenttype = 0;

        // Get next dir entry
        InitForIO();
		uii_readdata();
		uii_accept();
        DoneWithIO();

        datalength = strlen(uii_data);

        // Check if entry is a dir by checking if bit 4 of first byte is set
        if(uii_data[0]&0x10) { presenttype=1; }

        // Check if file is a matching image type
        if(!presenttype) {

            // Check for filename extension of a disk image (D64, D71 or D81)
            if( (uii_data[datalength-3] == 'd' || uii_data[datalength-3] == 'D') &&
                (uii_data[datalength-2] == '6') &&
                (uii_data[datalength-1] == '4')) { presenttype =2; }
            if( (uii_data[datalength-3] == 'd' || uii_data[datalength-3] == 'D') &&
                (uii_data[datalength-2] == '7') &&
                (uii_data[datalength-1] == '1')) { presenttype =3; }
            if( (uii_data[datalength-3] == 'd' || uii_data[datalength-3] == 'D') &&
                (uii_data[datalength-2] == '8') &&
                (uii_data[datalength-1] == '1')) { presenttype =4; }

            // Check of identified image type matches selected target drive
            if( presenttype != drivetypeID[targetdrive-1]+1) { presenttype=0; }  
        }

        //DoneWithIO();
        //SetPattern(0);
        //SetRectangleCoords(150,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
        //Rectangle();
        //gotoxy(1,20);
        //cprintf("%d %d %d %s",presenttype,drivetypeID[targetdrive]+1,datalength,uii_data);

        if(presenttype) {
            // Get file or dir name to buffer
            maxlength = datalength;
            if(maxlength>20) {maxlength=20; presenttype=5; }    // Truncate for max 20
            memset(buffer,0,21);
            CopyFString(maxlength,buffer,uii_data+1);

            // Allocate memory for dir entry
            present = calloc(1, 26);

            // Break loop if memory is full
            if(!present) { break; }

            // Set direntry data
            presentdirelement = present;
            CopyString(presentdirelement->filename,buffer);
            presentdirelement->type = presenttype;

            //gotoxy(1,21);
            //cprintf("%d %4x %s %s %d",maxlength,present,buffer,presentdirelement->filename,presentdirelement->type);
//
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

        //cgetc();
        //InitForIO();
	}
}

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
        SetRectangleCoords(21,44,interfaceCoords->filelist_xstart+1,interfaceCoords->scroll_xend-1);
        Rectangle();
    }

    // Get ID from UCI and print
    InitForIO();
    uii_identify();
	DoneWithIO();

    sprintf(buffer,"ID: %s",uii_data);
    PutString(buffer,29,interfaceCoords->filelist_xstart+5);

    // Get present path from UCI and print
    InitForIO();
    uii_get_path();
	DoneWithIO();

    sprintf(buffer,"Path: %s",uii_data);
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

    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(20,30,interfaceCoords->righttab_xstart+1,screen_pixel_width-1);
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
}

void DrawDir(refresh) {
// Draw the dirlisting

    unsigned char printpos = 59;
    unsigned char type;
    struct DirElement* present;

    // Clear area on request
    if(refresh)
    {
        SetPattern(0);
        SetRectangleCoords(46,186,interfaceCoords->filelist_xstart+1,interfaceCoords->filelist_xend-1);
        Rectangle();
    }

    // Read directory contents
    Readdir();

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
            type = presentdirelement->type;
            PutString(presentdirelement->filename,printpos,interfaceCoords->filelist_xstart+5);
            PutString(entrytypes[type-1],printpos,interfaceCoords->filelist_xstart+165);

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

// Icon handlers
void DriveselectA() {
// Select drive A as target if valid as target

    if(validdrive[0])
    {
        targetdrive=1;
        DrawTargetdrive(1);
    }
}

void DriveselectB() {
// Select drive B as target if valid as target

    if(validdrive[1])
    {
        targetdrive=2;
        DrawTargetdrive(1);
    }
}

void DriveselectC() {
// Select drive C as target if valid as target

    if(validdrive[2])
    {
        targetdrive=3;
        DrawTargetdrive(1);
    }
}

void DriveselectD() {
// Select drive A as target if valid as target

    if(validdrive[3])
    {
        targetdrive=4;
        DrawTargetdrive(1);
    }
}

void DirBack() {
// Go back to parent dir

    InitForIO();
    uii_change_dir("..");
	DoneWithIO();
    DrawIDandPath(1);
    DrawDir(1);
}

void DirRoot() {
// Go back to root dir

    InitForIO();
    uii_change_dir("/");
	DoneWithIO();
    DrawIDandPath(1);
    DrawDir(1);
}

void DirTop() {

}

void DirBottom() {

}

void ScrollUp() {

}

void ScrollDown() {

}

// Mouse handler functions

void MountSelected(unsigned char filepos) {
// Check if there is a file at the click location and if yes, mount the file

    unsigned char count;
    struct DirElement* present;

    // Don't act on mouse button release
    if ((mouseData & MOUSE_BTN_DOWN) != 0) return;

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

    // If type is dir, change dir
    if(presentdirelement->type == 1) {
        // Change dir
        InitForIO();
        uii_change_dir(presentdirelement->filename);
        DoneWithIO();

        // Redraw filebrowser
        DrawIDandPath(1);
        DrawDir(1);
        return;
    }

    // Mount disk
    InitForIO();
    uii_mount_disk(targetdrive+7,presentdirelement->filename);
    DoneWithIO();

    // Error handling
    if(CheckStatus()) { return; }

    // Confirm and ask if program should be exited
    sprintf(buffer,"%s mounted on ID %d",presentdirelement->filename,targetdrive+7);
    if(DlgBoxOkCancel(buffer,"Exit program?") == OK)
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

    // Check if mouse was pressed in filebrowser area
    if(xpos>5 && xpos<interfaceCoords->filelist_xend && ypos>50 && ypos<186)
    {
        filepos = (ypos-50)/10;
        MountSelected(filepos);
        return;
    }

    // Check if mouse was pressed in scroll up area
    if(xpos>interfaceCoords->filelist_xend && xpos<interfaceCoords->scroll_xend && ypos>52 && ypos<116)
    {
        ScrollUp();
        return;
    }

    // Check if mouse was pressed in scroll down area
    if(xpos>interfaceCoords->filelist_xend && xpos<interfaceCoords->scroll_xend && ypos>116 && ypos<186)
    {
        ScrollDown();
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