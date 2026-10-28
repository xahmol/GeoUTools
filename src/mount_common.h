// mount_common.h - GeoUTools: drive detection and GeoUMount settings,
// shared by GeoUMount and GeoUConfig.

#ifndef MOUNT_COMMON_H
#define MOUNT_COMMON_H

// GeoUMount settings, stored in GeoUMount's own GEOS file header in the
// application data area (O_GHAPDAT, header bytes $89-$8E =
// fileheader.author[40..45]).
struct mconfig {
	char auto_override;     // 1: use valid[] instead of auto-detection
	char valid[4];          // drive type per drive A-D (index into drivetypes)
	char target;            // default target drive 1-4, 0 = first valid
};
extern struct mconfig mountconfig;

#define MCONFIG_OFFSET  40  // in struct fileheader.author

extern char validdrive[4];
extern const char drivetypes[8][14];
extern const char entrytypes[7][4];
extern char drivetypeID[4];
extern char ramdiskID[8];
extern char targetdrive;
extern char firmwareflag;  // 0 fw 3.10f+, 1 no RAM disk info, 2 no device info

char CheckStatus(void);
char Checkcommandsupport(void);
void ReadConfigdata(void);
void SetValidDrives(void);

#pragma compile("mount_common.c")

#endif
