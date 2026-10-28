/*
GeoUConfig
Settings for GeoUTime (NTP server, UTC offset, verbosity) and GeoUMount
(drive detection override, valid drives, default target).
Written in 2023 by Xander Mol, Oscar64 version 2026

https://github.com/xahmol/GeoUTools
https://www.idreamtin8bits.com/

Code and resources from others used:

-   Scott Hutter - Ultimate II+ library and GeoUTerm
    https://github.com/xlar54/ultimateii-dos-lib/
    (now via https://github.com/xahmol/ultimate-uci-oscar64)

-   Oscar64 by Dr. Mortal Wombat, https://github.com/drmortalwombat/oscar64

-   Hitchhikers Guide to GEOS v2022, Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2022

-   Heureks: GEOS64 Programming Examples
    https://codeberg.org/heurekus/C64-Geos-Programming

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
#include <stdlib.h>
#include <geos.h>
#include "defines.h"
#include "mount_common.h"
#include "time_common.h"
#include "interface.h"

static char mount_notfound;

// One dummy icon (every GEOS application needs at least one)
static const GEOS_ICONTAB(1) cfgicons = { 1, {0, 0}, { {0, 0, 0, 1, 1, 0} } };

// ---- GeoUMount's file header ----------------------------------------------

// Load GeoUMount's file header into fileHeader; 0 if found.
static char LoadFileHeader(void)
{
	if (FindFile("GeoUMount"))
		return 1;
	if (GetFHdrInfo((const struct filehandle *)&dirEntryBuf))
		return 1;
	return 0;
}

// Write the settings into GeoUMount's file header block.
static char SaveFileHeader(void)
{
	struct tr_se hdr;

	if (LoadFileHeader())
		return 1;
	memcpy((char *)&fileHeader.author[MCONFIG_OFFSET], &mountconfig, sizeof(mountconfig));
	hdr.track = dirEntryBuf.header.track;
	hdr.sector = dirEntryBuf.header.sector;
	return PutBlock(&hdr, (const char *)&fileHeader);
}

// ---- display ------------------------------------------------------------

static void PrintConfigdata(char refresh)
{
	char drive;

	if (refresh) {
		SetPattern(0);
		SetRectangleCoords(21, 169, 5, screen_pixel_width - 1);
		Rectangle();
	}

	if (mount_notfound)
		PutString("GeoUMount application not found.", 29, 5);
	else {
		PutString(CBOLDON "GeoUMount config data:" CPLAINTEXT, 29, 5);
		sprintf(buffer, "Detection override: %s", mountconfig.auto_override ? "Yes" : "No");
		PutString(buffer, 39, 5);
		switch (firmwareflag) {
		case 0: PutString("Autodetection of valid drives succeeded.", 49, 5); break;
		case 1: PutString("Autodetection might be incorrect.", 49, 5); break;
		case 2: PutString("Autodetection not supported in firmware.", 49, 5); break;
		}
		for (drive = 0; drive < 4; drive++) {
			sprintf(buffer, "Drive %c: %s", drive + 'A', drivetypes[validdrive[drive]]);
			PutString(buffer, 59 + 10 * drive, 5);
		}
		if (targetdrive)
			sprintf(buffer, "Target drive: %c", targetdrive - 1 + 'A');
		else
			strcpy(buffer, "No valid target drive set");
		PutString(buffer, 99, 5);
	}

	PutString(CBOLDON "GeoUTime config data:" CPLAINTEXT, 119, 5);
	sprintf(buffer, "Hostname: %s", host);
	PutString(buffer, 129, 5);
	sprintf(buffer, "UTC offset: %ld", secondsfromutc);
	PutString(buffer, 139, 5);
	sprintf(buffer, "Update from NTP: %sabled", ntpon ? "En" : "Dis");
	PutString(buffer, 149, 5);
	sprintf(buffer, "Verbose: %sabled", verbose ? "En" : "Dis");
	PutString(buffer, 159, 5);
}

// ---- menu actions -----------------------------------------------------------

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

static void settingNTPOn(void)
{
	GotoFirstMenu();
	sprintf(buffer, "Present value is: %s", ntpon ? "Yes" : "No");
	ntpon = DlgBoxYesNo("Do you want to enable NTP query?", buffer) == YES;
	ConfigSave();
	PrintConfigdata(1);
}

static void settingHostname(void)
{
	GotoFirstMenu();
	if (DlgBoxGetString(host, 80, "Enter NTP server hostname", "(Cancel to keep present)") != CANCEL) {
		ConfigSave();
		PrintConfigdata(1);
	}
}

static void settingUTCOffset(void)
{
	char offsetinput[12];

	GotoFirstMenu();
	sprintf(offsetinput, "%ld", secondsfromutc);
	if (DlgBoxGetString(offsetinput, 9, "Enter UTC offset in seconds", "(Cancel to keep present)") != CANCEL) {
		secondsfromutc = strtol(offsetinput, NULL, 10);
		ConfigSave();
		PrintConfigdata(1);
	}
}

static void settingVerbose(void)
{
	GotoFirstMenu();
	sprintf(buffer, "Present value is: %s", verbose ? "Yes" : "No");
	verbose = DlgBoxYesNo("Do you want to enable verbosity?", buffer) == YES;
	ConfigSave();
	PrintConfigdata(1);
}

static void settingOverride(void)
{
	GotoFirstMenu();
	sprintf(buffer, "Present value is: %s", mountconfig.auto_override ? "Yes" : "No");
	mountconfig.auto_override = DlgBoxYesNo("Do you want to enable override?", buffer) == YES;
	SaveFileHeader();
	SetValidDrives();
	PrintConfigdata(1);
}

// target: 0 = first valid drive (auto), 1-4 = drive A-D
static void SetTargetGeneric(char target)
{
	GotoFirstMenu();
	if (target && !validdrive[target - 1]) {
		DlgBoxOk("Drive is no valid target", "Press OK.");
		return;
	}
	mountconfig.target = target;
	SaveFileHeader();
	SetValidDrives();
	PrintConfigdata(1);
}

static void settingTargetAuto(void) { SetTargetGeneric(0); }
static void settingTargetA(void)    { SetTargetGeneric(1); }
static void settingTargetB(void)    { SetTargetGeneric(2); }
static void settingTargetC(void)    { SetTargetGeneric(3); }
static void settingTargetD(void)    { SetTargetGeneric(4); }

// Manual setting of a drive as valid target, with its GEOS drive type.
static void SetDrivesGeneric(char drive)
{
	char type;

	GotoFirstMenu();
	sprintf(buffer, "Set drive %c as valid?", 'A' + drive);
	type = DlgBoxYesNo(buffer, "Click Yes or No.") == YES ? drivetypeID[drive] : 0;
	// GEOS RAM drive types (bit 7 set) to the drivetypes[] index
	if (type > 3) {
		switch (type) {
		case 0x81: type = 4; break;
		case 0x82: type = 5; break;
		case 0x83: type = 6; break;
		case 0x84: type = 7; break;
		default:   type = 0; break;
		}
	}
	mountconfig.valid[drive] = type;
	if (type && !mountconfig.target)
		mountconfig.target = drive + 1;
	SaveFileHeader();
	SetValidDrives();
	PrintConfigdata(1);
}

static void settingDriveA(void) { SetDrivesGeneric(0); }
static void settingDriveB(void) { SetDrivesGeneric(1); }
static void settingDriveC(void) { SetDrivesGeneric(2); }
static void settingDriveD(void) { SetDrivesGeneric(3); }

static void informationCredits(void)
{
	unsigned x;

	GotoFirstMenu();
	x = CreateWindow();
	PutString(CBOLDON "GeoUConfig" CPLAINTEXT, 69, x);
	PutString("Settings configuration utility for GeoUTools", 79, x);
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
GEOS_CALLBACK(settingNTPOn_cb, settingNTPOn)
GEOS_CALLBACK(settingHostname_cb, settingHostname)
GEOS_CALLBACK(settingUTCOffset_cb, settingUTCOffset)
GEOS_CALLBACK(settingVerbose_cb, settingVerbose)
GEOS_CALLBACK(settingOverride_cb, settingOverride)
GEOS_CALLBACK(settingTargetAuto_cb, settingTargetAuto)
GEOS_CALLBACK(settingTargetA_cb, settingTargetA)
GEOS_CALLBACK(settingTargetB_cb, settingTargetB)
GEOS_CALLBACK(settingTargetC_cb, settingTargetC)
GEOS_CALLBACK(settingTargetD_cb, settingTargetD)
GEOS_CALLBACK(settingDriveA_cb, settingDriveA)
GEOS_CALLBACK(settingDriveB_cb, settingDriveB)
GEOS_CALLBACK(settingDriveC_cb, settingDriveC)
GEOS_CALLBACK(settingDriveD_cb, settingDriveD)

// ---- menus ------------------------------------------------------------------

static const GEOS_MENU(3) menuGEOS = {
	{ 15, 15 + 3 * 14, 0, 100 }, 3 | VERTICAL,
	{
		{ "Switch 40/80", MENU_ACTION, geosSwitch4080_cb },
		{ "Credits", MENU_ACTION, informationCredits_cb },
		{ "Exit", MENU_ACTION, geosExit_cb },
	}
};

static const GEOS_MENU(4) menuTime = {
	{ 15, 15 + 4 * 14, 35, 135 }, 4 | VERTICAL,
	{
		{ "Enable NTP", MENU_ACTION, settingNTPOn_cb },
		{ "Hostname", MENU_ACTION, settingHostname_cb },
		{ "UTC offset", MENU_ACTION, settingUTCOffset_cb },
		{ "Verbose", MENU_ACTION, settingVerbose_cb },
	}
};

static const GEOS_MENU(4) menuDrives = {
	{ 15, 15 + 4 * 14, 170, 225 }, 4 | VERTICAL,
	{
		{ "Drive A", MENU_ACTION, settingDriveA_cb },
		{ "Drive B", MENU_ACTION, settingDriveB_cb },
		{ "Drive C", MENU_ACTION, settingDriveC_cb },
		{ "Drive D", MENU_ACTION, settingDriveD_cb },
	}
};

static const GEOS_MENU(5) menuTarget = {
	{ 15, 15 + 5 * 14, 170, 225 }, 5 | VERTICAL,
	{
		{ "Auto", MENU_ACTION, settingTargetAuto_cb },
		{ "Drive A", MENU_ACTION, settingTargetA_cb },
		{ "Drive B", MENU_ACTION, settingTargetB_cb },
		{ "Drive C", MENU_ACTION, settingTargetC_cb },
		{ "Drive D", MENU_ACTION, settingTargetD_cb },
	}
};

static const GEOS_MENU(3) menuMount = {
	{ 15, 15 + 3 * 14, 70, 170 }, 3 | VERTICAL,
	{
		{ "Enable override", MENU_ACTION, settingOverride_cb },
		{ "Set drive", SUB_MENU, (const void *)&menuDrives },
		{ "Set target", SUB_MENU, (const void *)&menuTarget },
	}
};

static const GEOS_MENU(3) menuMain = {
	{ 0, 15, 0, 115 }, 3 | HORIZONTAL,
	{
		{ "GEOS", SUB_MENU, (const void *)&menuGEOS },
		{ "Time", SUB_MENU, (const void *)&menuTime },
		{ "Mount", SUB_MENU, (const void *)&menuMount },
	}
};

// Screen, menu, icons and settings for the current 40/80 column mode
static void Redraw(void)
{
	ReinitScreen(appname, (const struct icontab *)&cfgicons, (const struct icontab *)&cfgicons);
	DoMenu((const struct menu *)&menuMain);
	DoIcons(icons);
	PrintConfigdata(0);
}

int main(void)
{
	strcpy(appname, "   GeoUConfig   ");
	strcpy(version, VERSION);
	osType = get_ostype();

	mount_notfound = LoadFileHeader();
	if (!mount_notfound)
		ReadConfigdata();

	ConfigLoad();
	SetValidDrives();
	Redraw();
	return 0;	// MainLoop runs the menus
}
