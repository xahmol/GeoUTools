# GeoUTools
Tools for using the UII+ cartridge with GEOS

## Contents

[Version history and download](#version-history-and-download)

[Firmware and setting prerequisites and known issues](#firmware-and-setting-prerequisites-and-known-issues)

[Introduction](#introduction)

[What is on the disk](#what-is-on-the-disk)

[GeoUConfig instructions](#geouconfig-instructions)

[GeoUTime instructions](#geoutime-instructions)

[GeoUMount instructions](#geoumount-instructions)

[Credits](#credits)

![GeoUMount main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mainscreen.png)

## Version history and download
([Back to contents](#contents))

[Link to latest build](https://github.com/xahmol/GeoUTools/raw/main/GeoUTools-v10-20230520-0004.zip)

Version 1.0 - 20230517-1759:
- Official 1.0 release
- Updated documentation from 0.1 to 1.0 version

Version 0.3 - 20230512-1251:
- Again, many bugfixes
- Changed the name of GeoUTimeCfg to GeoUConfig as it now can also set configuration settings for GeoUMount
- Via GeoUConfig, now the automatic valid targets detection of GeoUMount can be manually changed in case of older firmware or issues with automatic detection. Also, the default target drive can be changed by the user.

Version 0.2 - 20230423-1542:
- Many bugfixes and code improvements fixing issues that was causing the application to crash in many GEOS configurations
- Added support for new firmware functionality to detect GEOS RAM drives that support image mounting and image saving from the Ultimate Command Interface.
- Better failsafe for older firmware


Version 0.1 - 20230128 - 2223:
- First public alpha version

## Firmware and setting prerequisites and known issues
([Back to contents](#contents))

### Ultimate II/II+/II+L firmware requirements

- Firmware 3.10f or newer: fully automatic drive detection and support to detect/mount/save GEOS RAM drives
- Firmware 3.xx: automatic device detection possible. But in firmware older than 3.10f automatic device detection is based on hardware drive IDs of the emulated A and B drives of the Ultimate II/II+/II+L cartridge, instead of the software ID. As in many GEOS configurations the software IDs of drives are changed by GEOS this can create issues in device detection going wrong and, as a result. unexpected results on mounting images to these drives. To fix this, choose manual override of the automated detected targets using GeoUConfig.
- Firmware before 3.xx: Automatic target detection is not possible, so setting valid drive targets via GeoUConfig is always needed for GeoUMount to work.

Link to Ultimate II/II+/II+L firmware:

- Official released versions: https://ultimate64.com/Firmware
- GitHub: https://github.com/GideonZ/ultimate_releases (might contain newer not yet officially released firmware, or firmware that were temporarily retracted)

### Ultimate II/II+/II+L setting requirements

![Ultimate Cartridge Settings](https://github.com/xahmol/GeoUTools/blob/main/screenshots/u2p-settings.png?raw=true)

For GeoUTools it is important to check these settings in the C64 and Cartridge Settings menu after pressing F2 from the UI:
- Command Interface: **IMPORTANT** This setting has to be set to 'enabled' for anything in GeoUTools to work
- UltiDOS: Allow SetDate: This setting has to be set to 'enabled' to allow GeoUTime to change the Ultimate Real Time Clock time
- RAM Expansion Unit: Should be set to 'enabled' to support detecting, mounting, and saving GEOS RAM drives
- REU Size: set the desired size of the REU here, maximum is 16 megabytes. Depending on the GEOS configuration and GEOS distribution used, up to 4 megabytes can be used by GEOS itself with the remaining 12 megabytes available for RAM Native ram drives (the latter only supported in distributions as Wheels or Megapatch)

## Introduction
([Back to contents](#contents))

**GeoUTools** is a little suite of (for now) three utilities to help using the Ultimate II+ cartridge from within GEOS.

### GeoUTime

Utility to set the GEOS system time (and the UII+ RTC clock) the time received from a user defined NTP server.

GeoUTIme is an auto exec application that starts automatically on boot. For this reason, the verbosity setting by default is 'Off' so not showing dialogues on every boot. But it can be switched on for debugging (or curiosity what happens).

### GeoUMount
This utility can be used to swap and mount virtual disks on your UII+ cartridge, helpful because the UII+ menu is not available under GEOS128 80 column mode and often crashes under GEOS128 40 column mode. However therefore mostly useful under GEOS128, the program should work in all flavours of GEOS on the C64 and C128 as long as that computer has an UII+ cartridge.

Images can be selected from a file browser, that filters on image types fitting the target drive.

If supported by a new enough firmware, also GEOS RAM drives residing in REU memory are supported for mounting and image saving.

Number of direntries in this build tops around 250 or so (rest of the dir is not shown). Which, also considering the filtering on dirs and fitting image types only, is more than enough for my personal needs.

### GeoUConfig

Utility to set the configuration for GeoUTime and GeoUMount.
For GeoUTime, enabling or disabling time query of an NTP server, the NTP server hostname, offset from UTC time in seconds and verbosity on or off can be set.
For GeoUMount, automatic valid target detection can be manually overridden, and the default target drive can be set.

## What is on the disk
([Back to contents](#contents))

On first mount from the disk image downloaded from GitHub, the disk is not yet converted to a GEOS disk, so you might (depending on GEOS distribution used) get this message:

![Non GEOS disk message](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20nongeosdisk.png)

Click on Yes in this dialogue. You will then see these contents on the disk:

![GeoUTools disk clean](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20contentsfirst.png)

- **GeoUMount**: Disk mounting tool
- **GeoUTime**: NTP time synching tool
- **GeoUConfig**: Utility to set configuration options for GeoUTime

Double click on the desired icon of the three to start the corresponding utility.

Note that on first use, both GeoUTime and GeoUConfig create a configuration file on the disk if no one is yet present. After this, the contents of the disk look like this:

![Configuration file added on disk](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20contents%20data.png)

A new file called GeoUTimeDat is added to store configuration data.

Obviously, you are free (even encouraged) to copy these files to another disk target, such as your main RAM drive.

## GeoUConfig instructions
([Back to contents](#contents))

This application sets the configuration options for GeoUTIme and saves them in the GeoUTimeDat configuration file. It creates this file on first start if no file is present already.

Interface after application start, showing the present configuration settings:

![GeoUTimeCfg main interface](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mainscreen.png?raw=true)

The options can be reached via the main menu in the top left corner. The interface will update on changed settings. All changed will be automatically saved.

Under 'GeoUMount config data' you can also see if autodetection of valid drive targets succeeded. This line can show the following options:

- Autodetection of valid drives succeeded: Firmware supports improved automated drive detection (firmware 3.10f and newer)
- Autodetection might be incorrect: Autodetection is supported, but based on hardware ID instead of software ID, so it might be wrong. Check the detected settings and manually adjust if needed (firmware 3.xx up to 3.10e)
- Autodetection not supported in firmware: Autodetection is not supported, so manual selection of valid targets is required. Detection override is forced on 'Enabled' (firmware before 3.xx)

### GEOS menu

Options in this menu:

![GEOS menu](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20geos%20menu.png?raw=true)

*Switch 40/80*

(Only works in GEOS128, ignored in GEOS64)

Switches between 40 and 80 column mode. On selecting, the other mode will be selected, and the screen will be redrawn. If a single monitor is used, switch monitor to the corresponding other mode to view.

*Credits*

Selecting this option shows the application credits:

![GeoUTime credits](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20GEOS%20credits.png?raw=true)

Press OK to leave the credits dialogue box.

*Exit*

Exits to the GEOS desktop and quits the application.

### Time menu

Options in this menu:

![Time menu](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20time%20menu.png?raw=true)

*Enable NTP*

Choose if time should be synched with the selected NTP server (setting is 'On'), or if only the GEOS system time should be synched to the UII+ Real Time Clock without an NTP request.

Selecting this option gives this dialogue:

![NTP enable dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20time%20ntp%20enable.png?raw=true)

Select Yes to enable, No to disable.

*Hostname*

Enter the hostname of the desired NTP server. The default server is pool.ntp.org.

This option results in this dialogue:

![Hostname dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20%20time%20hostname.png?raw=true)

For another NTP server, just enter its hostname (URL or IP). The application will not check validity of the input, to check if it works and the hostname resolves and results in a successful connection the verbose mode is suggested.

To keep the present hostname, press either CANCEL or ENTER.

*UTC offset*

Edits the time offset to UTC (Universal standard time). The offset needs to be provided in seconds. Automated adjustment for daylight savings ('Summer' and 'Winter' time) is not provided, so you have to adjust your offset on the change from daylight saving time to not.

Offset is set in seconds (to allow for half hour difference time zones and also for finetuning if desired), so multiply the UTC offset in hours by 3600. And note that the offset is signed, so start with a minus for offsets negative to UTC.

Default is set on Central European Time, which requires an offset of 3600. Setting to Central European Summer Time would require 7200. Another example: setting to Eastern Standard Time (EST, time zone for a.o. New York City) would be UTC -5, so -5*3600 = -18000.

See https://www.timeanddate.com/time/zones/ for all offsets in hours (multiply by 3600 to get to seconds).

This option presents this dialogue:

![UTC offset dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20%20time%20utc%20offset.png?raw=true)

Type the desired offset (noting the signing, so do not forget the minus sign if offsets are negative to UTC) and press ENTER. CANCEL to keep present value.

*Verbose*

Enable or disable the verbosity flag. Enabling this option makes the GeoUTime give visual feedback on the steps it performs, disabling will make GeoUTime perform its actions without any visual feedback.

Enabling is useful for debugging hostname and UTC offset settings, or if you just are curious. Disabling is recommended for day to day use as auto executing application at boot time.

Selecting this option gives this dialogue:

![Verbose dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20%20time%20verbosity.png?raw=true)

Select Yes for verbose mode, No for silent mode.

### Mount menu

Options in this menu:

![Mount menu](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mount%20menu.png?raw=true)

*Enable override*

With this menu option you can enable or disable overriding the automated valid target detection for GeoUmount. This menu option gives this dialogue box:

![Enable override dialogie](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mount%20override.png?raw=true)

Select Yes to enable manual override, No to use automatic target detection.

*Set drive*

With this menu option you can enable or disable drive A to D as valid target via this submenu:

![Set drive submenu](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mount%20set%20drive.png?raw=true)

Choose the desired drive to change from this submenu. This gives this dialogue box:

![Set drive as valid dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mount%20set%20drive%20valid.png?raw=true)

Select Yes to set the drive as valid target, No to select drive not to be a valid target.

*Set target*

With this menu option you can set the default active target drive. Without setting this manually, the target drive is set as the first encountered valid target.

You can select the target drive via the submenu:

![Set target submenu](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUConfig%20mount%20set%20target.png?raw=true)

Selecting a drive via this submenu will change the target setting. Selecting Auto will revert to just selecting the first encountered valid drive.

## GeoUTime instructions
([Back to contents](#contents))

This application is driven by the options stored in the configuration file and does not need further user interaction.

It is an auto exec application, so it should be started automatically if placed on the boot disk/drive. Therefore, it is recommended to use verbosity only incidentally and switch it off after use. It is also possible to start the application manually.

If verbosity is set to no in the configuration file options, nothing will be shown on the screen apart from the screen blanking during execution and the system time being updated after returning to the desktop.

If verbosity mode is 'On' and NTP enable is 'On', something like this will be shown on the screen:

![GeoUTime verbosity output](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20geoutime%20verbose%20output.png)

This mode is meant to either debug your configuration settings, or if you are simply curious what happens:
- Connecting to the selected hostname of the desired NTP server
- Sending the data request to that server
- Reiving the data and interpreting result
- Showing UNIX epoch received
- Converting that output to time in selected time zone using the UTC offset
- Setting the UII+ Real Time Clock with that time
- Synching GEOS system time with the UII+ RTC clock.
- Confirming success

Click the Quit icon to exit.

Note that on any NTP connection error, the GEOS clock will be synched to the unchanged UII+ RTC clock as fallback.

If NTP enable is switched to off, only the synch between UII+ RTC and the GEOS system time takes place, no NTP time request will be performed.

## GeoUMount instructions
([Back to contents](#contents))

This application enables to mount disk images on the UII+ filesystem to an UII+ emulated drive or GEOS RAM drive (if enabled and supported in firmware version) in GEOS via a file browser.

### Application limitations and considerations
The application ensures only disk images can be selected to mount that correspond with the drive type of the target drive by filtering the file list to show only the corresponding image type.

This is done as GEOS crashes if the drive type of any of the active GEOS drive is changed on the fly by for example mounting a .D64 image to a 1581 emulated drive. As GEOS is not aware of the changed disk type, it will load the wrong turbo load code on the drive, which makes GEOS crash or freeze. By filtering the file list, this should be prevented.

To properly enable filtering, image filenames should end with the proper corresponding .dxx extension:

- .d64 for 1541 drive type or 1571 drive type
- .g64 for 1541 drive type or 1571 drive type
- .d71 for 1571 drive type
- .d81 for 1581 drive type
- .dnp for RAM Native drive images

Other image types are not presently supported.

Also note that file and dirnames can only be selected if they have a maximum name length of 20 characters, including extension. Reason is memory constraints: allowing for longer filenames would cause less filenames that can be loaded in memory. If you need to select dirs or files with longer names, shorten them first to 20 chars (including extension) at most.

Maximum number of files shown is dependent on the free memory on the target system used but should normally be around 250 files. If you have more valid images or subdirectories in your present directory, any entry over that number will not be shown. If you need to reach these dirs or files, consider reorganizing your dir to place the files in subdirs with fewer entries.

### Main interface

After start, this main interface is shown:

![GeoUMount main interface](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mainscreen.png)

### Main menu

**GEOS menu**

Options in this menu:

![GEOS menu](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20geos%20menu.png)

*Switch 40/80*

(Only works in GEOS128, ignored in GEOS64)

Switches between 40 and 80 column mode. On selecting, the other mode will be selected, and the screen will be redrawn. If a single monitor is used, switch monitor to the corresponding other mode to view.

*Credits*

This menu option shows this dialogue:

![Credits GeoUMount](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20credits.png)

*Exit*

Exits to the GEOS desktop and quits the application.

**Save REU menu**

In this menu the contents of the REU memory can be saved to a .reu file in the presently active directory.

Select the memory size to save via this submenu:

![Save REU choose memory size to save](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20reu%20menu.png?raw=true)

Then a dialogue box asks for a filename for the image to save. Enter filename or press Cancel to cancel.

![Save REU filename dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20reu%20filename.png?raw=true)

If already a file with that name exists in the active directory, a new dialogue asks to confirm or cancel.

![Save REU file exists dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20reu%20file%20exists.png?raw=true)

Depending on selected memory size, saving can take a while. After success, a message like this is shown.

![Save REU succes](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20reu%20file%20succes.png?raw=true)


### Selecting the target drive

The target drive selection area is the upper right area in the main interface:

![Target drive area](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20drivetarget.png)

In this area you can select the target drive to mount the selected disk image to.

The text right of the icons A, B, C and D show if this drive letter can be selected as valid target to mount images to via the Ultimate II+ cartridge. Obviously only those drive letters that correspond active drives emulated by the UII+ can selected. If the drive letter is not a valid target, the text 'No target' is shown to the right of the button. If it corresponds to a valid target, the type of the emulated drive is presented (supported for now are emulated 1541, 1571 and 1581 drives).

GeoUMount presently does not support changing this drive configuration from within the application. Use the standard Configure app to change drive configuration if desired.

Pressing the icon of a valid drive target will change the target to this drive. If the drive type of the new target drive is different than the type of the old target, a refresh of the presently shown directory will be triggered to apply filtering for this new drive type.

### File browser navigation icons

This section is the lower right area of the main interface:

![Navgiation icons section](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20navicons.png)

|Icon|Function|
|---|---|
|![Back icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20icon.png)|Pressing this icon moves the present directory shown to the parent directory of the presently selected directory.|
|![Root icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20root.png)|Pressing this icon moves the present directory shown to the root directory of the filesystem.|
|![Home icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20home.png)|Pressing this icon moves the present directory shown to the directory configured as home dir in the UII+ interface options.|
|![Top icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20pageup.png)|Pressing this icon moves to the top of the file list in the present shown directory.|
|![Bottom icon](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20back%20pagedown.png)|Pressing this icon moves to the bottom of the file list in the present shown directory.|
|![Save icon](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20image.png?raw=true)|Pressing this icon saves the contents of the active RAM drive to an image.|

**Save image icon**

This button only works if the selected target drive is a RAM drive, otherwise clicking this icon will be ignored.

With clicking this icon, the contents of the target RAM drive can be saved to an image file in the presently active directory. Extension will be based on the drive type of the target drive.

First a filename will be asked, enter filename, or click Cancel to cancel.

![Save Image filename dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20image%20filename.png?raw=true)

If a file with this filename already exists, a confirmation dialogue box will pop up. Click Yes to confirm and overwrite the existing file, No to cancel.

![File exists dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20image%20file%20exists.png?raw=true)

Depending on size of the image, saving can take a while. On success, this message will pop up. Click OK to continue.

![Image save success dialogue](https://github.com/xahmol/GeoUTools/blob/main/screenshots/geoutools%20-%20GeoUMount%20save%20image%20file%20succes.png?raw=true)


### File browser: pathname

![file browser path name](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20file browser%20id%20path.png)

This section shows the path of the presently selected directory, next to the identification (Ultimate DOS version) of the UII+ file system Ultimate Command Interface DOS target.

### File browser: line scroll arrows

![file browser linescroll](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20file browser%20linescroll.png)

By clicking on the arrow icons in the file browser area you can scroll one entry up (upper arrow icon) or down (lower arrow icon).

### File browser: page up and page down

![file browser page scroll](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20file browser%20pagescroll.png)

By clicking on the upper and lower scrollbar areas in the file browser area you can scroll one page up (upper scrollbar area) or down (lower scrollbar area).

### File browser: file list

![file browser file list](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20file browser%20dirs%20and%20files.png)

This area shows the visible part of the file and directory list of the presently selected directory. 13 files or directories can be shown visibly, use the scroll or navigation icons/areas to move the visible part.

In the left column the name of the file or directory is shown, in the right column the type.

Types:

![Types](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20too%20long.png)

|Code|Description|
|---|---|
|DIR|Directory|
|D64|1541 images with .d64 extension|
|D71|1571 images with .d71 extension|
|D81|1581 images with .d81 extension|
|!TL|Name too long to be fully shown. Can not be selected without shortening name|
|!IS|DNP image with a size not matching size of target drive|

**Moving to a subdirectory**

Move to a subdirectory by clicking on the desired directory name shown.

**Selecting an image to mount**

Select an image to mount by clicking on a valid filename (so type .d64, .d71 or .d81 dependent on selected target drive).

On success, the following dialogue is presented:

![Image mount dialogue](https://raw.githubusercontent.com/xahmol/GeoUTools/main/screenshots/geoutools%20-%20GeoUMount%20mount%20confirm.png)

If this is shown, the selected image is already mounted.

Choose Yes if you want to proceed with GeoUMount (for example if you selected a wrong image or if you want to mount images on other targets as well).

Choose No to exit the GeoUMount application and go back to the GEOS desktop with the new image mounted to the selected target drive.

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

-   Hitchhikers Guide to GEOS v2022
    Berkeley Softworks / Paul B Murdaugh
    https://github.com/geos64128/HG2G2022

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

-   CBM Files GEOS disk images
    https://cbmfiles.com/geos/geos-13.php

-   GEOS - Wheels - GeoWorks - MegaPatch - gateWay - BreadBox Facebook Group
    https://www.facebook.com/groups/704637082964003/permalink/5839146806179646

-   Bart van Leeuwen for testing and providing the Device Manager ROM and GEOS RAM Boot

-   markusC64 (https://github.com/markusC64)
    for code review, testing and adapting UII+ firmware to enableRAM disk functionality.

-   mkslack ( https://github.com/mkslack ) for code review and testing

-   wweicht for GeoWrite version of manual and testing

-   Forum64.de forum thread for testing and suggestions
    ( https://www.forum64.de/index.php?thread/133681-news-zur-ultimate-1541-ii-und-geos/ )

-   Gideon Zweijtzer for creating the Ultimate II+ cartridge and the Ultimate64, and the Ultimate Command Interface enabling this software.
   
-   Tested using real hardware (C128D, C128DCR, Ultimate 64) using GEOS128 2.0, GEOS64 2.0 and Megapatch3 128.

Licensed under the GNU General Public License v3.0

The code can be used freely as long as you retain
a notice describing original source and author.

THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL,
BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!

