# GeoUTools
Tools for using the UII+ cartridge with GEOS

## Contents

[Version history and download](#version-history-and-download)

[Known issues and bugs](#known-issues-and-bugs)

[Introduction](#introduction)

[What is on the disk](#what-is-on-the-disk)

[GeiUTimeCfg instructions](#geiutimecfg-instructions)

[GeoUTime instructions](#geoutime-instructions)

[GeoUMount instructions](#geoumount-instructions)

[Credits](#credits)

![GeoUMount main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mainscreen.png)

## Version history and download
([Back to contents](#contents))

[Link to latest build](https://github.com/xahmol/GeoUTools/raw/main/GeoUTools.d81)

Version 0.1 - 20230128 - 2223:
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

## GeiUTimeCfg instructions
([Back to contents](#contents))

This application sets the configuration options for GeoUTIme and saves them in the GeoUTimeDat configuration file. It creates this file on first start if no file is present already.

Interface after application start:

![GeoUTimeCfg main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20mainscreen.png)

The options can be reached via the main menu in the topleft corner.

### GEOS menu

Options in this menu:

![GEOS menu](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20geos%20menu.png)

*Switch 40/80*

(Only works in GEOS128, ignored in GEOS64)

Switches between 40 and 80 column mode. On selecting, the other mode will be selected and the screen will be redrwam. If a single monitor is used, switch monitor to the corresponding other mode to view.

*Exit*

Exits to the GEOS desktop and quits the application.

### Time menu

Options in this menu:

![Time menu](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20time%20menu.png)

*Enable NTP*

Choose if time should be synched with the selected NTP server (setting is 'On'), or if only the GEOS system time should be synched to he UII+ Real Time Clock without an NTP request.

Selecting this option gives this dialogue:

![NTP enable dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20time%20ntp%20enable.png)

Select Yes to enable, No to disable.

*Hostname*

Enter the hostname of the desired NTP server. The default server is pool.ntp.org.

This option results in this dialogue:

![Hostname dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20time%20hostname.png)

For another NTP server, just enter its hostname (URL or IP). The application will not check validity of the input, to check if it works and the hostname resolves and results in a successful connection the verbose mode is suggested.

To keep the present hostname, press either CANCEL or ENTER.

*UTC offset*

Edits the time offset to UTC (Universal standard time). The offset needs to provided in seconds. Automated adjustment for daylight savings ('Summer' and 'Winter' time) is not provided, so you have to adjust your offset on the change from daylight saving time to not.

Offset is set in seconds (to allow for half hour difference timezones and also for finetuning if desired), so multiply the UTC offset in hours by 3600. And note that the offset is signed, so start with a minus for offsets negative to UTC.

Default is set on Central European Time, which requires an offset of 3600. Setting to Central European Summer Time would require 7200. Another example: setting to Eastern Standard Time (EST, timezone for a.o. New York City) would be UTC -5, so -5*3600 = -18000.

See https://www.timeanddate.com/time/zones/ for all offsets in hours (multiply by 3600 to get to seconds).

This option presents this dialogue:

![UTC offset dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20time%20utc%20offset.png)

Tupe the desired offset (noting the signing, so do not forget the minus sign if offsets are negative to UTC) and press ENTER. CANCEL to keep present value.

*Verbose*

Enable or disable the verbosity flag. Enabling this option makes the GeoUTime give visual feedback on the steps it performs, disabling will make GeoUTime perform its actions without any visual feedback.

Enabling is useful for debugging hostname and UTC offset settings, or if you just are curious. Disabling is recommended for day to day use as auto executing application at boot time.

Selecting this option gives this dialogue:

![Verbose dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20time%20verbosity.png)

Select Yes for verbose mode, No for silent mode.

## Information menu

Only option in this menu is *Credits*. Selecting this option shows the application credits:

![GeoUTime credits](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUTimeCfg%20information%20credits.png)

Press OK to leave the credits dialogue box.

## GeoUTime instructions
([Back to contents](#contents))

This application is driven by the options stored in the configuration file and does not need further user interaction.

It is an auto exec application, so it should be started automatically if placed on the boot disk/drive. Therefore it is recommended to use verbosity only incidentally and switch it off after use. It is also possible to start the application manually.

If verbosity is set to no in the configuration file options, nothing will be shown on the screen apart from the screen blanking during execution and the system time being updated after returning to the desktop.

If verbosity mode is 'On' and NTP enable is 'On', something like this will be shown on the screen:

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

If NTP enable is switched to off, only the synch between UII+ RTC and the GEOS system time takes place, no NTP time request will be performed.

## GeoUMount instructions
([Back to contents](#contents))

This application enables to mount disk images on the UII+ filesystem to an UII+ emulated drive in GEOS via a filebrowser.

### Application limitations and considerations
The application ensures only disk images can be selected to mount that correspond with the drive type of the target drive by filtering the filelist to show only the corresponding image type.

This is done as GEOS crashes if the drive type of any of the active GEOS drive is changed on the fly by for example mounting a .D64 image to a 1581 emulated drive. As GEOS is not aware of the changed disk type, it will load the wrong turbo load code on the drive, which makes GEOS crash or freeze. By filtering the filelist, this should be prevented.

To properly enable filtering, image filenames should end with the proper corresponding .dxx extension:

- .d64 for 1541 drive type
- .d71 for 1571 drive type
- .d81 for 1581 drive type

Other image types ate not presently supported.

Also note that file and dirnames can only be selected if they have a maximum name length of 20 characters, including extension. Reason is memory constraints: allowing for longer filenames would cause less filenames that can be loaded in memory. If you need to select dirs or files with longer names, shorten them first to 20 chars (including extension) at most.

Maximum number of files shown is dependent on the free memory on the target system used, but should normally be between 170-180 files. If you have more valid images or subdirectories in your present directory, any entry over that number will not be shown. If you need to reach these dirs or files, consider reorganizing your dir to place the files in subdirs with fewer entries.

### Main interface

After start, this main interface is shown:

![GeoUMount main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mainscreen.png)

### Main menu

**GEOS menu**

Options in this menu:

![GEOS menu](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20geos%20menu.png)

*Switch 40/80*

(Only works in GEOS128, ignored in GEOS64)

Switches between 40 and 80 column mode. On selecting, the other mode will be selected and the screen will be redrwam. If a single monitor is used, switch monitor to the corresponding other mode to view.

*Exit*

Exits to the GEOS desktop and quits the application.

**Credits**

This menu option shows this dialogue:

![Credits GeoUMount](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20credits.png)

### Selecting the target drive

The target drive selection area is the upper right area in the main interface:

![Target drive area](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20drivetarget.png)

In this area you can select the target drive to mount the selected disk image to.

The text right of the icons A, B, C and D show if this drive letter can be selected as valid target to mount images to via the Ultimate II+ cartridge. Obviously only those drive letters that correspond active drives emulated by the UII+ can selected. If the drive letter is not a valid target, the text 'No target' is shown to the right of the button. If it corresponds to a valid target, the type of the emulated drive is presented (supported for now are emulated 1541, 1571 and 1581 drives).

GeoUMount presently does not support changing this drive configuration from within the application. Use the standard Configure app to change drive configuration if desired.

Pressing the icon of a valid drive target wull change the target to this drive. If the drive type of the new target drive is different than the type of the old target, a refresh of the presently shown directory will be triggered to apply filtering for this new drive type.

### Filebrowser navigation icons

This section is the lower right area of the main interface:

![Navgiation icons section](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20navicons.png)

|Icon|Function|
|---|---|
|![Back icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20icon.png)|Pressing this icon moves the present directory shown to the parent directory of the presently selected directory.|
|![Root icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20root.png)|Pressing this icon moves the present directory shown to the root directory of the filesystem.|
|![Home icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20home.png)|Pressing this icon moves the present directory shown to the directory configured as home dir in the UII+ interface options.|
|![Top icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20pageup.png)|Pressing this icon moves to the top of the filelist in the present showb directory.|
|![Bottom icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20pagedown.png)|Pressing this icon moves to the bottom of the filelist in the present showb directory.|

### Filebrowser: pathname

![Filebrowser path name](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20filebrowser%20id%20path.png)

This section shows the path of the presently selected directory, next to the identification (Ultimate DOS version) of the UII+ file system Ultimate Command Interface DOS target.

### Filebrowser: line scroll arrows

![Filebrowser linescroll](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20filebrowser%20linescroll.png)

By clicking on the arrow icons in the filebrowser area you can scroll one entry up (upper arrow icon) or down (lower arrow icon).

### Filebrowser: page up and page down

![Filebrowser page scroll](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20filebrowser%20pagescroll.png)

By clicking on the upper and lower scrollbar areas in the filebrowser area you can scroll one page up (upper scrollbar area) or down (lower scrollbar area).

### Filebrowser: filelist

![Filebrowser filelist](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20filebrowser%20dirs%20and%20files.png)

This area shows the visible part of the file and directory list of the presently selected directory. 13 files or directories can be shown visibly, use the scroll or navigation icons/areas to move the visible part.

In the left column the name of the file or directory is shown, in the right column the type.

Tupes:

![Types](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20too%20long.png)

|Code|Description|
|---|---|
|DIR|Directory|
|D64|1541 images with .d64 extension|
|D71|1571 images with .d71 extension|
|D81|1581 images with .d81 extension|
|!TL|Name to long to be fully shown. Can not be selected without shortening name|

**Moving to a subdirectory**

Move to a subdirectory by clicking on the desired directory name shown.

**Selecting an image to mount**

Select an image to mount by clicking on a valid filename (so type .d64, .d71 or .d81 dependent on selected target drive).

On success, the following dialogue is presented:

![Image mount dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mount%20confirm.png)

If this is shown, the selected image is already is mounted.

Choose Cancel if you want to proceed with GeoUMount (for example if you selected a wrog image).

Choose OK to exit the GeoUMount application and go back to the GEOS desktop with the new image mounted to the selected target drive.

## Credits
([Back to contents](#contents))

### GeoUTools

Tools for using the UII+ cartridge with GEOS

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

