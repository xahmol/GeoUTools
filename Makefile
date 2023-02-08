
# GeoUTools
# Tools for using the UII+ cartridge with GEOS

# Prerequisites for building:
# - CC65 compiled for Linux and included in path with sudo make avail
# - GEOSBuild tool compiled for Linux using Lazarus IDE.
#   See compiling instructions at https://github.com/M3wP/GEOSBuild
#   Provide path to tool below
# - VICE /bin directory, or at least the c1541 tool, in the path environment
#   and compiled for Linux
#   https://vice-emu.sourceforge.io/
# - ZIP packages installed: sudo apt-get install zip
# - wput command installed: sudo apt-get install wput

# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= geos-cbm

# If SYS was given on the commandline, redirect "c64" to "geos-cbm" and
# "apple2enh" to "geos-apple"
ifeq ($(origin SYS),command line)
  ifeq ($(SYS),c64)
    override SYS = geos-cbm
    $(info GEOS: c64 -> geos-cbm)
  endif
  ifeq ($(SYS),apple2enh)
    override SYS = geos-apple
    $(info GEOS: apple2enh -> geos-apple)
  endif
endif

# Just the usual way to find out if we're
# using cmd.exe to execute make rules.
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL = -del /f
  RMDIR = rmdir /s /q
else
  NULLDEV = /dev/null
  DEL = $(RM)
  RMDIR = $(RM) -r
endif

ifdef CC65_HOME
  AS = $(CC65_HOME)/bin/ca65
  CC = $(CC65_HOME)/bin/cc65
  CL = $(CC65_HOME)/bin/cl65
  LD = $(CC65_HOME)/bin/ld65
  SP = $(CC65_HOME)/bin/sp65
else
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
  SP := $(if $(wildcard ../../bin/sp65*),../../bin/sp65,sp65)
endif

# Path to GEOSBuild tool
GBUILD = /home/xahmol/geostools/GEOSBuild

# Application names
SUITE = geoutools
MOUNT = geoumount
TIME = geoutime
CONF = geoutimecfg

# Common compile flags
CFLAGS  = -t $(SYS)  -Os -I include

# Sources and config for GeoUMount
MOUNTSRC = src/mount.c src/interface.c src/ultimate_common_lib.c src/ultimate_dos_lib.c
MOUNTHDR = src/$(MOUNT)Hdr.s
MOUNTFLG = -C $(MOUNT)_cc65config.cfg -m $(MOUNT).map

# Sources and config for GeoUTime
TIMESRC = src/time_synch.c src/time_common.c src/ultimate_common_lib.c src/ultimate_time_lib.c src/ultimate_network_lib.c
TIMEHDR = src/$(TIME)Hdr.s
TIMEFLG = -C $(TIME)_cc65config.cfg -m $(TIME).map

# Sources and config for GeoUTime Config
CONFSRC = src/time_config.c src/time_common.c src/interface.c
CONFHDR = src/$(CONF)Hdr.s
CONFFLG = -C $(CONF)_cc65config.cfg -m $(CONF).map

# Hostname of Ultimate II+ target for deployment. Edit for proper IP and usb number
ULTHOST = ftp://192.168.1.19/usb1/11/
ULTHOST2 = ftp://192.168.1.31/usb1/11/
ULTHOST3 = ftp://192.168.1.55/usb0/Geos/

# Data for ZIP file
ZIP = GeoUTools-v01-$(shell date "+%Y%m%d-%H%M").zip
ZIPLIST = $(SUITE).d64 $(SUITE).d81 readme.pdf

########################################

.SUFFIXES:
.PHONY: all clean deploy vice
all: $(MOUNT).bin $(TIME).bin $(CONF).bin $(SUITE).d81 $(SUITE).d64 $(ZIP)

# Building GeoUMount
$(MOUNT).bin: $(MOUNT).grc $(MOUNTSRC) $(MOUNTHDR)
	$(CL) $(CFLAGS) $(MOUNTFLG) -o $(MOUNT).bin $(MOUNT).grc $(MOUNTSRC) $(MOUNTHDR)

# Building GeoUTime
$(TIME).bin: $(TIME).grc $(TIMESRC) $(TIMEHDR)
	$(CL) $(CFLAGS) $(TIMEFLG) -o $(TIME).bin $(TIME).grc $(TIMESRC) $(TIMEHDR)

# Building GeoUTime Config
$(CONF).bin: $(CONF).grc $(CONFSRC) $(CONFHDR)
	$(CL) $(CFLAGS) $(CONFFLG) -o $(CONF).bin $(CONF).grc $(CONFSRC) $(CONFHDR)

# Creating suite .d81 disk image
$(SUITE).d81:		$(MOUNT).bin $(MOUNT)Hdr.bin $(MOUNT)d81.gbuild $(TIME).bin $(TIME)Hdr.bin $(TIME)d81.gbuild $(CONF).bin $(CONF)Hdr.bin $(CONF)d81.gbuild
	$(DEL) $(SUITE).d81 2>$(NULLDEV)
	$(GBUILD) $(MOUNT)d81.gbuild
	$(GBUILD) $(TIME)d81.gbuild
	$(GBUILD) $(CONF)d81.gbuild

# Creating suite .d64 disk image
$(SUITE).d64:		$(MOUNT).bin $(MOUNT)Hdr.bin $(MOUNT)d64.gbuild $(TIME).bin $(TIME)Hdr.bin $(TIME)d64.gbuild $(CONF).bin $(CONF)Hdr.bin $(CONF)d64.gbuild
	$(DEL) $(SUITE).d64 2>$(NULLDEV)
	$(GBUILD) $(MOUNT)d64.gbuild
	$(GBUILD) $(TIME)d64.gbuild
	$(GBUILD) $(CONF)d64.gbuild

# Creating ZIP file for distribution
$(ZIP): $(ZIPLIST)
	zip $@ $^

# Cleaning repo of build files
clean:
	$(DEL) $(MOUNT)*.bin $(MOUNT).map $(TIME)*.bin $(TIME).map $(CONF)*.bin $(CONF).map 2>$(NULLDEV)

# To deploy software to UII+ enter make deploy. Obviously C128 needs to powered on with UII+ and USB drive connected.
deploy: $(SUITE).d81
	wput -u $(SUITE).d81 $(ULTHOST)
	wput -u $(SUITE).d81 $(ULTHOST2)
#	wput -u $(SUITE).d81 $(ULTHOST3)