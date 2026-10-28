/*
GeoUTools - GeoUTime settings and time helpers (GeoUTime, GeoUConfig)
Written in 2023 by Xander Mol, Oscar64 version 2026

https://github.com/xahmol/GeoUTools
https://www.idreamtin8bits.com/

Code and resources from others used:

-   Scott Hutter - Ultimate II+ library and GeoUTerm
    https://github.com/xlar54/ultimateii-dos-lib/
    (now via https://github.com/xahmol/ultimate-uci-oscar64)

-   ntp2ultimate by MaxPlap: code for obtaining time via NTP
    https://github.com/MaxPlap/ntp2ultimate

-   Hitchhikers Guide to GEOS v2022, Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2022

-   Heureks: GEOS64 Programming Examples
    https://codeberg.org/heurekus/C64-Geos-Programming

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

char buffer[81];

long secondsfromutc = 3600;
char host[81] = "pool.ntp.org";
char verbose = 0;
char ntpon = 1;
char configfilename[13] = "GeoUTimeDat";

// Data file layout (95 bytes, unchanged since 1.0 so older files load):
//   0-79  NTP host name (null-terminated)
//   80-91 UTC offset in seconds, decimal text
//   92    verbose flag, '0'/'1'
//   93    NTP on flag, '0'/'1'
//   94    file version, '0' + CONFIGFILEVERSION
static char configbuffer[95];

// GEOS file header for the data file. The first word points to the file
// name when SaveFile is called (set in ConfigSave). Class "GeoUTimeData"
// V1.1, parent application GeoUConfig (opened on double click).
static struct fileheader configfileHdr = {
	{0, 0},
	{3, 21, 63 | 0x80},
	{   0xC0,0x3C,0x00,0xF0,0x3C,0x00,0xF0,0x3C,0x00,0xF0,0x3C,0x00,0x00,0x03,0xE0,0xF0,
	    0x03,0xC0,0x00,0x03,0x00,0xF0,0x00,0x00,0x00,0x03,0x00,0xF0,0x03,0xC0,0x00,0x03,
	    0xE0,0xF0,0x3C,0x00,0xF0,0x3C,0x38,0xF0,0x3C,0x44,0xF8,0x3C,0x92,0xFF,0xFD,0x11,
	    0x7F,0xFD,0x1D,0x3F,0xFD,0x01,0x0F,0xFC,0x82,0x00,0x00,0x44,0x00,0x00,0x38},
	0x83,				// USR
	APPL_DATA,
	SEQUENTIAL,
	0, 0, 0,
	{'G','e','o','U','T','i','m','e','D','a','t','a','V','1','.','1',0,0,0},
	0x00,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 'G','e','o','U','C','o','n','f','i','g',' ',' ','V','1','.','1',0},
	"Config file for GeoUTools."
};

void ConfigSave(void)
{
	char error;

	memset(configbuffer, 0, sizeof(configbuffer));
	strncpy(configbuffer, host, 79);
	sprintf(configbuffer + 80, "%ld", secondsfromutc);
	configbuffer[92] = verbose + '0';
	configbuffer[93] = ntpon + '0';
	configbuffer[94] = CONFIGFILEVERSION + '0';

	configfileHdr.n_block.track = (unsigned)configfilename & 0xff;
	configfileHdr.n_block.sector = (unsigned)configfilename >> 8;
	configfileHdr.load_address = (unsigned)configbuffer;
	configfileHdr.end_address = (unsigned)configbuffer + sizeof(configbuffer);

	DeleteFile(configfilename);
	error = SaveFile(0, &configfileHdr);
	if (error) {
		sprintf(buffer, "Error: %d", error);
		DlgBoxOk("Error saving config file", buffer);
	}
}

void ConfigLoad(void)
{
	char error;

	error = GetFile(ST_LD_AT_ADDR, configfilename, configbuffer, NULL, NULL);
	if (error) {
		// No file yet: create one with the defaults. (GeoUTools 1.x had
		// "error = FILE_NOT_FOUND" here, which treated every error so.)
		if (error == FILE_NOT_FOUND)
			ConfigSave();
		else {
			sprintf(buffer, "Error: %d", error);
			DlgBoxOk("Error loading config file", buffer);
		}
		return;
	}

	verbose = configbuffer[92] - '0';
	ntpon = configbuffer[93] - '0';
	strncpy(host, configbuffer, 80);
	host[80] = 0;
	secondsfromutc = strtol(configbuffer + 80, NULL, 10);
}

// Days in each month of a non-leap year
static const char month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void unix_to_datetime(unsigned long t, struct datetime *dt)
{
	unsigned days = (unsigned)(t / 86400ul);
	unsigned long rest = t % 86400ul;
	unsigned year = 1970, ylen;
	char month = 0, mlen;

	dt->hour = (char)(rest / 3600);
	rest %= 3600;
	dt->minute = (char)(rest / 60);
	dt->second = (char)(rest % 60);

	for (;;) {
		ylen = (year & 3) ? 365 : 366;		// valid 1901-2099
		if (days < ylen)
			break;
		days -= ylen;
		year++;
	}
	for (;;) {
		mlen = month_days[month];
		if (month == 1 && !(year & 3))
			mlen = 29;
		if (days < mlen)
			break;
		days -= mlen;
		month++;
	}
	dt->year = (char)(year - 1900);
	dt->month = month + 1;
	dt->day = (char)days + 1;
}
