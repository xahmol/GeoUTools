; SpriteSet Data...
; 1 images, 64 bytes per image, total size is 64 ($40) bytes.

* = addr_spriteset_data
spriteset_data

sprite_image_0
.byte $C0,$3C,$00,$F0,$3C,$00,$F0,$3C,$00,$F0,$3C,$00,$00,$03,$E0,$F0
.byte $03,$C0,$00,$03,$00,$F0,$00,$00,$00,$03,$00,$F0,$03,$C0,$00,$03
.byte $E0,$F0,$3C,$00,$F0,$3C,$38,$F0,$3C,$44,$F8,$3C,$92,$FF,$FD,$11
.byte $7F,$FD,$1D,$3F,$FD,$01,$0F,$FC,$82,$00,$00,$44,$00,$00,$38,$00



; SpriteSet Attribute Data...
; 1 attributes, 1 per image, 8 bits each, total size is 1 ($1) bytes.
; nb. Upper nybbles = MYXV, lower nybbles = colour (0-15).

* = addr_spriteset_attrib_data
spriteset_attrib_data

.byte $00



