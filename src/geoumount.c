/*
GeoUMount
Disk image mounter for the Ultimate II+, Ultimate 64 and C64 Ultimate in
GEOS: browse the Ultimate's storage, mount images on emulated drives or
load them into GEOS RAM drives, save RAM drives and the REU to files.
Written in 2023 by Xander Mol, Oscar64 version 2026

https://github.com/xahmol/GeoUTools
https://www.idreamtin8bits.com/

Code and resources from others used:

-   Scott Hutter - Ultimate II+ library and GeoUTerm
    https://github.com/xlar54/ultimateii-dos-lib/
    (now via https://github.com/xahmol/ultimate-uci-oscar64)

-   Oscar64 by Dr. Mortal Wombat, https://github.com/drmortalwombat/oscar64

-   David Lee - GEOS Image Editor
    https://www.facebook.com/groups/704637082964003/permalink/5839146806179646
    (editor for GEOS icons)

-   Subchrist Software - SpritePad Pro
    https://subchristsoftware.itch.io/spritepad-c64-pro
    (editor used for the file icons)

-   Hitchhikers Guide to GEOS v2022, Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2022

-   Heureks: GEOS64 Programming Examples
    https://codeberg.org/heurekus/C64-Geos-Programming

-   Lyonlabs / Cenbe GEOS resources page:
    https://www.lyonlabs.org/commodore/onrequest/geos/index.html

-   Bart van Leeuwen for testing and providing the Device Manager ROM and
    GEOS RAM Boot

-   Gideon Zweijtzer for creating the Ultimate II+ cartridge and the
    Ultimate64, and the Ultimate Command Interface enabling this software.

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
#include "uci_io.h"
#include "interface.h"
#include "mount_common.h"
#include <ultimate_common_lib.h>
#include <ultimate_dos_lib.h>

// ---- directory list ---------------------------------------------------------

// Entry types (entrytypes[type - 1]): 1 dir, 2 D64, 3 D71, 4 D81, 5 DNP,
// 6 name too long, 7 DNP of the wrong size for the RAM drive
#define TYPE_DIR        1
#define TYPE_DNP        5
#define TYPE_TOOLONG    6
#define TYPE_BADSIZE    7

#define NAME_MAX        20
#define MAX_DIRENTRIES  250

struct DirElement {
	char type;
	char filename[NAME_MAX + 1];
	struct DirElement *next;
	struct DirElement *prev;
};

struct Directory {
	struct DirElement *firstelement;
	struct DirElement *lastelement;
	struct DirElement *firstprint;
	struct DirElement *lastprint;
};

// Fixed pool instead of the heap (GeoUTools 1.x used calloc/free)
static struct DirElement direntries[MAX_DIRENTRIES];
static unsigned direntry_count;
static struct Directory presentdir;

char buffer[81];

// ---- icons ------------------------------------------------------------------

// Select drive icons: 2 cards x 16 lines
static const char iconA[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x83,0x81,0x83,0x83,0xC3,0x83,0xC3,0x86,
	0x63,0x86,0x63,0x8F,0xF3,0x8C,0x33,0x8C,0x33,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconB[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x8F,0xC3,0x8C,0x63,0x8C,0x63,0x8F,0xC3,0x8C,
	0x63,0x8C,0x63,0x8C,0x63,0x8C,0x63,0x8F,0xC3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconC[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x87,0xC3,0x8C,0x63,0x8C,0x03,0x8C,0x03,0x8C,
	0x03,0x8C,0x03,0x8C,0x03,0x8C,0x63,0x87,0xC3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconD[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x8F,0x83,0x8C,0xC3,0x8C,0x63,0x8C,0x63,0x8C,
	0x63,0x8C,0x63,0x8C,0x63,0x8C,0xC3,0x8F,0x83,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
// Scroll arrows: 1 card x 8 lines
static const char iconArrowUp[] = { 0x88,0xFF,0x88,0x9C,0xBE,0x80,0xBE,0x80,0xFF };
static const char iconArrowDown[] = { 0x88,0xFF,0x80,0xBE,0x80,0xBE,0x9C,0x88,0xFF };
// Directory icons: 2 cards x 16 lines
static const char iconBack[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x03,0x83,0x03,0x87,0x03,0x8F,0xF3,0x9F,
	0xF3,0x8F,0xF3,0x87,0x03,0x83,0x03,0x81,0x03,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconRoot[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x80,0x33,0x80,0x73,0x80,0xE3,0x81,0xC3,0x83,
	0x83,0x87,0x03,0x8E,0x03,0x9C,0x03,0x98,0x03,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconHome[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x81,0x03,0x82,0x83,0x84,0x43,0x88,0x23,0x90,
	0x13,0x97,0x53,0x95,0x53,0x95,0x13,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};
static const char iconTop[] = {
	0x94,0xFF,0xFE,0x80,0x03,0x80,0x03,0x9F,0xF3,0x9F,0xF3,0x80,0x03,0x81,0x03,0x83,
	0x83,0x87,0xC3,0x8F,0xE3,0x04,0x83,0x88,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,0xFF
};
static const char iconBottom[] = {
	0x86,0xFF,0xFE,0x80,0x03,0x80,0x03,0x04,0x83,0x96,0x8F,0xE3,0x87,0xC3,0x83,0x83,
	0x81,0x03,0x80,0x03,0x9F,0xF3,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,0xFF
};
static const char iconSave[] = {
	0xA0,0xFF,0xFE,0x80,0x03,0x80,0x03,0x9F,0xE3,0x97,0x53,0x97,0x53,0x97,0x53,0x97,
	0xD3,0x90,0x13,0x97,0xD3,0x98,0x33,0x9F,0xF3,0x80,0x03,0x80,0x03,0xFF,0xFF,0x7F,
	0xFF
};

// Layout for 40 and 80 columns
struct intCoords {
	unsigned filelist_xstart;
	unsigned filelist_xend;
	unsigned scroll_xend;
	unsigned righttab_xstart;
};
static const struct intCoords *interfaceCoords;
static const struct intCoords vic_intCoords = { 5, 200, 208, 224 };
static const struct intCoords vdc_intCoords = { 5, 392, 408, 464 };

// ---- reading the directory ------------------------------------------------

static void Freedir(void)
{
	direntry_count = 0;
	presentdir.firstelement = presentdir.lastelement = NULL;
	presentdir.firstprint = presentdir.lastprint = NULL;
}

// Entry type of an image name, or 0 if it does not fit the target drive.
static char ImageType(const char *name, char len)
{
	char d = validdrive[targetdrive - 1];
	char e1, e2, e3;

	if (len < 5 || name[len - 4] != '.')
		return 0;
	e1 = name[len - 3] | 0x20;	// lower case
	e2 = name[len - 2] | 0x20;
	e3 = name[len - 1] | 0x20;
	if (e1 != 'd' && e1 != 'g')
		return 0;
	// D64/G64 on 1541, 1571 and RAM 1541
	if (e2 == '6' && e3 == '4' && (d == 1 || d == 2 || d == 4))
		return 2;
	// D71/G71 on 1571 and RAM 1571
	if (e2 == '7' && e3 == '1' && (d == 2 || d == 5))
		return 3;
	// D81 on 1581 and RAM 1581
	if (e2 == '8' && e3 == '1' && (d == 3 || d == 6))
		return 4;
	// DNP on RAM native
	if (e1 == 'd' && e2 == 'n' && e3 == 'p' && d == 7)
		return TYPE_DNP;
	return 0;
}

// Read the current UCI directory: subdirectories and images that fit the
// target drive.
static void Readdir(void)
{
	struct DirElement *e;
	char type, len;

	Freedir();

	enableIO();
	uii_open_dir();
	if (!UII_SUCCESS) {
		uii_abort();
		restoreIO();
		return;
	}
	uii_get_dir();
	while (uii_isdataavailable()) {
		uii_readdata();
		uii_accept();

		// uii_data[0] = attributes (bit 4: directory), then the name
		len = (char)strlen(uii_data + 1);
		type = (uii_data[0] & 0x10) ? TYPE_DIR : ImageType(uii_data + 1, len);
		if (!type)
			continue;

		if (direntry_count == MAX_DIRENTRIES) {
			uii_abort();
			break;
		}
		e = &direntries[direntry_count++];
		if (len > NAME_MAX) {
			len = NAME_MAX;
			type = TYPE_TOOLONG;
		}
		memcpy(e->filename, uii_data + 1, len);
		e->filename[len] = 0;
		e->type = type;
		e->next = NULL;
		e->prev = presentdir.lastelement;
		if (presentdir.lastelement)
			presentdir.lastelement->next = e;
		else
			presentdir.firstelement = presentdir.firstprint = e;
		presentdir.lastelement = e;
	}
	restoreIO();
}

// ---- drawing ----------------------------------------------------------------

static void DrawIDandPath(char refresh)
{
	unsigned maxlength = interfaceCoords->scroll_xend - 11;
	const char *p;

	if (refresh) {
		SetPattern(0);
		SetRectangleCoords(21, 44, interfaceCoords->filelist_xstart + 1, interfaceCoords->scroll_xend - 1);
		Rectangle();
	}

	enableIO();
	uii_identify();
	restoreIO();
	sprintf(buffer, "ID: %s", uii_data);
	PutString(buffer, 29, interfaceCoords->filelist_xstart + 5);

	// Path, shortened from the left until it fits
	enableIO();
	uii_get_path();
	restoreIO();
	p = uii_data;
	while (*p && StringLength(p) > maxlength)
		p++;
	PutString(p, 39, interfaceCoords->filelist_xstart + 5);
}

static void DrawDrivetypes(void)
{
	char drive;
	for (drive = 0; drive < 4; drive++)
		PutString(drivetypes[validdrive[drive]], 58 + 20 * drive, interfaceCoords->righttab_xstart + 20);
}

static void DrawTargetdrive(char refresh)
{
	if (refresh) {
		SetPattern(0);
		SetRectangleCoords(20, 30, interfaceCoords->righttab_xstart + 1, screen_pixel_width - 2);
		Rectangle();
	}
	if (targetdrive)
		sprintf(buffer, "Target is drive %c", targetdrive - 1 + 'A');
	else
		strcpy(buffer, "No valid target");
	PutString(buffer, 29, interfaceCoords->righttab_xstart);
}

static void PrintDirEntry(struct DirElement *e, char printpos)
{
	if (e->type == TYPE_DNP) {
		// Check that the DNP fits the RAM drive (load "what if")
		enableIO();
		uii_loadIntoRamDisk(targetdrive - 1, e->filename, 1);
		if (!UII_SUCCESS) {
			uii_abort();
			e->type = TYPE_BADSIZE;
		}
		restoreIO();
	}
	PutString(e->filename, printpos, interfaceCoords->filelist_xstart + 5);
	PutString(entrytypes[e->type - 1], printpos, interfaceCoords->filelist_xstart + 165);
}

// refresh: 0 = draw, 1 = clear, reread and draw, 2 = clear and draw
static void DrawDir(char refresh)
{
	char printpos = 59;
	struct DirElement *e;

	if (refresh) {
		SetPattern(0);
		SetRectangleCoords(46, 186, interfaceCoords->filelist_xstart + 1, interfaceCoords->filelist_xend - 1);
		Rectangle();
	}
	if (refresh != 2)
		Readdir();

	if (!presentdir.firstprint) {
		PutString("No data.", 59, interfaceCoords->filelist_xstart + 5);
		return;
	}
	for (e = presentdir.firstprint; e; e = e->next) {
		PrintDirEntry(e, printpos);
		presentdir.lastprint = e;
		printpos += 10;
		if (printpos >= 187)
			break;
	}
}

static void DrawFilebrowser(void)
{
	interfaceCoords = vdc ? &vdc_intCoords : &vic_intCoords;

	SetRectangleCoords(20, 187, interfaceCoords->filelist_xstart, interfaceCoords->scroll_xend);
	FrameRectangle(255);
	HorizontalLine(255, 45, interfaceCoords->filelist_xstart, interfaceCoords->filelist_xend);
	HorizontalLine(255, 116, interfaceCoords->filelist_xend, interfaceCoords->scroll_xend);
	VerticalLine(255, 45, 187, interfaceCoords->filelist_xend);

	DrawIDandPath(0);
	DrawTargetdrive(0);
	DrawDrivetypes();
	DrawDir(0);
}

// ---- scrolling ----------------------------------------------------------------

// In 80 columns the list is scrolled in the GEOS 128 background buffer
// (which spans $6000-$7F3F and $A000-$BF3F) and recovered to the screen;
// in 40 columns it is redrawn.
static void scroll_down(void)
{
	char t;

	if (!presentdir.lastprint->next)
		return;
	presentdir.lastprint = presentdir.lastprint->next;
	presentdir.firstprint = presentdir.firstprint->next;

	if (!vdc) {
		DrawDir(2);
		return;
	}
	for (t = 60; t < 100; t++)
		MoveData(BACK_SCR_BASE + (t - 10) * 80 + 1, BACK_SCR_BASE + t * 80 + 1, 49);
	for (t = 0; t < 10; t++)
		MoveData(BACK_SCR_BASE + (t + 90) * 80 + 1, SCREEN_BASE + t * 80 + 65, 49);
	for (t = 10; t < 82; t++)
		MoveData(SCREEN_BASE + (t - 10) * 80 + 65, SCREEN_BASE + t * 80 + 65, 49);
	SetRectangleCoords(50, 180, 8, interfaceCoords->filelist_xend - 1);
	RecoverRectangle();
	SetPattern(0);
	SetRectangleCoords(46, 49, 8, interfaceCoords->filelist_xend - 1);
	Rectangle();
	SetRectangleCoords(172, 186, 8, interfaceCoords->filelist_xend - 1);
	Rectangle();
	PrintDirEntry(presentdir.lastprint, 179);
}

static void scroll_up(void)
{
	signed char t;

	if (!presentdir.firstprint->prev)
		return;
	presentdir.firstprint = presentdir.firstprint->prev;
	presentdir.lastprint = presentdir.lastprint->prev;

	if (!vdc) {
		DrawDir(2);
		return;
	}
	for (t = 69; t > -1; t--)
		MoveData(SCREEN_BASE + (t + 10) * 80 + 65, SCREEN_BASE + t * 80 + 65, 49);
	for (t = 9; t > -1; t--)
		MoveData(SCREEN_BASE + t * 80 + 65, BACK_SCR_BASE + (t + 90) * 80 + 1, 49);
	for (t = 89; t > 47; t--)
		MoveData(BACK_SCR_BASE + (t + 10) * 80 + 1, BACK_SCR_BASE + t * 80 + 1, 49);
	SetRectangleCoords(50, 180, 8, interfaceCoords->filelist_xend - 1);
	RecoverRectangle();
	SetPattern(0);
	SetRectangleCoords(46, 59, 8, interfaceCoords->filelist_xend - 1);
	Rectangle();
	SetRectangleCoords(182, 186, 8, interfaceCoords->filelist_xend - 1);
	Rectangle();
	PrintDirEntry(presentdir.firstprint, 59);
}

static void page_down(void)
{
	if (!presentdir.lastprint->next)
		return;
	presentdir.firstprint = presentdir.lastprint->next;
	DrawDir(2);
}

static void page_up(void)
{
	char count;
	for (count = 0; count < 13 && presentdir.firstprint->prev; count++)
		presentdir.firstprint = presentdir.firstprint->prev;
	DrawDir(2);
}

// ---- icon handlers ------------------------------------------------------------

static void TargetChange(char newdrive)
{
	char oldtype = validdrive[targetdrive - 1];
	targetdrive = newdrive;
	DrawTargetdrive(1);
	if (oldtype != validdrive[newdrive - 1])
		DrawDir(1);
}

static void DriveselectA(void) { if (validdrive[0]) TargetChange(1); }
static void DriveselectB(void) { if (validdrive[1]) TargetChange(2); }
static void DriveselectC(void) { if (validdrive[2]) TargetChange(3); }
static void DriveselectD(void) { if (validdrive[3]) TargetChange(4); }

static void ChangeDir(const char *dir)
{
	enableIO();
	if (dir)
		uii_change_dir((char *)dir);
	else
		uii_change_dir_home();
	restoreIO();
	DrawIDandPath(1);
	DrawDir(1);
}

static void DirBack(void) { ChangeDir(".."); }
static void DirRoot(void) { ChangeDir("/"); }
static void DirHome(void) { ChangeDir(NULL); }

static void DirTop(void)
{
	if (presentdir.firstprint == presentdir.firstelement)
		return;
	presentdir.firstprint = presentdir.firstelement;
	DrawDir(2);
}

static void DirBottom(void)
{
	struct DirElement *e;
	char count;

	if (presentdir.lastprint == presentdir.lastelement)
		return;
	e = presentdir.lastelement;
	for (count = 0; count < 12 && e->prev; count++)
		e = e->prev;
	presentdir.firstprint = e;
	DrawDir(2);
}

static void ScrollUp(void)
{
	if (presentdir.firstprint != presentdir.firstelement)
		scroll_up();
}

static void ScrollDown(void)
{
	if (presentdir.lastprint != presentdir.lastelement)
		scroll_down();
}

// Ask for a file name (default def, extension ext), handle an existing file.
// Returns 0 if cancelled.
static char AskImageName(char *name, const char *ext)
{
	char len;

	if (DlgBoxGetString(name, 16, "Enter filename for the image", "(or press Cancel)") == CANCEL)
		return 0;
	len = (char)strlen(name);
	name[len] = '.';
	strcpy(name + len + 1, ext);

	enableIO();
	uii_open_file(1, name);
	if (UII_SUCCESS) {
		uii_close_file();
		restoreIO();
		if (DlgBoxYesNo("File exists.", "Overwrite file?") == NO)
			return 0;
		enableIO();
		uii_delete_file(name);
	} else
		uii_abort();
	restoreIO();
	return 1;
}

// Save the target RAM drive to an image file.
static void SaveImage(void)
{
	char imagename[24] = "ramdisk";

	if (validdrive[targetdrive - 1] < 4)
		return;
	if (!AskImageName(imagename, entrytypes[validdrive[targetdrive - 1] - 3]))
		return;
	enableIO();
	uii_saveRamDisk(targetdrive - 1, imagename);
	restoreIO();
	if (!CheckStatus()) {
		sprintf(buffer, "ID %d to %s", targetdrive + 7, imagename);
		DlgBoxOk("Image saved.", buffer);
		DrawDir(1);
	}
}

GEOS_CALLBACK(DriveselectA_cb, DriveselectA)
GEOS_CALLBACK(DriveselectB_cb, DriveselectB)
GEOS_CALLBACK(DriveselectC_cb, DriveselectC)
GEOS_CALLBACK(DriveselectD_cb, DriveselectD)
GEOS_CALLBACK(DirBack_cb, DirBack)
GEOS_CALLBACK(DirRoot_cb, DirRoot)
GEOS_CALLBACK(DirHome_cb, DirHome)
GEOS_CALLBACK(DirTop_cb, DirTop)
GEOS_CALLBACK(DirBottom_cb, DirBottom)
GEOS_CALLBACK(SaveImage_cb, SaveImage)
GEOS_CALLBACK(ScrollUp_cb, ScrollUp)
GEOS_CALLBACK(ScrollDown_cb, ScrollDown)

static const GEOS_ICONTAB(12) vic_mainicons = {
	12, { 0, 0 },
	{
		{ iconA, 28, 50, 2, 16, DriveselectA_cb },
		{ iconB, 28, 70, 2, 16, DriveselectB_cb },
		{ iconC, 28, 90, 2, 16, DriveselectC_cb },
		{ iconD, 28, 110, 2, 16, DriveselectD_cb },
		{ iconBack, 28, 130, 2, 16, DirBack_cb },
		{ iconRoot, 28, 150, 2, 16, DirRoot_cb },
		{ iconHome, 28, 170, 2, 16, DirHome_cb },
		{ iconTop, 31, 130, 2, 16, DirTop_cb },
		{ iconBottom, 31, 150, 2, 16, DirBottom_cb },
		{ iconSave, 31, 170, 2, 16, SaveImage_cb },
		{ iconArrowUp, 25, 45, 1, 8, ScrollUp_cb },
		{ iconArrowDown, 25, 180, 1, 8, ScrollDown_cb },
	}
};

static const GEOS_ICONTAB(12) vdc_mainicons = {
	12, { 0, 0 },
	{
		{ iconA, 56, 50, 2 | DOUBLE_B, 16, DriveselectA_cb },
		{ iconB, 56, 70, 2 | DOUBLE_B, 16, DriveselectB_cb },
		{ iconC, 56, 90, 2 | DOUBLE_B, 16, DriveselectC_cb },
		{ iconD, 56, 110, 2 | DOUBLE_B, 16, DriveselectD_cb },
		{ iconBack, 56, 130, 2 | DOUBLE_B, 16, DirBack_cb },
		{ iconRoot, 56, 150, 2 | DOUBLE_B, 16, DirRoot_cb },
		{ iconHome, 56, 170, 2 | DOUBLE_B, 16, DirHome_cb },
		{ iconTop, 61, 130, 2 | DOUBLE_B, 16, DirTop_cb },
		{ iconBottom, 61, 150, 2 | DOUBLE_B, 16, DirBottom_cb },
		{ iconSave, 61, 170, 2 | DOUBLE_B, 16, SaveImage_cb },
		{ iconArrowUp, 49, 45, 1 | DOUBLE_B, 8, ScrollUp_cb },
		{ iconArrowDown, 49, 180, 1 | DOUBLE_B, 8, ScrollDown_cb },
	}
};

// ---- clicks in the file list --------------------------------------------------

static void MountSelected(char filepos)
{
	struct DirElement *e = presentdir.firstprint;
	char count;

	for (count = 0; e && count < filepos; count++)
		e = e->next;
	if (!e)
		return;

	// Visual feedback
	SetRectangleCoords(50 + filepos * 10, 59 + filepos * 10, 5, interfaceCoords->filelist_xend);
	InvertRectangle();
	InvertRectangle();

	if (e->type > TYPE_DNP)
		return;
	if (e->type == TYPE_DIR) {
		ChangeDir(e->filename);
		return;
	}

	enableIO();
	if (!ramdiskID[(targetdrive - 1) * 2])
		uii_mount_disk(targetdrive + 7, e->filename);
	else
		uii_loadIntoRamDisk(targetdrive - 1, e->filename, 0);
	restoreIO();
	if (CheckStatus())
		return;

	sprintf(buffer, "%s mounted on ID %d", e->filename, targetdrive + 7);
	if (DlgBoxYesNo(buffer, "Exit program?") == YES)
		EnterDeskTop();
}

static void OtherMousePress(void)
{
	unsigned xpos = mouseXPos;
	char ypos = mouseYPos;

	if (windowflag)
		return;
	if (mouseData & MOUSE_BTN_DOWN)		// ignore the button release
		return;

	if (xpos > 5 && xpos < interfaceCoords->filelist_xend && ypos > 50 && ypos < 186) {
		MountSelected((ypos - 50) / 10);
		return;
	}
	if (xpos > interfaceCoords->filelist_xend && xpos < interfaceCoords->scroll_xend) {
		if (ypos > 52 && ypos < 116 && presentdir.firstprint != presentdir.firstelement)
			page_up();
		else if (ypos > 116 && ypos < 180 && presentdir.lastprint != presentdir.lastelement)
			page_down();
	}
}
GEOS_CALLBACK(OtherMousePress_cb, OtherMousePress)

// ---- menu actions ---------------------------------------------------------------

static void Redraw(void);

static void geosSwitch4080(void)
{
	GotoFirstMenu();
	if (!(c128Flag & 0x80)) {
		DlgBoxOk("40/80 columns", "Only on GEOS 128.");
		return;
	}
	graphMode ^= 0x80;	// HG2G: toggle bit 7, then SetNewMode
	SetNewMode();
	Redraw();
}

static void geosExit(void)
{
	GotoFirstMenu();
	if (DlgBoxOkCancel("Exit to desktop", "Are you sure?") == OK)
		EnterDeskTop();
}

// Save the REU to a file; size index 0-7 = 128 KB ... 16 MB.
static void saveREU(char sizeindex)
{
	char imagename[24] = "geosram";

	GotoFirstMenu();
	if (!AskImageName(imagename, "reu"))
		return;
	enableIO();
	uii_open_file(0x06, imagename);
	uii_save_reu(sizeindex);
	uii_close_file();
	restoreIO();
	if (!CheckStatus()) {
		sprintf(buffer, "%u KB to %s", 128u << sizeindex, imagename);
		DlgBoxOk("Image saved.", buffer);
		DrawDir(1);
	}
}

static void REUSave0(void) { saveREU(0); }
static void REUSave1(void) { saveREU(1); }
static void REUSave2(void) { saveREU(2); }
static void REUSave3(void) { saveREU(3); }
static void REUSave4(void) { saveREU(4); }
static void REUSave5(void) { saveREU(5); }
static void REUSave6(void) { saveREU(6); }
static void REUSave7(void) { saveREU(7); }

static void informationCredits(void)
{
	unsigned x;

	GotoFirstMenu();
	x = CreateWindow();
	PutString(CBOLDON "GeoUMount" CPLAINTEXT, 69, x);
	PutString("Disk mounter for the Ultimate in GEOS", 79, x);
	sprintf(buffer, "Version: %s", version);
	PutString(buffer, 99, x);
	PutString("Written by Xander Mol, 2023-2026", 109, x);
	PutString("For documentation, source, license and credits, see", 119, x);
	PutString("https://github.com/xahmol/GeoUTools", 129, x);
	PutString("https://www.idreamtin8bits.com/", 139, x);
	WinOKButton();
}

GEOS_CALLBACK(geosSwitch4080_cb, geosSwitch4080)
GEOS_CALLBACK(geosExit_cb, geosExit)
GEOS_CALLBACK(informationCredits_cb, informationCredits)
GEOS_CALLBACK(REUSave0_cb, REUSave0)
GEOS_CALLBACK(REUSave1_cb, REUSave1)
GEOS_CALLBACK(REUSave2_cb, REUSave2)
GEOS_CALLBACK(REUSave3_cb, REUSave3)
GEOS_CALLBACK(REUSave4_cb, REUSave4)
GEOS_CALLBACK(REUSave5_cb, REUSave5)
GEOS_CALLBACK(REUSave6_cb, REUSave6)
GEOS_CALLBACK(REUSave7_cb, REUSave7)

static const GEOS_MENU(3) menuGEOS = {
	{ 15, 15 + 3 * 14, 0, 100 }, 3 | VERTICAL,
	{
		{ "Switch 40/80", MENU_ACTION, geosSwitch4080_cb },
		{ "Credits", MENU_ACTION, informationCredits_cb },
		{ "Exit", MENU_ACTION, geosExit_cb },
	}
};

static const GEOS_MENU(8) menuREUSave = {
	{ 15, 15 + 8 * 14, 50, 100 }, 8 | VERTICAL,
	{
		{ "16 MB", MENU_ACTION, REUSave7_cb },
		{ "8 MB", MENU_ACTION, REUSave6_cb },
		{ "4 MB", MENU_ACTION, REUSave5_cb },
		{ "2 MB", MENU_ACTION, REUSave4_cb },
		{ "1 MB", MENU_ACTION, REUSave3_cb },
		{ "512 KB", MENU_ACTION, REUSave2_cb },
		{ "256 KB", MENU_ACTION, REUSave1_cb },
		{ "128 KB", MENU_ACTION, REUSave0_cb },
	}
};

static const GEOS_MENU(2) menuMain = {
	{ 0, 15, 0, 100 }, 2 | HORIZONTAL,
	{
		{ "GEOS", SUB_MENU, (const void *)&menuGEOS },
		{ "Save REU", SUB_MENU, (const void *)&menuREUSave },
	}
};

static void Redraw(void)
{
	ReinitScreen(appname, (const struct icontab *)&vic_mainicons, (const struct icontab *)&vdc_mainicons);
	DoMenu((const struct menu *)&menuMain);
	DoIcons(icons);
	DrawFilebrowser();
}

int main(void)
{
	strcpy(appname, "   GeoUMount   ");
	strcpy(version, VERSION);
	osType = get_ostype();

	ReadConfigdata();
	SetValidDrives();
	if (!targetdrive) {
		DlgBoxOk("No targets detected or set.", "Press OK to abort.");
		EnterDeskTop();
	}

	Redraw();
	otherPressVec = (void_func)OtherMousePress_cb;
	return 0;	// MainLoop runs the program
}
