/*****************************************************************
Ultimate 64/II+ Command Library _ Network functions
Scott Hutter, Francesco Sblendorio

Based on ultimate_dos-1.2.docx and command interface.docx
https://github.com/markusC64/1541ultimate2/tree/master/doc

Disclaimer:  Because of the nature of DOS commands, use this code
soley at your own risk.

Patches and pull requests are welcome
******************************************************************/

#ifndef _ULTIMATE_NETWORK_LIB_H_
#define _ULTIMATE_NETWORK_LIB_H_

// prototypes
unsigned char uii_tcpconnect(char* host, unsigned short port);
unsigned char uii_udpconnect(char* host, unsigned short port);
void uii_socketclose(unsigned char socketid);
int uii_socketread(unsigned char socketid, unsigned short length);
void uii_socketwrite(unsigned char socketid, char *data);
void uii_socketwritechar(unsigned char socketid, char one_char);
void uii_socketwrite_ascii(unsigned char socketid, char *data);

int uii_tcplistenstart(unsigned short port);
int uii_tcplistenstop(void);
int uii_tcpgetlistenstate(void);
unsigned char uii_tcpgetlistensocket(void);

void uii_logtext(char *text);
void uii_logstatusreg(void);
void uii_sendcommand(unsigned char *bytes, int count);
int uii_readdata(void);
int uii_readstatus(void);
void uii_accept(void);
void uii_abort(void);
int uii_isdataavailable(void);
int uii_isstatusdataavailable(void);

char uii_tcp_nextchar(unsigned char socketid);
int uii_tcp_nextline(unsigned char socketid, char*);
int uii_tcp_nextline_ascii(unsigned char socketid, char*);
void uii_tcp_emptybuffer(void);
void uii_reset_uiidata(void);

#endif
