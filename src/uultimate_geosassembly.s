; /*****************************************************************
; Ultimate 64/II+ Command Library - GEOS assembly additions
; Added by Xander Mol, 2023
; 
; Based on ultimate_dos-1.2.docx and command interface.docx
; https://github.com/markusC64/1541ultimate2/tree/master/doc
; 
; Disclaimer:  Because of the nature of DOS commands, use this code
; soley at your own risk.
; 
; Patches and pull requests are welcome
; ******************************************************************/


; Enable and disanble IO under GEOS
; Code by: Cenbe / https://www.lyonlabs.org/commodore/onrequest/geos/geos-prog-tips.html

    .export _enableIO
    .export _restoreIO

; ------------------------------------------------------------------------------------------
_enableIO:
; Function to enable IO range under GEOS while stoppimh interrupts
; ------------------------------------------------------------------------------------------

    php
    sei
    lda $01
    sta ioSave  ;save memory configuration
    and #$f8
    ora #$05    ;bank in I/O
    sta $01
    plp
    rts

; ------------------------------------------------------------------------------------------
_restoreIO:
; Function to restore GEOS to original state after using IO
; ------------------------------------------------------------------------------------------
    php
    sei
    lda ioSave  ;restore previous configuration
    sta $01
    plp
    rts

ioSave:
    .res 1