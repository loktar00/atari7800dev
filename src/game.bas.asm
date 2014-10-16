; MACRO.H
; Version 1.05, 13/NOVEMBER/2003

VERSION_MACRO         = 105

;
; THIS FILE IS EXPLICITLY SUPPORTED AS A DASM-PREFERRED COMPANION FILE
; PLEASE DO *NOT* REDISTRIBUTE MODIFIED VERSIONS OF THIS FILE!
;
; This file defines DASM macros useful for development for the Atari 2600.
; It is distributed as a companion machine-specific support package
; for the DASM compiler. Updates to this file, DASM, and associated tools are
; available at at http://www.atari2600.org/dasm
;
; Many thanks to the people who have contributed.  If you take issue with the
; contents, or would like to add something, please write to me
; (atari2600@taswegian.com) with your contribution.
;
; Latest Revisions...
;
; 1.05  14/NOV/2003      - Added VERSION_MACRO equate (which will reflect 100x version #)
;                          This will allow conditional code to verify MACRO.H being
;                          used for code assembly.
; 1.04  13/NOV/2003     - SET_POINTER macro added (16-bit address load)
;
; 1.03  23/JUN/2003     - CLEAN_START macro added - clears TIA, RAM, registers
;
; 1.02  14/JUN/2003     - VERTICAL_SYNC macro added
;                         (standardised macro for vertical synch code)
; 1.01  22/MAR/2003     - SLEEP macro added. 
;                       - NO_ILLEGAL_OPCODES switch implemented
; 1.0	22/MAR/2003		Initial release

; Note: These macros use illegal opcodes.  To disable illegal opcode usage, 
;   define the symbol NO_ILLEGAL_OPCODES (-DNO_ILLEGAL_OPCODES=1 on command-line).
;   If you do not allow illegal opcode usage, you must include this file 
;   *after* including VCS.H (as the non-illegal opcodes access hardware
;   registers and require them to be defined first).

; Available macros...
;   SLEEP n             - sleep for n cycles
;   VERTICAL_SYNC       - correct 3 scanline vertical synch code
;   CLEAN_START         - set machine to known state on startup
;   SET_POINTER         - load a 16-bit absolute to a 16-bit variable

;-------------------------------------------------------------------------------
; SLEEP duration
; Original author: Thomas Jentzsch
; Inserts code which takes the specified number of cycles to execute.  This is
; useful for code where precise timing is required.
; ILLEGAL-OPCODE VERSION DOES NOT AFFECT FLAGS OR REGISTERS.
; LEGAL OPCODE VERSION MAY AFFECT FLAGS
; Uses illegal opcode (DASM 2.20.01 onwards).

            MAC SLEEP            ;usage: SLEEP n (n>1)
.CYCLES     SET {1}

                IF .CYCLES < 2
                    ECHO "MACRO ERROR: 'SLEEP': Duration must be > 1"
                    ERR
                ENDIF

                IF .CYCLES & 1
                    IFNCONST NO_ILLEGAL_OPCODES
                        nop $80
                    ELSE
                        bit VSYNC
                    ENDIF
.CYCLES             SET .CYCLES - 3
                ENDIF
            
                REPEAT .CYCLES / 2
                    nop
                REPEND
            ENDM

;-------------------------------------------------------
; SET_POINTER
; Original author: Manuel Rotschkar
;
; Sets a 2 byte RAM pointer to an absolute address.
;
; Usage: SET_POINTER pointer, address
; Example: SET_POINTER SpritePTR, SpriteData
;
; Note: Alters the accumulator, NZ flags
; IN 1: 2 byte RAM location reserved for pointer
; IN 2: absolute address

            MAC SET_POINTER
.POINTER    SET {1}
.ADDRESS    SET {2}

                LDA #<.ADDRESS  ; Get Lowbyte of Address
                STA .POINTER    ; Store in pointer
                LDA #>.ADDRESS  ; Get Hibyte of Address
                STA .POINTER+1  ; Store in pointer+1

            ENDM

; EOF
 processor 6502
 ;include "macro.h"
 include "7800basic.h"
 include "7800basic_variable_redefs.h"

 ;start address of cart...
 ifconst ROM48k
   ORG $4000
 else
   ifconst bankswitchmode
     ORG  $8000
     RORG $8000
   else
     ORG $8000
   endif
 endif

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
 

 ifnconst bankswitchmode
   if ( * < $f000 )
     ORG $F000
   endif
 else
     ifconst ROM128K
       if ( * < $f000 )
         ORG $27000
         RORG $F000
       endif
     endif
    ifconst ROM256K
       if ( * < $f000 )
         ORG $47000
         RORG $F000
       endif
     endif
    ifconst ROM512K
       if ( * < $f000 )
         ORG $87000
         RORG $F000
       endif
     endif
 endif
     ;standard routines needed for pretty much all games

     ;NMI and IRQ handlers
NMI
     ;VISIBLEOVER is 1 while the screen is drawn, and 0 right after the visible screen is done.
     pha
     lda visibleover
     eor #1
     sta visibleover
     pla
     rti

IRQ
     RTI

clearscreen
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 8
             ldx #$17
         endif
         if ZONEHEIGHT = 16
             ldx #$0B
         endif
     else
         ldx #$0B
     endif
     lda #0
clearscreenloop
     sta dlend,x
     dex
     bpl clearscreenloop
     lda #0
     sta valbufend ; clear the bcd value buffer
     sta valbufendsave 
     rts

restorescreen
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 8
             ldx #$17
         endif
         if ZONEHEIGHT = 16
             ldx #$0B
         endif
     else
         ldx #$0B
     endif
     lda #0
restorescreenloop
     lda dlendsave,x
     sta dlend,x
     dex
     bpl restorescreenloop
     lda valbufendsave
     sta valbufend
     rts

savescreen
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 8
             ldx #$17
         endif
         if ZONEHEIGHT = 16
             ldx #$0B
         endif
     else
         ldx #$0B
     endif
savescreenloop
     lda dlend,x
     sta dlendsave,x
     dex
     bpl savescreenloop
     lda valbufend
     sta valbufendsave
     rts

drawscreen
     jsr servicesfxchannels 
drawscreenwait
  ifconst debugcolor
     lda #$00
     sta BACKGRND
  endif
     lda visibleover
     bne drawscreenwait ; make sure the visible screen isn't being drawn


     ;add DL end entry on each DL
     ldx #$0C
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 8
             ldx #$17
         endif
         if ZONEHEIGHT = 16
             ldx #$0B
         endif
     else
         ldx #$0B
     endif

dlendloop
     lda DLPOINTL,x
     sta dlpnt
     lda DLPOINTH,x
     sta dlpnt+1
     ldy dlend,x
     lda #$00
     iny
     sta (dlpnt),y
     dex
     bpl dlendloop

; Make sure the visible screen has *started* before we exit.  That way we can rely on drawscreen
; delaying a full frame, but still allowing time for basic calculations.
visiblescreenstarted
  ifconst debugcolor
     lda #$41
     sta BACKGRND
  endif
visiblescreenstartedwait
     lda visibleover
     beq visiblescreenstartedwait


     ;we poll the joystick fire buttons and throw them in shadow registers now
     lda #0
     sta sINPT1
     sta sINPT3
     lda INPT4
     bmi .skipp0firecheck
     ;one button joystick is down
     lda #$f0
     sta sINPT1
     lda joybuttonmode
     and #%00000100
     beq .skipp0firecheck
     lda joybuttonmode
     ora #%00000100
     sta joybuttonmode
     sta SWCHB
.skipp0firecheck
     lda INPT5
     bmi .skipp1firecheck
     ;one button joystick is down
     lda #$f0
     sta sINPT3
     lda joybuttonmode
     and #%00010000
     beq .skipp1firecheck
     lda joybuttonmode
     ora #%00010000
     sta joybuttonmode
     sta SWCHB
.skipp1firecheck
     lda INPT1
     ora sINPT1
     sta sINPT1

     lda INPT3
     ora sINPT3
     sta sINPT3


 ifnconst pauseroutineoff
  ; check to see if pause was pressed and released
pauseroutine
     lda #8
     bit SWCHB
     beq pausepressed
        ;pause isn't pressed
	lda #0
        sta pausebuttonflag ; clear pause hold state in case its set

	;check if we're in an already paused state
        lda pausestate
	beq leavepauseroutine 	; nope, leave

        cmp #1			; last frame was the start of pausing
        beq enterpausestate2 	; move from state 1 to 2

        cmp #2
        beq carryonpausing

        ;pausestate must be >2, which means we're ending an unpause 
        lda #0
        sta pausebuttonflag        
	sta pausestate 
        jmp leavepauseroutine

pausepressed
        ;pause is pressed
        lda pausebuttonflag
	cmp #$ff
        beq carryonpausing

        ;its a new press, increment the state
	inc pausestate

	;silence volume at the start and end of pausing
	lda #0	
	sta AUDV0
	sta AUDV1

 ifconst pokeysupport
	ldy #7
pausesilencepokeyaudioloop
	sta (pokeybase),y
	dey
	bpl pausesilencepokeyaudioloop
 endif

	lda #$ff
	sta pausebuttonflag
	jmp carryonpausing

enterpausestate2
	lda #2
	sta pausestate
	bne carryonpausing
leavepauseroutine
     rts

carryonpausing
     ifconst .pause
	jsr .pause
     endif
     jmp drawscreenwait ; going to drawscreenwait skips servicing the sfx routine, so our sfx pause too

 else
     rts
 endif


     ; Pointers to the DL storage

     ifconst ZONEHEIGHT 
         if ZONEHEIGHT = 8

DLPOINTH
   .byte  $18,$18,$19,$19,$19,$19,$1a,$1a,$1a,$1b,$1b,$1b, $1c,$1c,$1c,$1d,$1d,$1d,$1e,$1e,$1e,$1e,$1f,$1f
DLPOINTL
   .byte  $64,$b4,$04,$54,$a4,$f4,$44,$94,$e4,$34,$84,$d4, $24,$74,$c4,$14,$64,$b4,$04,$54,$a4,$f4,$44,$94
 ; last byte at 1fe3

         endif

         if ZONEHEIGHT = 16

DLPOINTH
   .byte  $18,$18,$19,$19,$19,$1a, $1a,$1b,$1b,$1c,$1c,$1c
DLPOINTL
   .byte  $40,$ae,$1c,$8a,$f8,$66, $d4,$42,$b0,$1e,$8c,$fa
 ; last byte at 1d67

         endif

     else

DLPOINTH
   .byte  $18,$18,$19,$19,$19,$1a, $1a,$1b,$1b,$1c,$1c,$1c
DLPOINTL
   .byte  $40,$ae,$1c,$8a,$f8,$66, $d4,$42,$b0,$1e,$8c,$fa
 ; last byte at 1d67

     endif

servicesfxchannels
 lda sfx1pointlo
 ora sfx1pointhi
 beq skipservicesfx1 ; (sfx1pointlo) isn't pointing at a sound

 lda sfx1tick
 beq servicesfx1_cont1
 dec sfx1tick        ; frame countdown is non-zero. subtract one and
 jmp skipservicesfx1 ; skip playing the sound

servicesfx1_cont1

 lda sfx1frames      ; set the frame countdown for this sound chunk
 sta sfx1tick

 lda sfx1priority    ; decrease the sound's priority if its non-zero
 beq servicesfx1_cont2
 dec sfx1priority
servicesfx1_cont2

 ldy #0              ; play the sound
 lda (sfx1pointlo),y
 sta temp1
 clc
 adc sfx1poffset     ; take into account any pitch modification
 sta AUDF0
 iny
 lda (sfx1pointlo),y
 sta AUDC0
 sta temp2
 iny
 lda (sfx1pointlo),y
 sta AUDV0

 ora temp2
 ora temp1    ; check if F|C|V=0
 beq zerosfx1 ; if so, we're at the end of the sound.

 ; advance the pointer to the next sound chunk
 clc
 lda sfx1pointlo
 adc #3
 sta sfx1pointlo
 lda sfx1pointhi
 adc #0
 sta sfx1pointhi
 jmp skipservicesfx1
 
zerosfx1
 sta sfx1pointlo
 sta sfx1pointhi
 sta sfx1priority
skipservicesfx1
 
 ifnconst TIASFXMONO
 lda sfx2pointlo
 ora sfx2pointhi
 beq skipservicesfx2 ; (sfx2pointlo) isn't pointing at a sound

 lda sfx2tick
 beq servicesfx2_cont1
 dec sfx2tick        ; frame countdown is non-zero. subtract one and
 jmp skipservicesfx2 ; skip playing the sound

servicesfx2_cont1

 lda sfx2frames      ; set the frame countdown for this sound chunk
 sta sfx2tick

 lda sfx2priority    ; decrease the sound's priority if its non-zero
 beq servicesfx2_cont2
 dec sfx2priority
servicesfx2_cont2

 ldy #0              ; play the sound
 lda (sfx2pointlo),y
 sta temp1
 clc
 adc sfx2poffset     ; take into account any pitch modification
 sta AUDF1
 iny
 lda (sfx2pointlo),y
 sta AUDC1
 sta temp2
 iny
 lda (sfx2pointlo),y
 sta AUDV1

 ora temp2
 ora temp1    ; check if F|C|V=0
 beq zerosfx2 ; if so, we're at the end of the sound.

 ; advance the pointer to the next sound chunk
 clc
 lda sfx2pointlo
 adc #3
 sta sfx2pointlo
 lda sfx2pointhi
 adc #0
 sta sfx2pointhi
 jmp skipservicesfx2
 
zerosfx2
 sta sfx2pointlo
 sta sfx2pointhi
 sta sfx2priority
skipservicesfx2

 endif ; TIASFXMONO

 rts  

schedulesfx
 ifnconst TIASFXMONO
 ; called with temp1=<data temp2=>data temp3=pitch offset
 lda sfx1pointlo
 ora sfx1pointhi
 beq schedulesfx1 ;if channel 1 is idle, use it
 lda sfx2pointlo
 ora sfx2pointhi
 beq schedulesfx2 ;if channel 2 is idle, use it
 ;otherwise, compare which active sound has a lower priority...
 lda sfx1priority
 cmp sfx2priority
 bcs schedulesfx2
schedulesfx1
 endif ; TIASFXMONO

 ldy #1 ;for now we just skip the VERSION byte...
 lda (temp1),y
 sta sfx1priority
 iny
 lda (temp1),y
 sta sfx1frames
 lda temp1
 clc
 adc #3
 sta sfx1pointlo
 lda temp2
 adc #0
 sta sfx1pointhi
 lda temp3
 sta sfx1poffset
 lda #0
 sta sfx1tick
 rts
 ifnconst TIASFXMONO
schedulesfx2
 ldy #1 ;for now we just skip the VERSION byte...
 lda (temp1),y
 sta sfx2priority
 iny
 lda (temp1),y
 sta sfx2frames
 lda temp1
 clc
 adc #3
 sta sfx2pointlo
 lda temp2
 adc #0
 sta sfx2pointhi
 lda temp3
 sta sfx2poffset
 lda #0
 sta sfx2tick
 rts
 endif ; TIASFXMONO

plotsprite
  ifconst debugcolor
     lda #$00
     sta BACKGRND
  endif
plotspritewait
     lda visibleover
     bne plotspritewait

  ifconst debugcolor
     lda #$00
     sta BACKGRND
  endif

     ;arguments: 
     ; temp1=lo graphicdata 
     ; temp2=hi graphicdata 
     ; temp3=palette | width byte
     ; temp4=x
     ; temp5=y
     ; temp6=mode
     lda temp5 ;Y position
     lsr ;Divide by 8 or 16
     lsr
     lsr
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             lsr
         endif
     else
         lsr
     endif

     tax

     ; the next block allows for vertical masking, and ensures we don't overwrite non-DL memory
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             cmp #12
         else
             cmp #24
         endif
     else
         cmp #12
     endif
     bcc continueplotsprite1 ; the sprite is fully on-screen, so carry on...
     ; otherwise, check to see if the bottom half is in zone 0...
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             cmp #15
         else
             cmp #31
         endif
     else
         cmp #15
     endif
     bne exitplotsprite1
     ldx #0
     jmp continueplotsprite2
exitplotsprite1
     rts

continueplotsprite1

     lda DLPOINTL,x ;Get pointer to DL that this sprite starts in
     sta dlpnt
     lda DLPOINTH,x
     sta dlpnt+1

     ;Create DL entry for upper part of sprite

     ldy dlend,x ;Get the index to the end of this DL

     lda temp1 ; graphic data, lo byte
     sta (dlpnt),y ;Low byte of data address

     iny
     lda temp6
     sta (dlpnt),y

     iny
     lda temp5 ;Y position
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             and #$0F
         endif
         if ZONEHEIGHT = 8
             and #$7
         endif
     else
         and #$0F
     endif

     ora temp2 ; graphic data, hi byte
     sta (dlpnt),y

     iny
     lda temp3 ;palette|width
     sta (dlpnt),y

     iny
     lda temp4 ;Horizontal position
     sta (dlpnt),y

     iny
     sty dlend,x

     lda temp5
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             and #$0F ;See if sprite is entirely within this region
         endif
         if ZONEHEIGHT = 8 
             and #$07 ;See if sprite is entirely within this region
         endif
     else
         and #$0F ;default is zoneheight 16
     endif
     beq doneSPDL ;branch if it is

     ;Create DL entry for lower part of sprite

     inx ;Next region

     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             cpx #12
         else
             cpx #24
         endif
     else
         cpx #12
     endif
     bcc continueplotsprite2 ; the second half of the sprite is fully on-screen, so carry on...
     rts
continueplotsprite2

     lda DLPOINTL,x ;Get pointer to next DL
     sta dlpnt
     lda DLPOINTH,x
     sta dlpnt+1
     ldy dlend,x ;Get the index to the end of this DL

     lda temp1 ; graphic data, lo byte
     sta (dlpnt),y

     iny
     lda temp6
     sta (dlpnt),y

     iny
     lda temp5 ;Y position
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             and #$0F
             eor #$0F
         endif
         if ZONEHEIGHT = 8
             and #$07
             eor #$07
         endif
     else
         and #$0F
         eor #$0F
     endif

     sta temp6
     lda temp2 ; graphic data, hi byte
     clc
     sbc temp6
     sta (dlpnt),y

     iny
     lda temp3 ;palette|width
     sta (dlpnt),y

     iny
     lda temp4 ;Horizontal position
     sta (dlpnt),y

     iny
     sty dlend,x
doneSPDL
     rts

plotcharacters
  ifconst debugcolor
     lda #$00
     sta BACKGRND
  endif
plotcharacterswait
     lda visibleover
     bne plotcharacterswait

  ifconst debugcolor
     lda #$00
     sta BACKGRND
  endif

     ;arguments: 
     ; temp1=lo charactermap
     ; temp2=hi charactermap
     ; temp3=palette | width byte
     ; temp4=x
     ; temp5=y

     lda temp5 ;Y position

     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             and #$0F
         endif
         if ZONEHEIGHT = 8
             and #$1F
         endif
     else
         and #$0F
     endif

     tax
     lda DLPOINTL,x ;Get pointer to DL that the characters are in
     sta dlpnt
     lda DLPOINTH,x
     sta dlpnt+1

     ;Create DL entry for the characters

     ldy dlend,x ;Get the index to the end of this DL

     lda temp1 ; character map data, lo byte
     sta (dlpnt),y ;(1) store low address

     iny
     lda charactermode 
     sta (dlpnt),y ;(2) store mode

     iny
     lda temp2 ; character map, hi byte
     sta (dlpnt),y ;(3) store high address

     iny
     lda temp3 ;palette|width
     sta (dlpnt),y ;(4) store palette|width

     iny
     lda temp4 ;Horizontal position
     clc
     adc finescrollx
     sta (dlpnt),y ;(5) store horizontal position

     iny
     sty dlend,x ; save display list end byte
     rts

plotvalue
     ; calling 7800basic command:
     ; plotvalue digit_gfx palette variable/data number_of_digits screen_x screen_y
     ; ...displays the variable as BCD digits
     ;
     ; asm sub arguments: 
     ; temp1=lo charactermap
     ; temp2=hi charactermap
     ; temp3=palette | width byte
     ; temp4=x
     ; temp5=y
     ; temp6=number of digits
     ; temp7=lo variable
     ; temp8=hi variable

     lda #0
     tay
     ldx valbufend
     sta VALBUFFER,x

     lda temp6
     and #1
     bne pvnibble2char_skipnibble

pvnibble2char
     ; high nibble...
     lda (temp7),y
     and #$f0 
     lsr
     lsr
     lsr
     ifnconst DOUBLEWIDE ; multiply value by 2 for double-width
         lsr
     endif

     clc
     adc temp1 ; add the offset to character graphics to our value
     sta VALBUFFER,x
     inx
     dec temp6

pvnibble2char_skipnibble
     ; low nibble...
     lda (temp7),y
     and #$0f 
     ifconst DOUBLEWIDE ; multiply value by 2 for double-width
         asl
     endif
     clc
     adc temp1 ; add the offset to character graphics to our value
     sta VALBUFFER,x     
     inx
     iny

     dec temp6
     bne pvnibble2char

     ;point to the start of our valuebuffer
     clc
     lda #<VALBUFFER
     adc valbufend
     sta temp1
     lda #>VALBUFFER
     adc #0
     sta temp2

     ;advance valbufend to the end of our value buffer
     clc
     txa
     adc valbufend
     sta valbufend

     jmp plotcharacters


plotvalueextra
     ; calling 7800basic command:
     ; plotvalue digit_gfx palette variable/data number_of_digits screen_x screen_y
     ; ...displays the variable as BCD digits
     ;
     ; asm sub arguments: 
     ; temp1=lo charactermap
     ; temp2=hi charactermap
     ; temp3=palette | width byte
     ; temp4=x
     ; temp5=y
     ; temp6=number of digits
     ; temp7=lo variable
     ; temp8=hi variable

     lda #0
     tay
     ldx valbufend
     sta VALBUFFER,x

     lda temp6
     and #1
    
     bne pvnibble2char_skipnibbleextra

pvnibble2charextra
     ; high nibble...
     lda (temp7),y
     and #$f0 
     lsr
     lsr
     ifnconst DOUBLEWIDE ; multiply value by 2 for double-width
         lsr
     endif
     clc
     adc temp1 ; add the offset to character graphics to our value
     sta VALBUFFER,x
     inx

     ; second half of the digit
     clc
     adc #1
     sta VALBUFFER,x
     inx

pvnibble2char_skipnibbleextra
     ; low nibble...
     lda (temp7),y
     and #$0f 
     ifconst DOUBLEWIDE ; multiply value by 2 for double-width
         asl
     endif
     asl

     clc
     adc temp1 ; add the offset to character graphics to our value
     sta VALBUFFER,x     
     inx

     clc
     adc #1
     sta VALBUFFER,x
     inx
     iny

     dec temp6
     bne pvnibble2charextra

     ;point to the start of our valuebuffer
     clc
     lda #<VALBUFFER
     adc valbufend
     sta temp1
     lda #>VALBUFFER
     adc #0
     sta temp2

     ;advance valbufend to the end of our value buffer
     clc
     txa
     adc valbufend
     sta valbufend

     jmp plotcharacters


boxcollision

     ; prior to getting here...
     ; we have 4x lda #immed, 4x lda zp, 8x sta zp, and 1x jsr
     ; = 50 cycles to start. eep!

__boxx1 = temp1
__boxy1 = temp2
__boxw1 = temp3
__boxh1 = temp4

__boxx2 = temp5
__boxy2 = temp6
__boxw2 = temp7
__boxh2 = temp8

 ifconst collisionwrap
     ; if collision durring screen wrap is required, we add constants to
     ; coordinates, so we're not looking at negatives. (if reasonably
     ; sized sprites are used, anyway.)
     lda __boxx1            ;3 
     clc                    ;2
     adc   #48              ;2
     sta __boxx1            ;3

     lda __boxx2            ;3
     clc                    ;2
     adc   #48              ;2
     sta __boxx2            ;3

     lda __boxy1            ;3
     clc                    ;2
     adc   #32              ;2
     sta __boxy1            ;3

     lda __boxy2            ;3
     clc                    ;2
     adc   #32              ;2
     sta __boxy2            ;3
 ;=== +40 cycles for using collisionwrap
 endif

DoXCollisionCheck
     lda __boxx1            ;3
     cmp __boxx2            ;3
     bcs X1isbiggerthanX2   ;2/3
X2isbiggerthanX1
     adc __boxw1            ;3
     cmp __boxx2            ;3
     bcs DoYCollisionCheck  ;3/2
     rts                    ;6 - carry clear, no collision
X1isbiggerthanX2
     clc                    ;2
     sbc __boxw2            ;3
     cmp __boxx2            ;3
     bcs noboxcollision     ;3/2
DoYCollisionCheck
     lda __boxy1            ;3
     cmp __boxy2            ;3
     bcs Y1isbiggerthanY2   ;3/2
Y2isbiggerthanY1
     adc __boxh1            ;3
     cmp __boxy2            ;3
     rts                    ;6 
Y1isbiggerthanY2
     clc                    ;2
     sbc __boxh2            ;3
     cmp __boxy2            ;3
     bcs noboxcollision     ;3/2
yesboxcollision
     sec                    ;2
     rts                    ;6
noboxcollision
     clc                    ;2
     rts                    ;6

    ;==48 cycles worst case for checks+rts
    ;TOTAL =~48+50 for call+checks+rts
    ;TOTAL =~48+50+40 for call+wrap+checks+rts

randomize
     lda rand
     lsr
     rol rand16
     bcc noeor
     eor #$B4
noeor
     sta rand
     eor rand16
     rts

 ; bcd conversion routine courtesy Omegamatrix
 ; http://atariage.com/forums/blog/563/entry-10832-hex-to-bcd-conversion-0-99/
converttobcd
  ;value to convert is in the accumulator
  sta  temp1
  lsr
  adc  temp1
  ror
  lsr
  lsr
  adc  temp1
  ror
  adc  temp1
  ror
  lsr
  and  #$3C
  sta  temp2
  lsr
  adc  temp2
  adc  temp1 
  rts ; return the result in the accumulator

     ; Y and A contain multiplicands, result in A
mul8
     sty temp1
     sta temp2
     lda #0
reptmul8
     lsr temp2
     bcc skipmul8
     clc
     adc temp1
     ;bcs donemul8 might save cycles?
skipmul8
     ;beq donemul8 might save cycles?
     asl temp1
     bne reptmul8
donemul8
     rts

div8
     ; A=numerator Y=denominator, result in A
     cpy #2
     bcc div8end+1;div by 0 = bad, div by 1=no calc needed, so bail out
     sty temp1
     ldy #$ff
div8loop
     sbc temp1
     iny
     bcs div8loop
div8end
     tya
     ; result in A
     rts

     ; Y and A contain multiplicands, result in temp2,A=low, temp1=high
mul16
     sty temp1
     sta temp2

     lda #0
     ldx #8
     lsr temp1
mul16_1
     bcc mul16_2
     clc
     adc temp2
mul16_2
     ror
     ror temp1
     dex
     bne mul16_1
     sta temp2
     rts


     ; div int/int
     ; numerator in A, denom in temp1
     ; returns with quotient in A, remainder in temp1
div16
     sty temp1
     ldx #8
loopdiv
     cmp temp1
     bcc toosmalldiv
     sbc temp1 ; Note: Carry is, and will remain, set.
     rol temp2
     rol
     dex
     bne loopdiv
     beq donediv
toosmalldiv
     rol temp2
     rol
     dex
     bne loopdiv
donediv
     sta temp1
     lda temp2
     rts

 ifconst bankswitchmode
BS_jsr
     ifconst MCPDEVCART
         ora #$18
         sta $3000
     else
         sta $8000
     endif
     pla
     tax
     pla
     rts

BS_return
     pla ; bankswitch bank
     ifconst BANKRAM
        sta currentbank
        ora currentrambank
     endif
     ifconst MCPDEVCART
         ora #$18
         sta $3000
     else
         sta $8000
     endif
     pla ; bankswitch $0 flag
     rts 
 endif

 ifconst pokeysupport
  ; pokey detection routine. we check for pokey in the XBOARD/XM location, 
  ; and the standard $4000 location.
  ; if pokey the pokey is present, this routine will reset it.

detectpokeylocation
checklocation1
	;XBoard/XM...
	lda #<($450)
	sta pokeybaselo
	lda #>($450)
	sta pokeybasehi
	jsr checkforpokey
	lda pokeydetected
	bne checklocation2
	eor #$ff ; invert state for 7800basic if...then test
	sta pokeydetected
	rts

;checklocation2
;	;CPUWIZ board...
;	lda #<($400)
;	sta pokeybaselo
;	lda #>($400)
;	sta pokeybasehi
;	jsr checkforpokey
;	lda pokeydetected
;	bne checklocation3
;	eor #$ff ; invert state for 7800basic if...then test
;	sta pokeydetected
;	rts

checklocation2
	;traditional...
	lda #<($4000)
	sta pokeybaselo
	lda #>($4000)
	sta pokeybasehi
	jsr checkforpokey
	eor #$ff ; invert state for 7800basic if...then test
	sta pokeydetected
	rts

checkforpokey
	ldy #$0f
	lda #$00
	sta pokeydetected
resetpokeyregistersloop
	sta (pokeybase),y
	dey 
	bpl resetpokeyregistersloop

	ldy #PAUDCTL
	sta (pokeybase),y
	ldy #PSKCTL
	sta (pokeybase),y

	; let the dust settle...
	nop
	nop
	nop

	; we're in reset, so the RANDOM register should read $ff...
	ldy #PRANDOM
	lda (pokeybase),y
	cmp #$ff
	bne nopokeydetected

	; take pokey out of reset...
	ldy #PSKCTL
	lda #3
	sta (pokeybase),y

	; let the dust settle again...
	nop
	nop
	nop

	; we're out of reset, so RANDOM should read non-$ff...
	ldy #PRANDOM
	lda (pokeybase),y
	cmp #$ff
	beq checkpokeyoncemore ;one $ff might be a fluke. try again...
	rts

checkpokeyoncemore
	lda (pokeybase),y
	cmp #$ff
	beq nopokeydetected
	rts

nopokeydetected
	dec pokeydetected
	rts
 endif
START
start

     ;******** more or less the Atari recommended startup procedure

     sei
     cld

     lda #$07
     sta INPTCTRL ;lock 7800 into 7800 mode
     lda #$7F
     sta CTRL ;disable DMA
     lda #$00
     sta OFFSET
     sta INPTCTRL
     ldx #$FF
     txs

     ;************** Clear Memory

     ldx #$40
     lda #$00
crloop1     
     sta $00,x ;Clear zero page
     sta $100,x ;Clear page 1
     inx
     bne crloop1


     ldy #$00 ;Clear Ram
     lda #$18 ;Start at $1800
     sta $81 
     lda #$00
     sta $80
crloop3
     lda #$00
     sta ($80),y ;Store data
     iny ;Next byte
     bne crloop3 ;Branch if not done page
     inc $81 ;Next page
     lda $81
     cmp #$20 ;End at $1FFF
     bne crloop3 ;Branch if not

     ldy #$00 ;Clear Ram
     lda #$22 ;Start at $2200
     sta $81 
     lda #$00
     sta $80
crloop4
     lda #$00
     sta ($80),y ;Store data
     iny ;Next byte
     bne crloop4 ;Branch if not done page
     inc $81 ;Next page
     lda $81
     cmp #$27 ;End at $27FF
     bne crloop4 ;Branch if not

     ldx #$00
     lda #$00
crloop5     ;Clear 2100-213F, 2000-203F
     sta $2000,x
     sta $2100,x
     inx
     cpx #$40
     bne crloop5

     sta $80
     sta $81
     sta $82
     sta $83

     ;seed random number with hopefully-random timer value
     lda #1
     ora INTIM
     sta rand


 ifconst pokeysupport
     ; if pokey support is on, so try to detect it...
     jsr detectpokeylocation
 endif
    
     ;Setup port B for two button reading, and turn on both joysticks...
     lda #$14
     sta CTLSWB
     lda #0
     sta SWCHB

     ;Setup port A to read mode
     lda #$00
     sta CTLSWA

     ;************** Setup DLL entries
     ; 25 blank lines, 2 parts (max 16 lines per zone)

     ldx #$00
     lda #%01001111 ;low nibble=16 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21 ; blank
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

     lda #%01001000 ;low nibble=9 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx


     ; 192 mode lines divided into 12 or 24 zones, depending on the zone height
     ldy #$00

     ; the first visible zone is setup outside of the loop, because the interrupt
     ; bit is set, to signal the start of the visible screen

     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             lda #%11001111 ;low nibble=16 lines, high nibble=Holey 16 DMA+interrupt
         endif
         if ZONEHEIGHT = 8
             lda #%10100111 ;low nibble=8 lines, high nibble=Holey 8 DMA+interrupt
         endif
     else
         lda #%11001111 ;low nibble=16 lines, high nibble=Holey 16 DMA+interrupt
     endif
     sta DLLMEM,x
     inx
     lda DLPOINTH,y
     sta DLLMEM,x
     inx
     lda DLPOINTL,y
     sta DLLMEM,x
     inx
     iny

DLLloop2
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
             lda #%01001111 ;low nibble=16 lines, high nibble=Holey 16 DMA
         endif
         if ZONEHEIGHT = 8
             lda #%00100111 ;low nibble=8 lines, high nibble=Holey 8 DMA
         endif
     else
         lda #%01001111 ;low nibble=16 lines, high nibble=Holey 16 DMA
     endif
     sta DLLMEM,x
     inx
     lda DLPOINTH,y
     sta DLLMEM,x
     inx
     lda DLPOINTL,y
     sta DLLMEM,x
     inx
     iny
     ifconst ZONEHEIGHT
         if ZONEHEIGHT = 16
              cpy #$0C ;12 DLL entries
         endif
         if ZONEHEIGHT = 8
              cpy #$18 ;24 DLL entries
         endif
     else
         cpy #$0C ;12 DLL entries
     endif
     bne DLLloop2

     ; 26 blank lines in following DLs
     ; the interrupt bit is set, to signal the end of the visible screen
     lda #%11001111 ;low nibble=16 lines, high nibble=Holey DMA+interrupt
     sta DLLMEM,x
     inx
     lda #$21 ;$2100 
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

  ifnconst PAL
     lda #$49 ;low nibble=9 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
  else
     lda #%01001111 ;low nibble=16 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

     lda #%01001111 ;low nibble=16 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

     lda #%01001111 ;low nibble=16 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

     lda #%01001111 ;low nibble=16 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x
     inx

     lda #%01001001 ;low nibble=10 lines, high nibble=Holey DMA
     sta DLLMEM,x
     inx
     lda #$21
     sta DLLMEM,x
     inx
     lda #$00
     sta DLLMEM,x

  endif

     lda #>DLLMEM
     sta DPPH
     lda #<DLLMEM
     sta DPPL

        ; CTRL 76543210
        ;    7    colorburst kill
        ;    6,5  dma ctrl 2=normal DMA, 3=no DMA
        ;    4    character width 1=2 byte chars, 0=1 byte chars
        ;    3    border control 0=background color border, 1=black border
        ;    2    kangaroo mode 0=transparancy, 1=kangaroo
        ;    1,0  read mode 0=160x2/160x4 1=N/A 2=320B/320D 3=320A/320C

     ifconst DOUBLEWIDE
        lda #%01010000          ;Enable DMA, mode=160x2/160x4, 2x character width
     else
        lda #%01000000          ;Enable DMA, mode=160x2/160x4
     endif
     sta CTRL

     ; Visibleover gets xor'ed with 1 at the beginning and end of each frame.
     ; Here we ensure that vblank has just started when we set visibleover=0,
     ; to be sure we get the on/off phase right.
startupvblankoverwait
     lda MSTAT
     and #$80
     bne startupvblankoverwait
startupvblankwait
     lda MSTAT
     and #$80
     beq startupvblankwait
     lda #0 
     sta visibleover

     ifconst bankswitchmode
          ; we need to switch to the first bank before we jump there!
          ifconst MCPDEVCART
              lda #$18 ; xxx11nnn - switch to bank 0
              sta $3000
          else
              lda #0
              sta $8000
          endif
     endif

     jmp game

  ifconst DEV
    ifnconst ZONEHEIGHT
      echo "* the 4k 7800basic area has",[($FFF8 - *)]d,"bytes free."
    else
      if ZONEHEIGHT =  8
        echo "* the 2k 7800basic area has",[($FFF8 - *)]d,"bytes free."
      else
        echo "* the 4k 7800basic area has",[($FFF8 - *)]d,"bytes free."
      endif
    endif
  endif

  ifnconst bankswitchmode 
    ORG $FFF8
  else
    ifconst ROM128K
      ORG $27FF8
      RORG $FFF8
    endif
    ifconst ROM256K
      ORG $47FF8
      RORG $FFF8
    endif
    ifconst ROM512K
      ORG $87FF8
      RORG $FFF8
    endif
  endif


  .byte   $FF	; region verification. $FF=all regions
  .byte   $F7	; high nibble:  encryption check from $N000 to $FF7F. we only hash the last 4k for faster boot.
		; low nibble :  N=7 atari rainbow start, N=3 no atari rainbow

  ;Vectors
  .word NMI
  .word START
  .word IRQ

