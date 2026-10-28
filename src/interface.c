/*
GeoUTools
GUI interface elements
Written in 2023 by Xander Mol, Oscar64 version 2026

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
#include <geos.h>
#include "defines.h"
#include "interface.h"

char version[25];
unsigned screen_pixel_width;
char osType = 0;
char vdc = 0;
char appname[21];
char windowflag = 0;

const struct window *winMain;
const struct window *winHeader;
const struct window *winInterface;
const struct window *winIntShadow;
const struct window *winIntRecover;

const struct window vic_winHdr = {0, 14, 200, SC_PIX_WIDTH - 1};
const struct window vic_winMain = {15, SC_PIX_HEIGHT - 1, 0, SC_PIX_WIDTH - 1};
const struct window vic_winInt = {50, 150, 40, 279};
const struct window vic_winShd = {58, 158, 48, 287};
const struct window vic_winRec = {50, 158, 40, 287};

const struct window vdc_winHdr = {0, 14, 400, SCREENPIXELWIDTH - 1};
const struct window vdc_winMain = {15, SC_PIX_HEIGHT - 1, 0, SCREENPIXELWIDTH - 1};
const struct window vdc_winInt = {50, 150, 120, 520};
const struct window vdc_winShd = {58, 158, 136, 536};
const struct window vdc_winRec = {50, 158, 120, 536};

const char IconOK[] = {
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

const struct icontab *icons;
const struct icontab *mainicons;
const struct icontab *winOK;
static const struct icontab *vic_mainicons, *vdc_mainicons;

GEOS_CALLBACK(closewindow_cb, CloseWindow)

const GEOS_ICONTAB(1) noicons = { 1, {0, 0}, { {0, 0, 0, 1, 1, 0} } };
static const GEOS_ICONTAB(1) vic_winOK = { 1, {0, 0}, { {IconOK, 28, 130, 6, 16, closewindow_cb} } };
static const GEOS_ICONTAB(1) vdc_winOK = { 1, {0, 0}, { {IconOK, 52, 130, 6 | DOUBLE_B, 16, closewindow_cb} } };

void SetRectangleCoords(char top, char bottom, unsigned left, unsigned right)
{
	drawWindow.top = top;
	drawWindow.bot = bottom;
	drawWindow.left = left;
	drawWindow.right = right;
}

// Width of a string in pixels in the current font.
unsigned StringLength(const char *s)
{
	unsigned w = 0;
	while (*s)
		w += GetCharWidth(*s++);
	return w;
}

void PutStringCentered(const char *s, char bottom, unsigned left, unsigned right)
{
	unsigned width = StringLength(s);
	if (right - left > width)
		PutString(s, bottom, ((left + right) / 2) - (width / 2));
}

void PutStringRight(const char *s, char bottom, unsigned left, unsigned right)
{
	unsigned width = StringLength(s);
	if (right - left > width)
		PutString(s, bottom, right - width);
}

// Open the standard window (40 or 80 columns); returns the x coordinate
// for text in it.
unsigned CreateWindow(void)
{
	unsigned xcoord;

	// Disable menus while the window is open
	mouseOn = SET_MSE_ON + SET_ICONSON;

	if (vdc) {
		winInterface = &vdc_winInt;
		winIntShadow = &vdc_winShd;
		winIntRecover = &vdc_winRec;
		winOK = (const struct icontab *)&vdc_winOK;
		xcoord = 130;
	} else {
		winInterface = &vic_winInt;
		winIntShadow = &vic_winShd;
		winIntRecover = &vic_winRec;
		winOK = (const struct icontab *)&vic_winOK;
		xcoord = 45;
	}

	dispBufferOn = ST_WR_FORE;
	SetPattern(1);
	InitDrawWindow(winIntShadow);
	Rectangle();
	SetPattern(0);
	InitDrawWindow(winInterface);
	Rectangle();
	FrameRectangle(255);

	windowflag = 1;
	return xcoord;
}

// OK icon that closes the window.
void WinOKButton(void)
{
	icons = winOK;
	DoIcons(icons);
}

// Close the window: restore the main icons, menus and the background.
void CloseWindow(void)
{
	mouseOn = SET_MSE_ON + SET_MENUON + SET_ICONSON;
	icons = mainicons;
	DoIcons(icons);
	InitDrawWindow(winIntRecover);
	dispBufferOn = ST_WR_FORE + ST_WR_BACK;
	RecoverRectangle();
	windowflag = 0;
}

// Draw the main screen with the application name s for the current mode
// (C64, C128 40 or 80 columns); vicicons/vdcicons are the application's
// main icon tables for 40 and 80 columns.
void ReinitScreen(const char *s, const struct icontab *vicicons, const struct icontab *vdcicons)
{
	unsigned hdrY;

	vic_mainicons = vicicons;
	vdc_mainicons = vdcicons;

	if ((osType & GEOS128) && (graphMode & 0x80)) {
		screen_pixel_width = SCREENPIXELWIDTH;
		winHeader = &vdc_winHdr;
		winMain = &vdc_winMain;
		hdrY = 400;
		vdc = 1;
		mainicons = vdc_mainicons;
	} else {
		screen_pixel_width = SC_PIX_WIDTH;
		winHeader = &vic_winHdr;
		winMain = &vic_winMain;
		hdrY = 200;
		vdc = 0;
		mainicons = vic_mainicons;
	}
	icons = mainicons;

	SetPattern(2);
	SetRectangleCoords(0, SC_PIX_HEIGHT - 1, 0, screen_pixel_width - 1);
	Rectangle();
	SetPattern(0);
	InitDrawWindow(winMain);
	Rectangle();
	FrameRectangle(255);

	SetPattern(0);
	InitDrawWindow(winHeader);
	Rectangle();
	HorizontalLine(255, 1, hdrY, screen_pixel_width - 1);
	HorizontalLine(255, 4, hdrY, screen_pixel_width - 1);
	HorizontalLine(255, 6, hdrY, screen_pixel_width - 1);
	HorizontalLine(255, 8, hdrY, screen_pixel_width - 1);
	HorizontalLine(255, 10, hdrY, screen_pixel_width - 1);
	HorizontalLine(255, 13, hdrY, screen_pixel_width - 1);

	UseSystemFont();
	PutStringCentered(s, 9, hdrY, screen_pixel_width - 1);
}
