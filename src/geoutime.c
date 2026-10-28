/*
GeoUTime
Utility to synch the GEOS system time with an NTP server and the Ultimate
real-time clock. Runs as an auto-exec application at boot.
Written in 2023 by Xander Mol, Oscar64 version 2026

https://github.com/xahmol/GeoUTools
https://www.idreamtin8bits.com/

Code and resources from others used:

-   Scott Hutter - Ultimate II+ library and GeoUTerm
    https://github.com/xlar54/ultimateii-dos-lib/
    (now via https://github.com/xahmol/ultimate-uci-oscar64)

-   ntp2ultimate by MaxPlap: code for obtaining time via NTP
    https://github.com/MaxPlap/ntp2ultimate

-   Oscar64 by Dr. Mortal Wombat, https://github.com/drmortalwombat/oscar64

-   Hitchhikers Guide to GEOS v2022, Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2022

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
#include <stdlib.h>
#include <geos.h>
#include "defines.h"
#include "uci_io.h"
#include "time_common.h"
#include <ultimate_common_lib.h>
#include <ultimate_time_lib.h>
#include <ultimate_network_lib.h>

static char socket = 0;

// Quit icon (compacted bitmap, 6 cards x 16 lines)
static const char iconQuit[] = {
	0x05,0xFF,0x82,0xFE,0x80,0x04,0x00,0x82,0x03,0x80,0x04,0x00,0xB8,0x03,0x80,0x0F,
	0x80,0x19,0x80,0x03,0x80,0x18,0xC0,0x01,0x80,0x03,0x80,0x18,0xD9,0xBB,0xC0,0x03,
	0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x18,0xD9,0x99,
	0x80,0x03,0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x1B,0xD9,0x99,0x80,0x03,0x80,0x0F,
	0x8F,0x98,0xE0,0x03,0x80,0x04,0x00,0x82,0x03,0x80,0x04,0x00,0x81,0x03,0x06,0xFF,
	0x81,0x7F,0x05,0xFF
};

static void QuitApplication(void)
{
	EnterDeskTop();
}
GEOS_CALLBACK(quit_cb, QuitApplication)

static const GEOS_ICONTAB(1) quiticon = {
	1, { 0, 0 },
	{ { iconQuit, 1, 160, 6, 16, quit_cb } }
};

// Print a status line (verbose mode only)
static void say(const char *text, char y)
{
	if (verbose)
		PutString(text, y, 10);
}

// Check the UCI status after a command; show it in verbose mode.
static char CheckStatus(void)
{
	if (!UII_SUCCESS) {
		if (verbose)
			DlgBoxOk("Ultimate Command Interface error!", uii_status);
		return 1;
	}
	return 0;
}

static void close_socket(void)
{
	enableIO();
	uii_socketclose(socket);
	restoreIO();
}

// Get the time from the NTP server and set the Ultimate RTC with it.
static void get_ntp_time(void)
{
	// UCI socket write command + 48-byte NTP request (LI 0, version 3,
	// mode 3 client: first byte $1B)
	char fullcmd[51];
	char settime[6];
	char attempt = 1, clock;
	unsigned long t;
	struct datetime dt;

	memset(fullcmd, 0, sizeof(fullcmd));
	fullcmd[1] = NET_CMD_SOCKET_WRITE;
	fullcmd[3] = 0x1b;

	if (verbose) {
		sprintf(buffer, "Connecting to: %s", host);
		say(buffer, 49);
	}
	enableIO();
	socket = uii_udpconnect(host, 123);
	restoreIO();
	if (CheckStatus()) {
		close_socket();
		return;
	}

	say("Sending NTP request", 59);
	fullcmd[2] = socket;
	enableIO();
	uii_settarget(TARGET_NETWORK);
	uii_sendcommand(fullcmd, 51);
	uii_readstatus();
	uii_accept();
	restoreIO();
	if (CheckStatus()) {
		close_socket();
		return;
	}

	// Up to 4 attempts to read the answer, one second apart
	do {
		enableIO();
		clock = cia_seconds;
		while (cia_seconds == clock)
			;
		restoreIO();

		if (verbose) {
			sprintf(buffer, "Reading result attempt %d", attempt);
			say(buffer, 69);
		}
		enableIO();
		uii_socketread(socket, 50);
		restoreIO();
		attempt = UII_SUCCESS ? 5 : attempt + 1;
	} while (attempt < 5);

	if (CheckStatus()) {
		close_socket();
		return;
	}

	// Transmit timestamp seconds (big-endian) at uii_data[34..37]
	t = (unsigned long)uii_data[37] | ((unsigned long)uii_data[36] << 8) |
	    ((unsigned long)uii_data[35] << 16) | ((unsigned long)uii_data[34] << 24);
	t -= NTP_TIMESTAMP_DELTA;
	close_socket();

	if (verbose) {
		sprintf(buffer, "UNIX epoch: %lu", t);
		say(buffer, 79);
	}

	unix_to_datetime(t + secondsfromutc, &dt);

	if (verbose) {
		sprintf(buffer, "NTP datetime: %04u-%02u-%02u %02u:%02u:%02u",
		        dt.year + 1900, dt.month, dt.day, dt.hour, dt.minute, dt.second);
		say(buffer, 89);
	}

	settime[0] = dt.year;
	settime[1] = dt.month;
	settime[2] = dt.day;
	settime[3] = dt.hour;
	settime[4] = dt.minute;
	settime[5] = dt.second;
	enableIO();
	uii_set_time(settime);
	restoreIO();
	if (verbose) {
		sprintf(buffer, "Synched Ultimate RTC. Status: %s", uii_status);
		say(buffer, 99);
	}
}

// Two ASCII digits to a number
static char twodigits(const char *p)
{
	return (p[0] - '0') * 10 + (p[1] - '0');
}

// Set the GEOS system time and the CIA 1 time of day clock from the
// Ultimate RTC ("yyyy/mm/dd hh:mm:ss").
static void timeSynch(void)
{
	char rtc_hour, bcd_hour_base;

	enableIO();
	uii_get_time();
	restoreIO();
	if (CheckStatus())
		return;

	if (verbose) {
		sprintf(buffer, "Get Ultimate RTC. Time: %s", uii_data);
		say(buffer, 109);
	}

	system_date.s_year = twodigits(uii_data + 2);
	system_date.s_month = twodigits(uii_data + 5);
	system_date.s_day = twodigits(uii_data + 8);
	rtc_hour = twodigits(uii_data + 11);
	system_date.s_hour = rtc_hour;
	system_date.s_minutes = twodigits(uii_data + 14);
	system_date.s_seconds = twodigits(uii_data + 17);

	// CIA TOD: 12-hour BCD with bit 7 = PM
	if (rtc_hour > 12) {
		bcd_hour_base = 0x80;
		rtc_hour -= 12;
	} else
		bcd_hour_base = 0;

	enableIO();
	cia_hour = bcd_hour_base + (rtc_hour / 10) * 16 + rtc_hour % 10;
	cia_minutes = (uii_data[15] - '0') + (uii_data[14] - '0') * 16;
	cia_seconds = (uii_data[18] - '0') + (uii_data[17] - '0') * 16;
	cia_tensofsec = 0;		// writing tenths starts the clock
	restoreIO();

	if (verbose) {
		sprintf(buffer, "New GEOS system time: %2d/%2d/%2d %2d:%2d:%2d",
		        system_date.s_day, system_date.s_month, system_date.s_year,
		        system_date.s_hour, system_date.s_minutes, system_date.s_seconds);
		say(buffer, 119);
	}
}

int main(void)
{
	char ok;

	ok = uci_detect();
	if (!ok) {
		DlgBoxOk("No Ultimate Command Interface", "Press OK to abort program.");
		EnterDeskTop();
	}

	ConfigLoad();

	if (verbose) {
		SetPattern(0);
		r2L = 0;
		r2H = 199;
		r3 = 0;
		r4 = 319 | DOUBLE_W | ADD1_W;
		Rectangle();
		FrameRectangle(255);
		PutString(CBOLDON "GeoUTime - Verbose mode" CPLAINTEXT, 29, 10);
	}

	if (ntpon)
		get_ntp_time();
	else {
		say("NTP query disabled.", 49);
		say("Synch with present Ultimate RTC time.", 59);
	}

	timeSynch();

	if (verbose) {
		// Stay until the user clicks the quit icon
		DoIcons((const struct icontab *)&quiticon);
		return 0;
	}
	// Silent: continue the boot (or return to the deskTop)
	EnterDeskTop();
	return 0;
}
