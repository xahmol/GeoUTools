#ifndef __INTERFACE_H_
#define __INTERFACE_H_

// Global variables
extern char version[25];
extern unsigned int screen_pixel_width;
extern unsigned char osType;
extern unsigned char vdc;
extern char appname[21];

// Window definitions
extern struct window *winMain;
extern struct window *winHeader;
extern struct window *winInterface;
extern struct window *winIntShadow;
extern struct window *winIntRecover;

extern struct window vic_winHdr;
extern struct window vic_winMain;
extern struct window vic_winInt;
extern struct window vic_winShd;
extern struct window vic_winRec;

extern struct window vdc_winHdr;
extern struct window vdc_winMain;
extern struct window vdc_winInt;
extern struct window vdc_winShd;
extern struct window vdc_winRec;

// Icons
extern char IconOK[];
extern struct icontab *icons;
extern struct icontab *winOK;
extern struct icontab noicons;
extern struct icontab vic_winOK;
extern struct icontab vdc_winOK;

// Function prototypes
void SetRectangleCoords(unsigned char top, unsigned char bottom, unsigned int left, unsigned int right);
int StringLength (char* inputstring);
void PutStringCentered(char* inputstring, unsigned char bottom, unsigned int left, unsigned int right);
void PutStringRight(char* inputstring, unsigned char bottom, unsigned int left, unsigned int right);
int CreateWindow(void);
void WinOKButton(void);
void CloseWindow (void);
void ReinitScreen(char *s);

#endif