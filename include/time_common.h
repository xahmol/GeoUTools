#ifndef __TIME_COMMON_H_
#define __TIME_COMMON_H_

// Functions to set time using NTP server
// Source: https://github.com/MaxPlap/ntp2ultimate

#define NTP_TIMESTAMP_DELTA 2208988800ul //0x83AA7E80
#define configfileversion   1
#define cia_hour            (*(char*)0xDC0B)                
#define cia_minutes         (*(char*)0xDC0A)
#define cia_seconds         (*(char*)0xDC09)
#define cia_tensofsec       (*(char*)0xDC08)

// Global variables
extern long secondsfromutc; 
extern unsigned char verbose;
extern unsigned char ntpon;
extern char host[81];
extern char configbuffer[95];
extern char filename[13];
extern struct fileheader configfileHdr;

// Function prototypes
void ConfigSave();
void ConfigLoad();

#endif