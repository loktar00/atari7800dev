game
.L00 ;  set doublewide on

.L01 ;  displaymode 160A

    lda #%01010000 ;Enable DMA, mode=160x2/160x4, 2x character width
    sta CTRL

.
 ; 

.L02 ;  incgraphic gfx/tileset_level.png 160A

.L03 ;  incgraphic gfx/bombman_f_1.png

.
 ; 

.L04 ;  characterset tileset_level

    lda #>tileset_level
    sta CHARBASE

    lda #(tileset_level_mode | %01100000)
    sta charactermode

.
 ; 

.L05 ;  dim frame  =  a

.L06 ;  dim tempplayerx  =  b

.L07 ;  dim tempplayery  =  c

.
 ; 

.L08 ;  dim player1posX  =  d

.L09 ;  dim player1posY  =  e

.L010 ;  dim player1CharN  =  f

.L011 ;  dim player1CharS  =  g

.L012 ;  dim player1CharE  =  h

.L013 ;  dim player1CharW  =  I

.
 ; 

.L014 ;  dim screendata  =  $2200

.L015 ;  dim playerx = $22DC

.L016 ;  dim playery = $22e0

.
 ; 

.L017 ;  BACKGRND = $D6

	LDA #$D6
	STA BACKGRND
.
 ; 

.L018 ;  rem tileset

.L019 ;  P0C1 = $0A

	LDA #$0A
	STA P0C1
.L020 ;  P0C2 = $06

	LDA #$06
	STA P0C2
.L021 ;  P0C3 = $00

	LDA #$00
	STA P0C3
.
 ; 

.L022 ;  rem white

.L023 ;  P1C1 = $5A

	LDA #$5A
	STA P1C1
.L024 ;  P1C2 = $2D

	LDA #$2D
	STA P1C2
.L025 ;  P1C3 = $03

	LDA #$03
	STA P1C3
.
 ; 

.L026 ;  rem black

.L027 ;  P2C1 = $04

	LDA #$04
	STA P2C1
.L028 ;  P2C2 = $2D

	LDA #$2D
	STA P2C2
.L029 ;  P2C3 = $01

	LDA #$01
	STA P2C3
.
 ; 

.L030 ;  rem blue

.L031 ;  P3C1 = $86

	LDA #$86
	STA P3C1
.L032 ;  P3C2 = $2D

	LDA #$2D
	STA P3C2
.L033 ;  P3C3 = $91

	LDA #$91
	STA P3C3
.
 ; 

.L034 ;  rem red

.L035 ;  P4C1 = $36

	LDA #$36
	STA P4C1
.L036 ;  P4C2 = $2D

	LDA #$2D
	STA P4C2
.L037 ;  P4C3 = $31

	LDA #$31
	STA P4C3
.
 ; 

.L038 ;  playerx[0] = 8

	LDA #8
	LDX #0
	STA playerx,x
.L039 ;  playery[0] = 16

	LDA #16
	LDX #0
	STA playery,x
.
 ; 

.L040 ;  playerx[1] = 136

	LDA #136
	LDX #1
	STA playerx,x
.L041 ;  playery[1] = 16

	LDA #16
	LDX #1
	STA playery,x
.
 ; 

.L042 ;  playerx[2] = 8

	LDA #8
	LDX #2
	STA playerx,x
.L043 ;  playery[2] = 144

	LDA #144
	LDX #2
	STA playery,x
.
 ; 

.L044 ;  playerx[3] = 136

	LDA #136
	LDX #3
	STA playerx,x
.L045 ;  playery[3] = 144

	LDA #144
	LDX #3
	STA playery,x
.
 ; 

.L046 ;  clearscreen

 jsr clearscreen
.L047 ;  memcpy screendata defaultMap 220

 ldy #220
memcpyloop0
 lda defaultMap-1,y
 sta screendata-1,y
 dey
 bne memcpyloop0
.L048 ;  plotmap screendata 0 0 0 20 11

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
    lda #<screendata
    sta temp1

    lda #>screendata
    sta temp2

plotcharactersloop1
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
    bne plotcharactersloop1
.
 ; 

.L049 ;  savescreen

 jsr savescreen
.
 ; 

.main
 ; main

.L050 ;  restorescreen

 jsr restorescreen
.
 ; 

.L051 ;  player1posX  =   ( playerx[0] - 4 )  / 8

; complex statement detected
	LDX #0
	LDA playerx,x
	SEC
	SBC #4
	lsr
	lsr
	lsr
	STA player1posX
.L052 ;  player1posY  =   ( playery[0] - 8 )  / 16

; complex statement detected
	LDX #0
	LDA playery,x
	SEC
	SBC #8
	lsr
	lsr
	lsr
	lsr
	STA player1posY
.L053 ;  tempplayerx  =   ( playerx[0] + 4 )  / 8

; complex statement detected
	LDX #0
	LDA playerx,x
	CLC
	ADC #4
	lsr
	lsr
	lsr
	STA tempplayerx
.L054 ;  tempplayery  =   ( playery[0] )  / 16

; complex statement detected
	LDX #0
	LDA playery,x
	lsr
	lsr
	lsr
	lsr
	STA tempplayery
.
 ; 

.L055 ;  pokechar screendata tempplayerx player1posY 20 11 3

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy tempplayerx
    lda #3
    sta (temp1),y
.
 ; 

.L056 ;  rem North Check

.L057 ;  player1CharS  =  peekchar ( screendata ,  player1posX ,  player1posY ,  20 , 11 ) 

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda (temp1),y
	STA player1CharS
.
 ; 

.L058 ;  rem South CHeck

.L059 ;  player1posY  =   ( playery[0] + 24 )  / 16

; complex statement detected
	LDX #0
	LDA playery,x
	CLC
	ADC #24
	lsr
	lsr
	lsr
	lsr
	STA player1posY
.L060 ;  player1CharN  =  peekchar ( screendata ,  player1posX ,  player1posY ,  20 , 11 ) 

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda (temp1),y
	STA player1CharN
.
 ; 

.L061 ;  pokechar screendata tempplayerx player1posY 20 11 3

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy tempplayerx
    lda #3
    sta (temp1),y
.
 ; 

.L062 ;  rem West Check

.L063 ;  player1CharW  =  peekchar ( screendata ,  player1posX ,  tempplayery ,  20 , 11 ) 

    ldy tempplayery
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda (temp1),y
	STA player1CharW
.
 ; 

.L064 ;  pokechar screendata player1posX player1posY 20 11 3

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda #3
    sta (temp1),y
.
 ; 

.L065 ;  rem East Check

.L066 ;  player1posX  =   ( playerx[0] + 16 )  / 8

; complex statement detected
	LDX #0
	LDA playerx,x
	CLC
	ADC #16
	lsr
	lsr
	lsr
	STA player1posX
.L067 ;  player1CharE  =  peekchar ( screendata ,  player1posX ,  player1posY ,  20 , 11 ) 

    ldy player1posY
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda (temp1),y
	STA player1CharE
.
 ; 

.L068 ;  pokechar screendata player1posX tempplayery 20 11 3

    ldy tempplayery
    lda screendata_mult_lo,y
    sta temp1
    lda screendata_mult_hi,y
    sta temp2
    ldy player1posX
    lda #3
    sta (temp1),y
.
 ; 

.L069 ;  if joy0down  &&  playery[0]  <  144 then playery[0] = playery[0] + 1 : goto donePlayerWalk

 lda #$20
 bit SWCHA
	BNE .skipL069
.condpart0
	LDX #0
	LDA playery,x
	CMP #144
     BCS .skip0then
.condpart1
	LDX #0
	INC playery,x
 jmp .donePlayerWalk

.skip0then
.skipL069
.L070 ;  if joy0up  &&  playery[0]  >  16 then playery[0] = playery[0] - 1 : goto donePlayerWalk

 lda #$10
 bit SWCHA
	BNE .skipL070
.condpart2
	LDA #16
	LDX #0
	CMP playery,x
     BCS .skip2then
.condpart3
	LDX #0
	DEC playery,x
 jmp .donePlayerWalk

.skip2then
.skipL070
.
 ; 

.L071 ;  if joy0left  &&  playerx[0]  >  8 then playerx[0] = playerx[0] - 1 : goto donePlayerWalk

 bit SWCHA
	BVS .skipL071
.condpart4
	LDA #8
	LDX #0
	CMP playerx,x
     BCS .skip4then
.condpart5
	LDX #0
	DEC playerx,x
 jmp .donePlayerWalk

.skip4then
.skipL071
.L072 ;  if joy0right  &&  playerx[0]  <  136 then playerx[0] = playerx[0] + 1 : goto donePlayerWalk

 bit SWCHA
	BMI .skipL072
.condpart6
	LDX #0
	LDA playerx,x
	CMP #136
     BCS .skip6then
.condpart7
	LDX #0
	INC playerx,x
 jmp .donePlayerWalk

.skip6then
.skipL072
.
 ; 

.donePlayerWalk
 ; donePlayerWalk

.
 ; 

.L073 ;  for z = 0 to 3

	LDA #0
	STA z
.L073forz
.L074 ;  tempplayerx  =  playerx[z]

	LDX z
	LDA playerx,x
	STA tempplayerx
.L075 ;  tempplayery  =  playery[z]

	LDX z
	LDA playery,x
	STA tempplayery
.L076 ;  q =  ( z & 3 )  + 1

; complex statement detected
	LDA z
	AND #3
	CLC
	ADC #1
	STA q
.L077 ;  plotsprite bombman_f_1 q tempplayerx tempplayery

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
.L078 ;  next

	LDA z
	CMP #3

	INC z
 if ((* - .L073forz) < 127) && ((* - .L073forz) > -128)
	bcc .L073forz
 else
	bcs .0skipL073forz
	jmp .L073forz
.0skipL073forz
 endif
.
 ; 

.L079 ;  drawscreen

 jsr drawscreen
.L080 ;  goto main

 jmp .main

.
 ; 

.L081 ;  alphachars ' abcdefghijklmnopqrstuvwxyz'

.
 ; 

.L082 ;  alphadata defaultMap tileset_level

	JMP .skipL082
defaultMap
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
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
 .byte (<tileset_level + $00)
.skipL082
.
 ; 

gameend
 echo " ",[($E000 - gameend)]d , "bytes of ROM space left in the main area."

 ORG $E000  ; *************

tileset_level
       HEX 00000000ffffffffff6abfaf
bombman_f_1
       HEX 03ffffc0

 ORG $E100  ; *************

;tileset_level
       HEX 000000007fffffffdaf76ebb
;bombman_f_1
       HEX 03ffffc0

 ORG $E200  ; *************

;tileset_level
       HEX 000000005aaaaaafdadbad7b
;bombman_f_1
       HEX 0357d5c0

 ORG $E300  ; *************

;tileset_level
       HEX 000000005aaaaaaffad9ebeb
;bombman_f_1
       HEX 035ff5c0

 ORG $E400  ; *************

;tileset_level
       HEX 000000005aaaaaafdaf5d76a
;bombman_f_1
       HEX 03fd7fc0

 ORG $E500  ; *************

;tileset_level
       HEX 000000005a9ffeafdabfff69
;bombman_f_1
       HEX ffd557ff

 ORG $E600  ; *************

;tileset_level
       HEX 000000005a9aaeafb57faf57
;bombman_f_1
       HEX ffd557ff

 ORG $E700  ; *************

;tileset_level
       HEX 000000005a9aaeaf9ffdabfe
;bombman_f_1
       HEX f5ffff5f

 ORG $E800  ; *************

;tileset_level
       HEX 000000005a9aaeaf5aadabdd
;bombman_f_1
       HEX 3ff55ffc

 ORG $E900  ; *************

;tileset_level
       HEX 000000005a9aaeafdaab6b6f
;bombman_f_1
       HEX 035555c0

 ORG $EA00  ; *************

;tileset_level
       HEX 000000005a955eafdaaba7ab
;bombman_f_1
       HEX 0d6eb970

 ORG $EB00  ; *************

;tileset_level
       HEX 000000005aaaaaafdaab9dab
;bombman_f_1
       HEX 0daeba70

 ORG $EC00  ; *************

;tileset_level
       HEX 000000005aaaaaafdaaa9dab
;bombman_f_1
       HEX 0d6aa970

 ORG $ED00  ; *************

;tileset_level
       HEX 000000005aaaaaafb5597daa
;bombman_f_1
       HEX 0d569570

 ORG $EE00  ; *************

;tileset_level
       HEX 000000005555555f9fffd75e
;bombman_f_1
       HEX 035555c0

 ORG $EF00  ; *************

;tileset_level
       HEX 00000000555555575f6bd7fd
;bombman_f_1
       HEX 00ffff00
screendata_mult_lo
  .byte <(screendata+0)
  .byte <(screendata+20)
  .byte <(screendata+40)
  .byte <(screendata+60)
  .byte <(screendata+80)
  .byte <(screendata+100)
  .byte <(screendata+120)
  .byte <(screendata+140)
  .byte <(screendata+160)
  .byte <(screendata+180)
  .byte <(screendata+200)
screendata_mult_hi
  .byte >(screendata+0)
  .byte >(screendata+20)
  .byte >(screendata+40)
  .byte >(screendata+60)
  .byte >(screendata+80)
  .byte >(screendata+100)
  .byte >(screendata+120)
  .byte >(screendata+140)
  .byte >(screendata+160)
  .byte >(screendata+180)
  .byte >(screendata+200)
 

