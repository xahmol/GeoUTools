		LoadW	r0, ClearScreen
		jsr	GraphicsString

		LoadW	r0, AppBanner
		jsr	GraphicsString

		LoadW	r0, AppText
		LoadW	r11, 00F6
		LoadB	r1H, 58
		jsr	PutString

		LoadW	r0, MainMenu
		jsr	DoMenu

		LoadW	r0, MainIcons
		jsr	DoIcons

DoGEOSAbout:
		jsr	GotoFirstMenu
		rts

DoFileQuit:
		jsr	GotoFirstMenu
		rts

DoIconOK:
		rts

DoIconCancel:
		rts

ClearScreen:
		.byte	MOVEPENTO
		.word	$0000
		.byte	$00
		.byte	NEWPATTERN
		.byte	$02
		.byte	RECTANGLETO
		.word	$027F
		.byte	$C7
		.byte	NULL

AppBanner:
		.byte	MOVEPENTO
		.word	$00F4
		.byte	$4F
		.byte	NEWPATTERN
		.byte	$00
		.byte	RECTANGLETO
		.word	$018A
		.byte	$77
		.byte	FRAME_RECTO
		.word	$00F4
		.byte	$4F
		.byte	NULL

AppText:
		.byte	BOLDON
		.byte	"GeoUMount"
		.byte	PLAINTEXT
		.byte	NULL

MainMenu:
		.byte	$00
		.byte	$0E
		.word	$0000
		.word	$003F
		.byte	02 | HORIZONTAL
		.word	MainMenuText0
		.byte	SUB_MENU
		.word	GEOSMenu
		.word	MainMenuText1
		.byte	SUB_MENU
		.word	FileMenu
GEOSMenu:
		.byte	$0F
		.byte	$1E
		.word	$0000
		.word	$0034
		.byte	01 | VERTICAL
		.word	MainMenuText2
		.byte	MENU_ACTION
		.word	DoGEOSAbout
FileMenu:
		.byte	$0F
		.byte	$1E
		.word	$0024
		.word	$004C
		.byte	01 | VERTICAL
		.word	MainMenuText3
		.byte	MENU_ACTION
		.word	DoFileQuit
MainMenuText0:
		.byte	"geos", $00
MainMenuText1:
		.byte	"file", $00
MainMenuText2:
		.byte	"about", $00
MainMenuText3:
		.byte	"quit", $00

MainIcons:
		.byte	$02
		.word	$0000
		.byte	$00
		.word	IconOK
		.byte	$49
		.byte	$A0
		.byte	$06
		.byte	$10
		.word	DoIconOK
		.word	IconCancel
		.byte	$49
		.byte	$B4
		.byte	$06
		.byte	$10
		.word	DoIconCancel

IconOK:
		.byte	$05, $FF, $82, $FE, $80, $04, $00, $82
		.byte	$03, $80, $04, $00, $B8, $03, $80, $00
		.byte	$F8, $C6, $00, $03, $80, $01, $8C, $CC
		.byte	$00, $03, $80, $01, $8C, $D8, $00, $03
		.byte	$80, $01, $8C, $F0, $00, $03, $80, $01
		.byte	$8C, $E0, $00, $03, $80, $01, $8C, $F0
		.byte	$00, $03, $80, $01, $8C, $D8, $00, $03
		.byte	$80, $01, $8C, $CC, $00, $03, $80, $00
		.byte	$F8, $C6, $00, $03, $80, $04, $00, $82
		.byte	$03, $80, $04, $00, $81, $03, $06, $FF
		.byte	$81, $7F, $05, $FF

IconCancel:
		.byte	$05, $FF, $82, $FE, $80, $04, $00, $82
		.byte	$03, $80, $04, $00, $B8, $03, $87, $C0
		.byte	$00, $00, $00, $E3, $8C, $60, $00, $00
		.byte	$00, $63, $8C, $07, $9F, $1E, $3C, $63
		.byte	$8C, $0C, $DD, $B3, $66, $63, $8C, $07
		.byte	$D9, $B0, $66, $63, $8C, $0C, $D9, $B0
		.byte	$7E, $63, $8C, $0C, $D9, $B0, $60, $63
		.byte	$8C, $6C, $D9, $B3, $66, $63, $87, $C7
		.byte	$D9, $9E, $3C, $63, $80, $04, $00, $82
		.byte	$03, $80, $04, $00, $81, $03, $06, $FF
		.byte	$81, $7F, $05, $FF





