# GeoUTools
# Tools for using the Ultimate II+ / Ultimate 64 / C64 Ultimate with GEOS:
# GeoUMount (disk image mounter), GeoUTime (NTP time synch, auto-exec) and
# GeoUConfig (settings). GEOS 64 and GEOS 128 (40/80 columns).
#
# Built with Oscar64 and the geos-oscar64 library; see docs/ARCHITECTURE.md.

# Cross-platform shell detection
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL     = -del /f
  RMDIR   = rmdir /s /q
  MKDIR   = mkdir
else
  NULLDEV = /dev/null
  DEL     = $(RM)
  RMDIR   = $(RM) -r
  MKDIR   = mkdir -p
endif

# Local settings (gitignored): Ultimate IPs, compiler, library paths
-include .env

# Toolchain -- override in .env if oscar64 lives elsewhere. Programs with
# sprintf need an Oscar64 with the fast-call range fix (geos-oscar64
# manual, section 10). Plain '=' for CC is wrong here: CC is a make
# built-in, so set OSCAR64 instead.
OSCAR64     ?= /home/xahmol/oscar64/bin/oscar64
OSCAR64_INC ?= $(dir $(OSCAR64))../include
CC           = $(OSCAR64)

# Libraries (git submodules, pinned to release tags; never edit lib/)
GEOSLIB ?= lib/geos-oscar64
UCILIB  ?= lib/ultimate-uci-oscar64

# Build versioning
VERSION_MAJOR     = 2
VERSION_MINOR     = 0
VERSION_PATCH     = 0
VERSION_TIMESTAMP = $(shell date "+%Y%m%d-%H%M")
VERSION           = v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)-$(VERSION_TIMESTAMP)
GEOSVERSION       = V$(VERSION_MAJOR).$(VERSION_MINOR)

# Compile flags
#   -tm=atari -tf=bin -n -rt=...  : GEOS target (geos-oscar64 manual, section 1)
#   -i=src                        : project sources and headers
#   -i=GEOSLIB, UCILIB            : libraries
#   -O2                           : optimise
#   -dNOFLOAT                     : no float support in printf
#   -dVERSION                     : version string
CFLAGS = -ii=$(OSCAR64_INC) \
         -i=src \
         -i=$(GEOSLIB)/include \
         -i=$(UCILIB)/include \
         -tm=atari \
         -tf=bin \
         -n \
         -rt=$(GEOSLIB)/include/geos/geos_crt.c \
         -O2 \
         -dNOFLOAT \
         -dVERSION="\"$(VERSION)\""

# GeoUTime runs as AUTO_EXEC at boot: the kernal's boot code still lives at
# $5000-$5FFF then, so its program memory ends at $5000.
TIMEFLAGS = -dGEOS_PROG_END=0x5000

GEOSAPP = python3 -I $(GEOSLIB)/tools/geosapp.py

# Sources (Oscar64 follows #pragma compile chains; listed so make rebuilds)
LIBSRCS    = $(wildcard $(GEOSLIB)/include/*.h $(GEOSLIB)/include/geos/*.h $(GEOSLIB)/include/geos/*.c) \
             $(wildcard $(UCILIB)/include/*.c $(UCILIB)/include/*.h)
COMMONSRCS = src/defines.h src/uci_io.h src/uci_io.c
TIMESRCS   = src/geoutime.c src/time_common.c src/time_common.h $(COMMONSRCS) $(LIBSRCS)
CONFSRCS   = src/geouconfig.c src/time_common.c src/time_common.h \
             src/mount_common.c src/mount_common.h \
             src/interface.c src/interface.h $(COMMONSRCS) $(LIBSRCS)
MOUNTSRCS  = src/geoumount.c src/mount_common.c src/mount_common.h \
             src/interface.c src/interface.h $(COMMONSRCS) $(LIBSRCS)

# Outputs
SUITE   = GeoUTools
APPS    = build/geoumount.cvt build/geoutime.cvt build/geouconfig.cvt
DISKS   = build/$(SUITE).d64 build/$(SUITE).d81
MANUALS = docs/geowrite-manual-en.cvt docs/geowrite-manual-de.cvt
ZIPFILE = build/$(SUITE)-$(VERSION).zip
README  = README.pdf README_de.pdf

# Ultimate device deployment target. Store only the IP in .env.
ULTIP1  ?= <set_ULTIP1_in_.env>
ULTUSB  ?= usb0
ULTPATH ?= Dev
ULTFTP1  = ftp://$(ULTIP1)/$(ULTUSB)/$(ULTPATH)/
ULTUSB2 ?= $(ULTUSB)
ifdef ULTIP2
ULTFTP2  = ftp://$(ULTIP2)/$(ULTUSB2)/$(ULTPATH)/
endif

.SUFFIXES:
.PHONY: all clean apps disks zip docs deploy check-deploy deploy2 check-deploy2

all: apps disks $(README) zip

apps: $(APPS)

# ---- applications -----------------------------------------------------

build/geoutime.bin: $(TIMESRCS)
	@$(MKDIR) build 2>$(NULLDEV) ; true
	$(CC) $(CFLAGS) $(TIMEFLAGS) -o=$@ src/geoutime.c

build/geouconfig.bin: $(CONFSRCS)
	@$(MKDIR) build 2>$(NULLDEV) ; true
	$(CC) $(CFLAGS) -o=$@ src/geouconfig.c

build/geoumount.bin: $(MOUNTSRCS)
	@$(MKDIR) build 2>$(NULLDEV) ; true
	$(CC) $(CFLAGS) -o=$@ src/geoumount.c

build/%.cvt: build/%.bin assets/%.toml
	$(GEOSAPP) assets/$*.toml $< $@ --set version=$(GEOSVERSION)

# ---- disk images ----------------------------------------------------------

disks: $(DISKS)

build/$(SUITE).d64: $(APPS) $(MANUALS)
	$(DEL) $@ 2>$(NULLDEV) ; true
	c1541 -format "geoutools,1A" d64 $@ >$(NULLDEV)
	for f in $(APPS) $(MANUALS); do c1541 -attach $@ -geoswrite "$$f" >$(NULLDEV) || exit 1; done

build/$(SUITE).d81: $(APPS) $(MANUALS)
	$(DEL) $@ 2>$(NULLDEV) ; true
	c1541 -format "geoutools,1A" d81 $@ >$(NULLDEV)
	for f in $(APPS) $(MANUALS); do c1541 -attach $@ -geoswrite "$$f" >$(NULLDEV) || exit 1; done

# ---- documentation ----------------------------------------------------

# Regenerate the PDFs (pandoc + texlive-xetex); warns and skips without pandoc.
docs: $(README)

%.pdf: %.md pandoc-defaults.yaml pandoc-header.tex
	@if which pandoc >$(NULLDEV) 2>&1; then \
		pandoc --defaults=pandoc-defaults.yaml $< -o $@; \
	else \
		echo "WARNING: pandoc not found -- $@ not updated (install: sudo apt install pandoc texlive-xetex)"; \
	fi

# ---- distribution ---------------------------------------------------------

zip: $(DISKS) $(README)
	cd build && zip -j $(SUITE)-$(VERSION).zip $(SUITE).d64 $(SUITE).d81 ../README.pdf ../README_de.pdf

clean:
	$(RMDIR) build 2>$(NULLDEV) ; true

# ---- deployment to an Ultimate ----------------------------------------------

check-deploy:
	@curl -s --connect-timeout 3 ftp://$(ULTIP1)/ >$(NULLDEV) 2>&1 || \
		(echo "ERROR: Cannot reach Ultimate device at $(ULTIP1) -- check ULTIP1 in .env" && false)

deploy: check-deploy $(DISKS)
	wput -u build/$(SUITE).d81 $(ULTFTP1)

check-deploy2:
ifndef ULTIP2
	$(error ULTIP2 is not set -- add it to .env to use deploy2)
endif
	@curl -s --connect-timeout 3 ftp://$(ULTIP2)/ >$(NULLDEV) 2>&1 || \
		(echo "ERROR: Cannot reach Ultimate device at $(ULTIP2) -- check ULTIP2 in .env" && false)

deploy2: check-deploy2 $(DISKS)
	wput -u build/$(SUITE).d81 $(ULTFTP2)
