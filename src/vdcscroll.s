; ====================================================================================
; vdcscroll.s
; Core assembly routine for VDC scrolling
;
; Credits for code and inspiration:
;
; C128 Programmers Reference Guide:
; http://www.zimmers.net/anonftp/pub/cbm/manuals/c128/C128_Programmers_Reference_Guide.pdf
;
; Scott Hutter - VDC Core functions inspiration:
; https://github.com/Commodore64128/vdc_gui/blob/master/src/vdc_core.c
; (used as starting point, but channged to inline assembler for core functions, added VDC wait statements and expanded)
;
; Francesco Sblendorio - Screen Utility:
; https://github.com/xlar54/ultimateii-dos-lib/blob/master/src/samples/screen_utility.c
;
; DevDef: Commodore 128 Assembly - Part 3: The 80-column (8563) chip
; https://devdef.blogspot.com/2018/03/commodore-128-assembly-part-3-80-column.html
;
; Tips and Tricks for C128: VDC
; http://commodore128.mirkosoft.sk/vdc.html
;
; 6502.org: Practical Memory Move Routines
; http://6502.org/source/general/memory_move.html
;
; =====================================================================================

	.export		_VDC_ScrollCopy_core
	.export		_VDC_addrh
	.export		_VDC_addrl
	.export		_VDC_desth
	.export		_VDC_destl
	.export		_VDC_tmp1
	.export		_VDC_tmp2

VDC_ADDRESS_REGISTER    = $D600
VDC_DATA_REGISTER       = $D601

.segment	"CODE"

_VDC_regadd:
	.res	1
_VDC_regval:
	.res	1
_VDC_addrh:
	.res	1
_VDC_addrl:
	.res	1
_VDC_desth:
	.res	1
_VDC_destl:
	.res	1
_VDC_tmp1:
	.res	1
_VDC_tmp2:
	.res	1

; ------------------------------------------------------------------------------------------
VDC_Write:
; Function to do a VDC read and wait for ready status
; Input:	X = register number
; 			A = value to write
; ------------------------------------------------------------------------------------------

	stx VDC_ADDRESS_REGISTER            ; Store X in VDC address register
notyetreadywrite:						; Start of wait loop to wait for VDC status ready
	bit VDC_ADDRESS_REGISTER            ; Check status bit 7 of VDC address register
	bpl notyetreadywrite                ; Continue loop if status is not ready
	sta VDC_DATA_REGISTER               ; Store A to VDC data
	rts

; ------------------------------------------------------------------------------------------
_VDC_ScrollCopy_core:
; Function to copy memory from one to another position within VDC memory
; Input:	VDC_addrh = high byte of source address
;			VDC_addrl = low byte of source address
;			VDC_desth = high byte of destination address
;			VDC_destl = low byte of destination address
;			VDC_tmp1 = number of lines to copy
;			VDC_tmp2 = length per line to copy
; ------------------------------------------------------------------------------------------

loopscrollcpy:
	; Hi-byte of the destination address to register 18
	ldx #$12    						; Load $12 for register 18 (VDC RAM address high) in X	
	lda _VDC_desth      				; Load high byte of dest in A
	jsr VDC_Write						; Write VDC

	; Lo-byte of the destination address to register 19
	ldx #$13    						; Load $13 for register 19 (VDC RAM address high) in X	
	lda _VDC_destl       				; Load high byte of dest in A
	jsr VDC_Write						; Write VDC

	; Set the copy bit (bit 7) of register 24 (block copy mode)
	ldx #$18    						; Load $18 for register 24 (block copy mode) in X	
	lda #$80			        		; Load prepared value with bit 7 set in A
	jsr VDC_Write						; Write VDC

	; Hi-byte of the source address to block copy source register 32
	ldx #$20					    	; Load $20 for register 32 (block copy source) in X	
	lda _VDC_addrh			        	; Load high byte of source in A
	jsr VDC_Write						; Write VDC
	
	; Lo-byte of the source address to block copy source register 33
	ldx #$21					    	; Load $21 for register 33 (block copy source) in X	
	lda _VDC_addrl		        		; Load low byte of source in A
	jsr VDC_Write						; Write VDC

	; Number of bytes to copy
	ldx #$1E    						; Load $1E for register 30 (word count) in X
	lda _VDC_tmp2		        		; Set length
	jsr VDC_Write						; Write VDC

	; Increase destination address for one line
	clc									; Clear carry
	lda _VDC_destl		        		; Load low byte of destination
	adc #$50							; Add 80 charachters for next line
	sta _VDC_destl						; Store low byte of destination
	lda _VDC_desth						; Load high byte of destination
	adc #$00							; Add 0 to add carry if needed
	sta _VDC_desth						; Store high byte of destination

	; Increase source address for one line
	clc									; Clear carry
	lda _VDC_addrl		        		; Load low byte of destination
	adc #$50							; Add 80 charachters for next line
	sta _VDC_addrl						; Store low byte of destination
	lda _VDC_addrh						; Load high byte of destination
	adc #$00							; Add 0 to add carry if needed
	sta _VDC_addrh						; Store high byte of destination	

	; Decrease line counter and loop until last page
	dec _VDC_tmp1		        		; Decrease line counter
	lda _VDC_tmp1						; Load counter to A
	cmp #$ff							; Check if below zero
	beq scrollcpyend			        ; Go to end if counter is zero
	jmp loopscrollcpy					; Jump to start of copy loop if not zero
scrollcpyend:
    rts
