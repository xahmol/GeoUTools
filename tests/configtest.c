// configtest.c - GeoUTools emulator test of the GeoUTime settings file
// (time_common.c): runs as AUTO_EXEC in VICE (no Ultimate needed).
//   1 ConfigLoad without a file creates GeoUTimeDat with the defaults
//   2 changed settings survive ConfigSave + ConfigLoad
//   3 unix_to_datetime on known dates
#include <stdio.h>
#include <string.h>
#include <geos.h>
#include "defines.h"
#include "time_common.h"

static char line = 30;

static void say(const char *t)
{
	PutString(t, line, 8);
	line += 10;
}

static void result(const char *what, char ok)
{
	sprintf(buffer, "%s: %s", what, ok ? "PASS" : "FAIL");
	say(buffer);
}

int main(void)
{
	struct datetime dt;
	char ok;

	SetPattern(0);
	r2L = 0; r2H = 199; r3 = 0; r4 = 319;
	Rectangle();
	say("GeoUTools config test");

	DeleteFile(configfilename);
	ConfigLoad();
	result("1 create defaults", FindFile(configfilename) == 0 && ntpon == 1 &&
	       secondsfromutc == 3600 && strcmp(host, "pool.ntp.org") == 0);

	strcpy(host, "time.example.org");
	secondsfromutc = -18000;
	verbose = 1;
	ntpon = 0;
	ConfigSave();
	strcpy(host, "x");
	secondsfromutc = 0;
	verbose = 0;
	ntpon = 1;
	ConfigLoad();
	sprintf(buffer, "loaded: %s %ld %d %d", host, secondsfromutc, verbose, ntpon);
	say(buffer);
	result("2 save and load", strcmp(host, "time.example.org") == 0 &&
	       secondsfromutc == -18000 && verbose == 1 && ntpon == 0);

	// 2026-10-24 11:52:03 UTC = 1792842723; 2000-02-29 00:00:00 = 951782400
	unix_to_datetime(1792842723ul, &dt);
	ok = dt.year == 126 && dt.month == 10 && dt.day == 24 && dt.hour == 11 && dt.minute == 52 && dt.second == 3;
	unix_to_datetime(951782400ul, &dt);
	ok = ok && dt.year == 100 && dt.month == 2 && dt.day == 29 && dt.hour == 0;
	result("3 unix_to_datetime", ok);
	return 0;
}
