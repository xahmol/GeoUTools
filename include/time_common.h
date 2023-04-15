#ifndef __TIME_COMMON_H_
#define __TIME_COMMON_H_

// Functions to set time using NTP server
// Source: https://github.com/MaxPlap/ntp2ultimate

#define NTP_TIMESTAMP_DELTA 2208988800ul //0x83AA7E80
#define configfileversion   1
#define year                (*(char*)0x8516)
#define month               (*(char*)0x8517)
#define day                 (*(char*)0x8518)
#define hour                (*(char*)0x8519)                
#define minutes             (*(char*)0x851A)
#define seconds             (*(char*)0x851B)

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