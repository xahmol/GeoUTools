// interface.h - GeoUTools: screen layout and window helpers shared by
// GeoUMount and GeoUConfig (40 and 80 columns).

#ifndef INTERFACE_H
#define INTERFACE_H

#include <geos.h>

extern char version[25];
extern unsigned screen_pixel_width;
extern char osType;
extern char vdc;
extern char appname[21];
extern char windowflag;

extern const struct window *winMain;
extern const struct window *winHeader;
extern const struct window *winInterface;
extern const struct window *winIntShadow;
extern const struct window *winIntRecover;

extern const char IconOK[];
extern const struct icontab *icons;
extern const struct icontab *mainicons;
extern const struct icontab *winOK;

void SetRectangleCoords(char top, char bottom, unsigned left, unsigned right);
unsigned StringLength(const char *s);
void PutStringCentered(const char *s, char bottom, unsigned left, unsigned right);
void PutStringRight(const char *s, char bottom, unsigned left, unsigned right);
unsigned CreateWindow(void);
void WinOKButton(void);
void CloseWindow(void);
void ReinitScreen(const char *s, const struct icontab *vicicons, const struct icontab *vdcicons);

#pragma compile("interface.c")

#endif
