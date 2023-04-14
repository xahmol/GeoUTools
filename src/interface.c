/*
GeoUTools
GUI interface elements
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
#include "interface.h"

// Global variables
char version[25];
unsigned int screen_pixel_width;
unsigned char osType = 0;
unsigned char vdc = 0;
char appname[21];
unsigned char windowflag = 0;

// Window definitions
struct window *winMain;
struct window *winHeader;
struct window *winInterface;
struct window *winIntShadow;
struct window *winIntRecover;

struct window vic_winHdr = {0, 14, 200, SC_PIX_WIDTH-1 };
struct window vic_winMain = {15, SC_PIX_HEIGHT-1, 0, SC_PIX_WIDTH-1};
struct window vic_winInt = {50,150,10,310};
struct window vic_winShd = {58,158,18,318};
struct window vic_winRec = {50,158,10,318};

struct window vdc_winHdr = {0, 14, 400, SCREENPIXELWIDTH-1 };
struct window vdc_winMain = {15, SC_PIX_HEIGHT-1, 0, SCREENPIXELWIDTH-1};
struct window vdc_winInt = {50,150,120,520};
struct window vdc_winShd = {58,158,136,536};
struct window vdc_winRec = {50,158,120,536};

// Icons
char IconOK[] = {
	0x05, 0xFF, 0x82, 0xFE, 0x80, 0x04, 0x00, 0x82,
	0x03, 0x80, 0x04, 0x00, 0xB8, 0x03, 0x80, 0x00,
	0xF8, 0xC6, 0x00, 0x03, 0x80, 0x01, 0x8C, 0xCC,
	0x00, 0x03, 0x80, 0x01, 0x8C, 0xD8, 0x00, 0x03,
	0x80, 0x01, 0x8C, 0xF0, 0x00, 0x03, 0x80, 0x01,
	0x8C, 0xE0, 0x00, 0x03, 0x80, 0x01, 0x8C, 0xF0,
	0x00, 0x03, 0x80, 0x01, 0x8C, 0xD8, 0x00, 0x03,
	0x80, 0x01, 0x8C, 0xCC, 0x00, 0x03, 0x80, 0x00,
	0xF8, 0xC6, 0x00, 0x03, 0x80, 0x04, 0x00, 0x82,
	0x03, 0x80, 0x04, 0x00, 0x81, 0x03, 0x06, 0xFF,
	0x81, 0x7F, 0x05, 0xFF
};

struct icontab *icons;
struct icontab *mainicons;
struct icontab *winOK;

struct icontab noicons = {
    1,
    { 0,0 },
    { 0, 0, 0, 1, 1, 0 }
};

struct icontab vic_winOK = {
    1,
    { 0,0 },
    { IconOK, 31, 130, 6, 16, (int)CloseWindow }
};

struct icontab vdc_winOK = {
    1,
    { 0,0 },
    { IconOK, 58, 130, 6, 16, (int)CloseWindow }
};

// Screen functions

void SetRectangleCoords(unsigned char top, unsigned char bottom, unsigned int left, unsigned int right) {
// Set the co-ordinates to use for rectangle functions

    drawWindow.top = top;
    drawWindow.bot = bottom;
    drawWindow.left = left;
    drawWindow.right = right;
}

int StringLength (char* inputstring) {
// Return the length of a string in pixels in the presently used font

    unsigned char x;
    unsigned char lengthchar = strlen(inputstring);
    unsigned int lengthpixel = 0;

    for(x=0;x<lengthchar;x++) { lengthpixel += GetCharWidth(inputstring[x]); }

    return lengthpixel;
}

void PutStringCentered(char* inputstring, unsigned char bottom, unsigned int left, unsigned int right) {
// Print a string centered in the given area co-ordinates

    unsigned int width = StringLength(inputstring);
    if(right-left>width)
    {
        PutString(inputstring,bottom,((left+right)/2)-(width/2));
    }
}

void PutStringRight(char* inputstring, unsigned char bottom, unsigned int left, unsigned int right) {
// Print a string right aligned in the given area co-ordinates

    unsigned int width = StringLength(inputstring);
    if(right-left>width)
    {
        PutString(inputstring,bottom,right-width);
    }
}

int CreateWindow() {
// Creates a window at the default co-ordinates given the mode (40/80 column) used
// Returns the x co-ordinate for output to this window

    unsigned int xcoord = 0;

    // Disable menus to avoid unwanted interference
    mouseOn = SET_MSE_ON + SET_ICONSON;

    // Set co-ordinates based on which screen mode (40/80) is used
	if (vdc)
	{
		// == 0x80 - 80 col mode
		winInterface = &vdc_winInt;
        winIntShadow = &vdc_winShd;
        winIntRecover = &vdc_winRec;
        winOK = &vdc_winOK;
        xcoord = 130;
	}
    else
    {
        winInterface = &vic_winInt;
        winIntShadow = &vic_winShd;
        winIntRecover = &vic_winRec;
        winOK = &vic_winOK;
        xcoord = 20;
    }

    // Draw window, shadow and frame
    dispBufferOn = ST_WR_FORE;
    SetPattern(1);
    InitDrawWindow (winIntShadow);
    Rectangle();  // Shadow
    SetPattern(0);
    InitDrawWindow (winInterface);
    Rectangle();  // Window
    FrameRectangle(255);    // Frame

    // Set window flag so other mouse clicks know not to react
    windowflag = 1;

    // Returns the x co-ordinate for output to this window
    return xcoord;
}

void WinOKButton() {
// Shows OK button to end a dialogue window

    // Shows OK icon and enables icons
    icons = winOK;
    DoIcons(icons);
}

void CloseWindow() {
// Closes the last opened window and recovers background

    // Enable menus again
    mouseOn = SET_MSE_ON + SET_MENUON + SET_ICONSON;

    // Clear OK icon
    icons = mainicons;
    DoIcons(icons);

    // Recover background
    InitDrawWindow (winIntRecover);
    dispBufferOn = ST_WR_FORE + ST_WR_BACK;
    RecoverRectangle();

    // Clear window flag
    windowflag = 0;
}

void ReinitScreen(char *s) {
// Initialise main screen
// Input:   App name as string s
	
	unsigned short hdrY = 0;

    // Set co-ordinates based on which screen mode (40/80) is used
	if ((osType & GEOS64) == GEOS64) // c64
	{
		screen_pixel_width = SC_PIX_WIDTH;
		winHeader = &vic_winHdr;
		winMain = &vic_winMain;
		hdrY = 200;
		vdc = 0;
        mainicons = &vic_mainicons;
	}
	
	if ((osType & GEOS128) == GEOS128) // c128
	{
		if((graphMode & 0x80) == 0x00)
		{
			// 40 col mode
			screen_pixel_width = SC_PIX_WIDTH;
			winHeader = &vic_winHdr;
			winMain = &vic_winMain;
			hdrY = 200;
			vdc = 0;
            mainicons = &vic_mainicons;
		}
		else if((graphMode & 0x80) == 0x80)
		{
			// == 0x80 - 80 col mode
			screen_pixel_width = SCREENPIXELWIDTH;
			winHeader = &vdc_winHdr;
			winMain = &vdc_winMain;
			hdrY = 400;
			vdc = 1;
            mainicons = &vdc_mainicons;
		}
	}

    // Set icons
    icons = mainicons;
	
    // Clear main screen
    SetPattern(2);
    SetRectangleCoords(0,SC_PIX_HEIGHT-1,0,screen_pixel_width-1);
    Rectangle();
	SetPattern(0);
	InitDrawWindow (winMain);
	Rectangle();
    FrameRectangle(255);

    // Draw header
	SetPattern(0);
	InitDrawWindow (winHeader);
	Rectangle();
	HorizontalLine(255, 1, hdrY, screen_pixel_width-1);
	HorizontalLine(255, 4, hdrY, screen_pixel_width-1);
	HorizontalLine(255, 6, hdrY, screen_pixel_width-1);
	HorizontalLine(255, 8, hdrY, screen_pixel_width-1);
	HorizontalLine(255, 10, hdrY, screen_pixel_width-1);
	HorizontalLine(255, 13, hdrY, screen_pixel_width-1);
	
    // Draw application name
	UseSystemFont();
	PutStringCentered (s, 9, hdrY,screen_pixel_width-1);
}