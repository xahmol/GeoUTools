/*
GeoUTime - Synch program
Utility to synch GEOS system time with NTP server and UII+ RTC clock
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
#include <time.h>
#include "defines.h"
#include "ultimate_common_lib.h"
#include "ultimate_time_lib.h"
#include "ultimate_network_lib.h"
#include "time_common.h"

// Global variables
unsigned char socket = 0;
unsigned char osType = 0;

// Icons
void QuitApplication();

char iconQuit[] = {
    0x05,0xFF,0x82,0xFE,0x80,0x04,0x00,0x82,0x03,0x80,0x04,0x00,0xB8,0x03,0x80,0x0F,
    0x80,0x19,0x80,0x03,0x80,0x18,0xC0,0x01,0x80,0x03,0x80,0x18,0xD9,0xBB,0xC0,0x03,
    0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x18,0xD9,0x99,
    0x80,0x03,0x80,0x18,0xD9,0x99,0x80,0x03,0x80,0x1B,0xD9,0x99,0x80,0x03,0x80,0x0F,
    0x8F,0x98,0xE0,0x03,0x80,0x04,0x00,0x82,0x03,0x80,0x04,0x00,0x81,0x03,0x06,0xFF,
    0x81,0x7F,0x05,0xFF
};

struct icontab quiticon = {
    1,
    { 0,0 },
    {
        { iconQuit, 1, 160, 6, 16, (int)QuitApplication }
    }
};

// Application routines
unsigned char CheckStatus() {
// Function to check UII+ status and print error box if applicable

    if (!uii_success()) {
        if(verbose) {
            DlgBoxOk("Ultimate Command Interface error!",(const char *)uii_status);
        }
        return 1;
    }
    return 0;
}

void close_socket() {
// Close the open websocket

    enableIO();
    uii_socketclose(socket);
    restoreIO();
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
    if(verbose) {
        sprintf(buffer,"Connecting to: %s", host);
        PutString(buffer,49,10);
    }
    enableIO();
	socket = uii_udpconnect(host, 123); //https://github.com/markusC64/1541ultimate2/blob/master/software/io/network/network_target.cc
    restoreIO();
    
    if(CheckStatus()) {
        close_socket();
        return;
    }

    // Sending data request. Return on error
    if(verbose) {
        PutString("Sending NTP request,",59,10);
    }

	fullcmd[2] = socket;

    enableIO();
    uii_settarget(TARGET_NETWORK);
    uii_sendcommand(fullcmd, 51);//3 + sizeof( ntp_packet ));
	uii_readstatus();
	uii_accept();
    restoreIO();

    if(CheckStatus()) {
        close_socket();
        return;
    }

    // Recieving datat. Return on error
    if(verbose) {
        PutString("Reading result.",69,10);
    }
    enableIO();
    uii_socketread(socket, 50);// 2 + sizeof( ntp_packet ));
    restoreIO();

    if(CheckStatus()) {
        close_socket();
        return;
    }

    // Close socketwinOK
    close_socket();

    // Convert time received to UCI format and set UII+ RTC time
    t = uii_data[37] | (((unsigned long)uii_data[36])<<8)| (((unsigned long)uii_data[35])<<16)| (((unsigned long)uii_data[34])<<24);
    t -= NTP_TIMESTAMP_DELTA;

    if(verbose) {
        sprintf(buffer,"UNIX epoch: %lu", t);
        PutString(buffer,79,10);
    }

    _tz.timezone = secondsfromutc;
    datetime = localtime(&t);
    if (strftime(res, sizeof(res), "%F %H:%M:%S", datetime) == 0){
        if(verbose) {
            DlgBoxOk("Data error","Cannot parse date.");
        }
        return;
    }

    if(verbose) {
        sprintf(buffer,"NTP datetime: %s", res);
        PutString(buffer,89,10);
    }

    settime[0]=datetime->tm_year;
    settime[1]=datetime->tm_mon + 1;
    settime[2]=datetime->tm_mday;
    settime[3]=datetime->tm_hour;
    settime[4]=datetime->tm_min;
    settime[5]=datetime->tm_sec;

    enableIO();
    uii_set_time(settime);
    restoreIO();
    if(verbose) {
        sprintf(buffer,"Synched UII+ RTC. Status: %s", uii_status);
        PutString(buffer,99,10);
    }
}

void timeSynch () {
// Synch system time with UII+ RTC
    char* ptrend;
    unsigned char rtc_hour;
    
    // Get UII+ RTC time
    enableIO();
    uii_get_time();
    restoreIO();

    if(!CheckStatus())
    {
        if(verbose) {
            sprintf(buffer,"Get UII+ RTC. Time: %s", uii_data);
            PutString(buffer,109,10);
        }

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

        // Enable IO to access CIA time registers
        enableIO();

        // Copy hour
        buffer[0]=uii_data[11];
        buffer[1]=uii_data[12];
        rtc_hour = strtol(buffer,&ptrend,10);
        system_date.s_hour = rtc_hour;

        if(rtc_hour>12) {
            // Set PM
            cia_hour = (uii_data[12] - 50) + ((uii_data[11] - 49)*16) + 128;
        } else {
            // Set AM
            cia_hour = (uii_data[12] - 48) + ((uii_data[11] - 48)*16);
        }

        // Copy minutes
        buffer[0]=uii_data[14];
        buffer[1]=uii_data[15];
        system_date.s_minutes = strtol(buffer,&ptrend,10);
        cia_minutes = (uii_data[15] - 48) + ((uii_data[14] - 48)*16);

        // Copy seconds
        buffer[0]=uii_data[17];
        buffer[1]=uii_data[18];
        system_date.s_seconds = strtol(buffer,&ptrend,10);
        cia_seconds = (uii_data[18] - 48) + ((uii_data[17] - 48)*16);

        // Set tens of seconds to zero
        cia_tensofsec = 0;

        // Return to normal state
        restoreIO();

        if(verbose) {
            sprintf(buffer,"New GEOS system time: %2d/%2d/%2d %2d:%2d:%2d",
            system_date.s_day,system_date.s_month,system_date.s_year,
            system_date.s_hour,system_date.s_minutes,system_date.s_seconds);
            PutString(buffer,119,10);
        }
    }
}

// Quit icon handler
void QuitApplication() {
    EnterDeskTop();
}

// Main

void main (void)
{
    // Check if UCI is detected, else abort.
    enableIO();
    if(!uii_detect()) { restoreIO(); DlgBoxOk("No Ultimate Command Interface","Press OK to abort program."); EnterDeskTop(); }
    restoreIO();

    // Get host OS type
    osType = get_ostype();

    // Load config file
    ConfigLoad();

    // If verbose, draw dialogue window
    if(verbose)
    {
        SetPattern(0);
        drawWindow.top = 20;
        drawWindow.bot = 180;
        drawWindow.left = 0;
        if(osType & GEOS128 ) { drawWindow.right = 319 + DOUBLE_W;  }
        else { drawWindow.right = 319; }
        Rectangle();  // Window
        FrameRectangle(255);    // Frame
        PutString(CBOLDON "GeoUTime - Verbose mode" CPLAINTEXT,29,10);
    }

    // Get NTP time and set UII+ RTC if enabled
    if(ntpon)
    {
        get_ntp_time();
    }
    else
    {
        if(verbose) {
            PutString("NTP query disabled.",49,10);
            PutString("Synch with present UII+ RTC time.",59,10);
        }
    }

    // Synch GEOS system time with UII+ RTC
    timeSynch();

    // Activate quit icon
    if(verbose){
        DoIcons(&quiticon);
        MainLoop();
    }
}