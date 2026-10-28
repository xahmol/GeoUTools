// time_common.h - GeoUTools: GeoUTime settings (shared by GeoUTime and
// GeoUConfig) and time helpers.

#ifndef TIME_COMMON_H
#define TIME_COMMON_H

#include <geos.h>

// Seconds between 1900-01-01 (NTP epoch) and 1970-01-01 (Unix epoch).
// Source: ntp2ultimate by MaxPlap, https://github.com/MaxPlap/ntp2ultimate
#define NTP_TIMESTAMP_DELTA 2208988800ul

#define CONFIGFILEVERSION   1

// CIA 1 time of day clock (only with I/O banked in, see uci_io.h)
#define cia_hour            (*(volatile char *)0xDC0B)
#define cia_minutes         (*(volatile char *)0xDC0A)
#define cia_seconds         (*(volatile char *)0xDC09)
#define cia_tensofsec       (*(volatile char *)0xDC08)

// Settings, stored in the data file GeoUTimeDat
extern long secondsfromutc;
extern char verbose;
extern char ntpon;
extern char host[81];
extern char configfilename[13];

void ConfigSave(void);
void ConfigLoad(void);

// Broken-down local time (cc65's struct tm fields that GeoUTime uses)
struct datetime {
	char year;              // years since 1900
	char month;             // 1-12
	char day;               // 1-31
	char hour, minute, second;
};

// Unix time to date and time (replaces cc65's localtime, which Oscar64
// does not have): t must already include the UTC offset.
void unix_to_datetime(unsigned long t, struct datetime *dt);

#pragma compile("time_common.c")

#endif
