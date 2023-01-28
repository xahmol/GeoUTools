# GeoUTools
Tools for using the UII+ cartridge with GEOS

## Contents

[Version history and download](#version-history-and-download)

[Known issues and bugs](#known-issues-and-bugs)

[Introduction](#introduction)

[What is on the disk](#what-is-on-the-disk)

[GeoUTime instructions](#geoutime-instructions)

![GeoUMount main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mainscreen.png)

## Version history and download
([Back to contents](#contents))

[Link to latest build](https://github.com/xahmol/GeoUTools/raw/main/GeoUTools.d81)

Version 0.1 - 20230128 - 1623:
- First public alpha version

## Known issues and bugs
([Back to contents](#contents))
- Freezes in Wheels. Needs further testing and investigation

## Introduction
([Back to contents](#contents))

**GeoUTools** is alittle suite of (for now) two utilities to help using the Ultimate II+ cartridge from within GEOS.

### GeoUTime

Utility to set the GEOS system time (and the UII+to RTC clock) the time received from a user defined NTP server.
This includes GeoUTimeCfg, a tool to set the configuration settings for GeoUTime (NTP server hostname, offset from UTC time in seconds and verbosity on or off).

GeoUTIme is an auto exec application that starts automatically on boot. For this reason, the verbosity setting by default is 'Off' so not showing dialogues on every boot. But it can be switched on for debugging (or curiosity what happens).

### GeoUMount
This utility can be used to swap and mount virtual disks on your UII+ cartridge, helpful because the UII+ menu is not available under GEOS128 80 column mode and often crashes under GEOS128 40 column mode. However therefore mostly useful under GEOS128, the program should work in all flavours of GEOS on the C64 and C128 as long as that computer has an UII+ cartridge.

Images can be selected from a filebrowser, that filters on image types fitting the target drive.

Number of direntries in this build tops around 175 or so (rest of the dir is not shown). Which, also considering the filtering on dirs and fitting image types only, is more than enough for my personal needs.

## What is on the disk
([Back to contents](#contents))

On first mount from the disk image downloaded from GitHub, the disk is not yet converted to a GEOS disk, so you will get this message:

![Non GEOS disk message](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20nongeosdisk.png)

Click on Yes in this dialogie. You will then see these contents on the disk:

![GeoUTools disk clean](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20contentsfirst.png)

- **GeoUMount**: Disk mounting tool
- **GeoUTime**: NTP time synching tool
- **GeoUTimeCfg**: Utility to set configuration options for GeoUTime

Double click on the desired icon of the three to start the corresponding utility.

Note that on first use, both GeoUTime and GeoUTimeCfg create a configuration file on the disk if no one is yet present. After this, the contents of the disk look like this:

![Configuration file added on disk](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20contents%20data.png)

A new file called GeoUTimeDat is added to store configuration data.

Obviously you are free (even encouraged) tp copy these files to another disk target, such as your main RAM drive.

## GeoUTime instructions
([Back to contents](#contents))

This program is driven by the options stored in the configuration file and does not need further user interaction.

If verbosity is set to no in the configuration file options, nothing will be shown on the screen apart from the screen blanking during execution and the system time being updated after returning to the desktop.

If verbosity mode is 'On', something like this will be shown on the screen:

![GeoUTime verbosity output](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20geoutime%20verbose%20output.png)

This mode is meant to either debug your configuration settings, or if you are just curious what happens:
- Connecting to the selected hostname of the desired NTP server
- Sending the data request to that server
- Reiving the data and interpreting result
- Showing UNIX epoch received
- Cobverting that output to time in selected time zone using the UTC offset
- Setting the UII+ Real Time Clock with that time
- Synching GEOS system time with the UII+ RTC clock.
- Confirming succes
Click OK in dialogue box to exit.

Note that on any NTP connection error, the GEOS clock will be synched to the unchanged UII+ RTC clock as fallback.