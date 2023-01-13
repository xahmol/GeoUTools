/*
GeoUTime - Synch program
Utility to synch GEOS system time with NTP server and UII+ RTC clock
Written in 2022 by Xander Mol

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
#include <time.h>
#include "defines.h"
#include "ultimate_common_lib.h"
#include "ultimate_time_lib.h"
#include "ultimate_network_lib.h"
#include "time_common.h"

// Global variables
unsigned char socket = 0;

// Application routines

void close_socket() {
// Close the open websocket

    InitForIO();
    uii_socketclose(socket);
    DoneWithIO();
}

void get_ntp_time() {
// Function to get time from NTP server and set UII+ time with this
// Returns 1 on succes, 0 on fail

    struct tm *datetime;
    extern struct _timezone _tz;

    char settime[6];

    // Build UCI command
    unsigned char fullcmd[] = { 0x00, NET_CMD_SOCKET_WRITE, 0x00, \
                               0x1b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    time_t t;
    char res[32];

    socket = 0;

    // Connect to NTP host. Return on error
    InitForIO();
	socket = uii_udpconnect(host, 123); //https://github.com/markusC64/1541ultimate2/blob/master/software/io/network/network_target.cc
    DoneWithIO();
    
    if(uii_success()) {
        close_socket();
        return;
    }

    // Sending data request. Return on error
	fullcmd[2] = socket;

    InitForIO();
    uii_settarget(TARGET_NETWORK);
    uii_sendcommand(fullcmd, 51);//3 + sizeof( ntp_packet ));
	uii_readstatus();
	uii_accept();
    DoneWithIO();

    if(uii_success()) {
        close_socket();
        return;
    }

    // Recieving datat. Return on error
    InitForIO();
    uii_socketread(socket, 50);// 2 + sizeof( ntp_packet ));
    DoneWithIO();

    if(uii_success()) {
        close_socket();
        return;
    }

    // Close socketwinOK
    close_socket();

    // Convert time received to UCI format and set UII+ RTC time
    t = uii_data[37] | (((unsigned long)uii_data[36])<<8)| (((unsigned long)uii_data[35])<<16)| (((unsigned long)uii_data[34])<<24);
    t -= NTP_TIMESTAMP_DELTA;
    _tz.timezone = secondsfromutc;
    datetime = localtime(&t);
    if (strftime(res, sizeof(res), "%F %H:%M:%S", datetime) == 0){
        return;
    }
    settime[0]=datetime->tm_year;
    settime[1]=datetime->tm_mon + 1;
    settime[2]=datetime->tm_mday;
    settime[3]=datetime->tm_hour;
    settime[4]=datetime->tm_min;
    settime[5]=datetime->tm_sec;

    InitForIO();
    uii_set_time(settime);
    DoneWithIO();
}

void timeSynch () {
// Synch system time with UII+ RTC
    char* ptrend;
    
    // Get UII+ RTC time
    InitForIO();
    uii_get_time();
    DoneWithIO();

    if(uii_success())
    {
        // Set GEOS system time

        // Copy year
        buffer[0]=uii_data[2];
        buffer[1]=uii_data[3];
        buffer[2]=0;
        system_date.s_year = strtol(buffer,&ptrend,10);


        // Copy month
        buffer[0]=uii_data[5];
        buffer[1]=uii_data[6];
        system_date.s_month = strtol(buffer,&ptrend,10);

        // Copy day
        buffer[0]=uii_data[8];
        buffer[1]=uii_data[9];
        system_date.s_day = strtol(buffer,&ptrend,10);

        // Copy hour
        buffer[0]=uii_data[11];
        buffer[1]=uii_data[12];
        system_date.s_hour = strtol(buffer,&ptrend,10);

        // Copy minutes
        buffer[0]=uii_data[14];
        buffer[1]=uii_data[15];
        system_date.s_minutes = strtol(buffer,&ptrend,10);

        // Copy seconds
        buffer[0]=uii_data[17];
        buffer[1]=uii_data[18];
        system_date.s_seconds = strtol(buffer,&ptrend,10);
    }
}

// Main

void main (void)
{
    // Load config file
    ConfigLoad();

    // Get NTP time and set UII+ RTC
    get_ntp_time();

    // Synch GEOS system time with UII+ RTC
    timeSynch();
}