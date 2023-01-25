// ====================================================================================
// vdcscroll.s
// Header file for VDC scrolling
//
// Credits for code and inspiration:
//
// C128 Programmers Reference Guide:
// http://www.zimmers.net/anonftp/pub/cbm/manuals/c128/C128_Programmers_Reference_Guide.pdf
//
// Scott Hutter - VDC Core functions inspiration:
// https://github.com/Commodore64128/vdc_gui/blob/master/src/vdc_core.c
// (used as starting point, but channged to inline assembler for core functions, added VDC wait statements and expanded)
//
// Francesco Sblendorio - Screen Utility:
// https://github.com/xlar54/ultimateii-dos-lib/blob/master/src/samples/screen_utility.c
//
// DevDef: Commodore 128 Assembly - Part 3: The 80-column (8563) chip
// https://devdef.blogspot.com/2018/03/commodore-128-assembly-part-3-80-column.html
//
// Tips and Tricks for C128: VDC
// http://commodore128.mirkosoft.sk/vdc.html
//
// 6502.org: Practical Memory Move Routines
// http://6502.org/source/general/memory_move.html
//
// =====================================================================================

#ifndef _VDCSCROLL_H_
#define _VDCSCROLL_H_

// Variables in core Functions
extern unsigned char VDC_addrh;
extern unsigned char VDC_addrl;
extern unsigned char VDC_desth;
extern unsigned char VDC_destl;
extern unsigned char VDC_tmp1;
extern unsigned char VDC_tmp2;

// Import assembly core Functions
void VDC_ScrollCopy_core();

// Function Prototypes
void VDC_ScrollCopy(unsigned int sourcebase, unsigned char sourcebank, unsigned int sourcewidth, unsigned int sourceheight, unsigned int sourcexoffset, unsigned int sourceyoffset, unsigned char xcoord, unsigned char ycoord, unsigned char viewwidth, unsigned char viewheight, unsigned char direction);

#endif