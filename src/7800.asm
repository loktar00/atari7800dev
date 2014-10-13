game
.
 ; 

.L00 ;  rem ** 24 sprites display. this is about the limit on real hardware.

.
 ; 

.L01 ;  set doublewide on

.
 ; 

.L02 ;  dim frame = a

.L03 ;  dim tempplayerx = b

.L04 ;  dim tempplayery = c

.L05 ;  dim tempplayercolor = d

.L06 ;  dim tempplayerdx = e

.L07 ;  dim tempplayerdy = f

.
 ; 

.L08 ;  dim playerx = $2200

.L09 ;  dim playery = $2220

.
 ; 

.L010 ;  incgraphic gfx/tileset_level.png 160A 0 1 2 3

.L011 ;  incgraphic gfx/bombman_f_1.png

.
 ; 

.L012 ;  characterset tileset_level

    lda #>tileset_level
    sta CHARBASE

    lda #(tileset_level_mode | %01100000)
    sta charactermode

.
 ; 

.L013 ;  BACKGRND = $D6

	LDA #$D6
	STA BACKGRND
.
 ; 

.L014 ;  rem tileset

.L015 ;  P0C1 = $0A

	LDA #$0A
	STA P0C1
.L016 ;  P0C2 = $06

	LDA #$06
	STA P0C2
.L017 ;  P0C3 = $00

	LDA #$00
	STA P0C3
.
 ; 

.L018 ;  rem white

.L019 ;  P1C1 = $5A

	LDA #$5A
	STA P1C1
.L020 ;  P1C2 = $2D

	LDA #$2D
	STA P1C2
.L021 ;  P1C3 = $03

	LDA #$03
	STA P1C3
.
 ; 

.L022 ;  rem black

.L023 ;  P2C1 = $04

	LDA #$04
	STA P2C1
.L024 ;  P2C2 = $2D

	LDA #$2D
	STA P2C2
.L025 ;  P2C3 = $01

	LDA #$01
	STA P2C3
.
 ; 

.L026 ;  rem blue

.L027 ;  P3C1 = $86

	LDA #$86
	STA P3C1
.L028 ;  P3C2 = $2D

	LDA #$2D
	STA P3C2
.L029 ;  P3C3 = $91

	LDA #$91
	STA P3C3
.
 ; 

.L030 ;  rem red

.L031 ;  P4C1 = $36

	LDA #$36
	STA P4C1
.L032 ;  P4C2 = $2D

	LDA #$2D
	STA P4C2
.L033 ;  P4C3 = $31

	LDA #$31
	STA P4C3
.
 ; 

.L034 ;  playerx[0] = 8

	LDA #8
	LDX #0
	STA playerx,x
.L035 ;  playery[0] = 1

	LDA #1
	LDX #0
	STA playery,x
.
 ; 

.L036 ;  playerx[1] = 136

	LDA #136
	LDX #1
	STA playerx,x
.L037 ;  playery[1] = 1

	LDA #1
	LDX #1
	STA playery,x
.
 ; 

.L038 ;  playerx[2] = 8

	LDA #8
	LDX #2
	STA playerx,x
.L039 ;  playery[2] = 9

	LDA #9
	LDX #2
	STA playery,x
.
 ; 

.L040 ;  playerx[3] = 136

	LDA #136
	LDX #3
	STA playerx,x
.L041 ;  playery[3] = 9

	LDA #9
	LDX #3
	STA playery,x
.
 ; 

.L042 ;  clearscreen

 jsr clearscreen
.L043 ;  plotmap screen1map 0 0 0 20 11

    lda #12 ; width in two's complement
    sta temp3
    ora #0 ; palette left shifted 5 bits
    sta temp3
    lda #0
    sta temp4

    lda #0
    sta temp5

    lda #11

    sta temp6
    lda #<screen1map
    sta temp1

    lda #>screen1map
    sta temp2

plotcharactersloop0
 jsr plotcharacters
    clc
    lda #20
    adc temp1
    sta temp1
    lda #0
    adc temp2
    sta temp2
    inc temp5
    dec temp6
    bne plotcharactersloop0
.L044 ;  drawscreen

 jsr drawscreen
.L045 ;  savescreen

 jsr savescreen
.
 ; 

.main
 ; main

.
 ; 

.L046 ;  rem ** we split up the logic from the plot commands. this allows our logic

.L047 ;  rem ** to run during the visible screen. issuing a plot command forces 

.L048 ;  rem ** 7800basic to wait for the visible screen to end, to avoid glitches.

.L049 ;  restorescreen

 jsr restorescreen
.
 ; 

.L050 ;  rem ** we could probably get a couple more sprites if we unrolled this loop

.L051 ;  rem ** and used a hardcoded palette value. but this is a bit more realistic.

.L052 ;  for z = 0 to 3

	LDA #0
	STA z
.L052forz
.L053 ;  tempplayerx = playerx[z]

	LDX z
	LDA playerx,x
	STA tempplayerx
.L054 ;  tempplayery = playery[z] * 16

	LDX z
	LDA playery,x
	asl
	asl
	asl
	asl
	STA tempplayery
.L055 ;  q =  ( z & 3 )  + 1

; complex statement detected
	LDA z
	AND #3
	CLC
	ADC #1
	STA q
.L056 ;  plotsprite bombman_f_1 q tempplayerx tempplayery

    lda #<bombman_f_1
    sta temp1

    lda #>bombman_f_1
    sta temp2

    lda q
    asl
    asl
    asl
    asl
    asl
    ora #bombman_f_1_width_twoscompliment
    sta temp3

    lda tempplayerx
    sta temp4

    lda tempplayery

    sta temp5

    lda #(bombman_f_1_mode|%01000000)
    sta temp6

 jsr plotsprite
.L057 ;  next

	LDA z
	CMP #3

	INC z
 if ((* - .L052forz) < 127) && ((* - .L052forz) > -128)
	bcc .L052forz
 else
	bcs .0skipL052forz
	jmp .L052forz
.0skipL052forz
 endif
.L058 ;  drawscreen

 jsr drawscreen
.
 ; 

.L059 ;  goto main

 jmp .main

.
 ; 

.L060 ;  alphachars ' abcdefghijklmnopqrstuvwxyz'

.
 ; 

.L061 ;  alphadata screen1map tileset_level

	JMP .skipL061
screen1map
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
 .byte (<tileset_level + $06)
 .byte (<tileset_level + $04)
.skipL061
.
 ; 

gameend
 echo " ",[($E000 - gameend)]d , "bytes of ROM space left in the main area."

 ORG $E000  ; *************

tileset_level
       HEX 00000000ffffffff
bombman_f_1
       HEX 03ffffc0

 ORG $E100  ; *************

;tileset_level
       HEX 000000007fffffff
;bombman_f_1
       HEX 03ffffc0

 ORG $E200  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 0357d5c0

 ORG $E300  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 035ff5c0

 ORG $E400  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 03fd7fc0

 ORG $E500  ; *************

;tileset_level
       HEX 000000005a9ffeaf
;bombman_f_1
       HEX ffd557ff

 ORG $E600  ; *************

;tileset_level
       HEX 000000005a9aaeaf
;bombman_f_1
       HEX ffd557ff

 ORG $E700  ; *************

;tileset_level
       HEX 000000005a9aaeaf
;bombman_f_1
       HEX f5ffff5f

 ORG $E800  ; *************

;tileset_level
       HEX 000000005a9aaeaf
;bombman_f_1
       HEX 3ff55ffc

 ORG $E900  ; *************

;tileset_level
       HEX 000000005a9aaeaf
;bombman_f_1
       HEX 035555c0

 ORG $EA00  ; *************

;tileset_level
       HEX 000000005a955eaf
;bombman_f_1
       HEX 0d6eb970

 ORG $EB00  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 0daeba70

 ORG $EC00  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 0d6aa970

 ORG $ED00  ; *************

;tileset_level
       HEX 000000005aaaaaaf
;bombman_f_1
       HEX 0d569570

 ORG $EE00  ; *************

;tileset_level
       HEX 000000005555555f
;bombman_f_1
       HEX 035555c0

 ORG $EF00  ; *************

;tileset_level
       HEX 0000000055555557
;bombman_f_1
       HEX 00ffff00
 

