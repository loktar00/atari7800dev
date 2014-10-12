game
.
 ; 

.L00 ;  displaymode 320A

    lda #%01000011 ;Enable DMA, mode=160x2/160x4
    sta CTRL

.
 ; 

.L01 ;  rem **background color...

.L02 ;  BACKGRND = $0

	LDA #$0
	STA BACKGRND
.
 ; 

.L03 ;  rem **set the height of characters and sprites...

.L04 ;  set zoneheight 8

.
 ; 

.L05 ;  rem **import the characterset png...

.L06 ;  incgraphic atascii.png 320A 

.
 ; 

.L07 ;  rem **set color of 320A text palette 0...

.L08 ;  P0C2 = $0F

	LDA #$0F
	STA P0C2
.
 ; 

.L09 ;  rem **set the current character set...

.L010 ;  characterset atascii

    lda #>atascii
    sta CHARBASE

    lda #(atascii_mode | %01100000)
    sta charactermode

.
 ; 

.L011 ;  rem **set the letters represent each graphic character...

.L012 ;  alphachars ASCII

.
 ; 

.L013 ;  clearscreen

 jsr clearscreen
.L014 ;  plotchars 'Hello^World!' 0 0 0

	JMP skipalphadata0
alphadata0
 .byte (<atascii + $48)
 .byte (<atascii + $65)
 .byte (<atascii + $6c)
 .byte (<atascii + $6c)
 .byte (<atascii + $6f)
 .byte (<atascii + $20)
 .byte (<atascii + $57)
 .byte (<atascii + $6f)
 .byte (<atascii + $72)
 .byte (<atascii + $6c)
 .byte (<atascii + $64)
 .byte (<atascii + $21)
skipalphadata0
    lda #<alphadata0
    sta temp1

    lda #>alphadata0
    sta temp2

    lda #20 ; width in two's complement
    ora #0 ; palette left shifted 5 bits
    sta temp3
    lda #0
    sta temp4

    lda #0

    sta temp5

 jsr plotcharacters
.L015 ;  plotchars 'It`s^great^to^be^alive.' 0 0 1

	JMP skipalphadata1
alphadata1
 .byte (<atascii + $49)
 .byte (<atascii + $74)
 .byte (<atascii + $60)
 .byte (<atascii + $73)
 .byte (<atascii + $20)
 .byte (<atascii + $67)
 .byte (<atascii + $72)
 .byte (<atascii + $65)
 .byte (<atascii + $61)
 .byte (<atascii + $74)
 .byte (<atascii + $20)
 .byte (<atascii + $74)
 .byte (<atascii + $6f)
 .byte (<atascii + $20)
 .byte (<atascii + $62)
 .byte (<atascii + $65)
 .byte (<atascii + $20)
 .byte (<atascii + $61)
 .byte (<atascii + $6c)
 .byte (<atascii + $69)
 .byte (<atascii + $76)
 .byte (<atascii + $65)
 .byte (<atascii + $2e)
skipalphadata1
    lda #<alphadata1
    sta temp1

    lda #>alphadata1
    sta temp2

    lda #9 ; width in two's complement
    ora #0 ; palette left shifted 5 bits
    sta temp3
    lda #0
    sta temp4

    lda #1

    sta temp5

 jsr plotcharacters
.L016 ;  plotchars '12345678901234567890123456789012' 0 0 4

	JMP skipalphadata2
alphadata2
 .byte (<atascii + $31)
 .byte (<atascii + $32)
 .byte (<atascii + $33)
 .byte (<atascii + $34)
 .byte (<atascii + $35)
 .byte (<atascii + $36)
 .byte (<atascii + $37)
 .byte (<atascii + $38)
 .byte (<atascii + $39)
 .byte (<atascii + $30)
 .byte (<atascii + $31)
 .byte (<atascii + $32)
 .byte (<atascii + $33)
 .byte (<atascii + $34)
 .byte (<atascii + $35)
 .byte (<atascii + $36)
 .byte (<atascii + $37)
 .byte (<atascii + $38)
 .byte (<atascii + $39)
 .byte (<atascii + $30)
 .byte (<atascii + $31)
 .byte (<atascii + $32)
 .byte (<atascii + $33)
 .byte (<atascii + $34)
 .byte (<atascii + $35)
 .byte (<atascii + $36)
 .byte (<atascii + $37)
 .byte (<atascii + $38)
 .byte (<atascii + $39)
 .byte (<atascii + $30)
 .byte (<atascii + $31)
 .byte (<atascii + $32)
skipalphadata2
    lda #<alphadata2
    sta temp1

    lda #>alphadata2
    sta temp2

    lda #0 ; width in two's complement
    ora #0 ; palette left shifted 5 bits
    sta temp3
    lda #0
    sta temp4

    lda #4

    sta temp5

 jsr plotcharacters
.
 ; 

.main
 ; main

.L017 ;  drawscreen

 jsr drawscreen
.L018 ;  goto main

 jmp .main

.
 ; 

gameend
 echo " ",[($F000 - gameend)]d , "bytes of ROM space left in the main area."

 ORG $F000  ; *************

atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 0000000000000000000000003000000000000000000000000000003000000000
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 000000000000007c00003c00000000006006000000000000007800

 ORG $F100  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 0018006618463b000e700000180018403c7e7e3c0c3c3c303c38181806006018
       HEX 3e667c3c787e603e667e3c667e63663c6036663c187e186366187e1e067800ff
       HEX 003e7c3c3e3c1806663c06663c63663c6006607c0e3e1836660c7e

 ORG $F200  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 000000ff7c6666001c38661818001860661830667e666630660c18180c7e3000
       HEX 607e66666c6060666618666c60636e66606c6c0618663c77661860180c180000
       HEX 006666606660183e6618066c186b66667c3e600618663c3e3c3e30

 ORG $F300  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 0018006606306f0018183c18000000307618180c6c0666186606000018001818
       HEX 6e6666606660606e66180678606b7e667c667c061866667f3c18301818186300
       HEX 003e6660667e186666180678187f66666666603c1866667f186618

 ORG $F400  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 001866663c1838181818ff7e007e00186e180c183c7c7c0c3c3e181830000c0c
       HEX 6e667c60667c7c607e180678607f7e666666663c1866666b3c3c181830183600
       HEX 18067c603e663e667c38066c187f6666666666601866666b3c660c

 ORG $F500  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 001866ff606c1c181c383c180000000c6638660c1c60600666661818187e1866
       HEX 663c66666c6060606618066c60777666666666601866666366660c1860181c00
       HEX 183c603c063c183e6000006018667c3c7c3e7c3e7e66666366667e

 ORG $F600  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 001866663e6636180e706618000000063c183c7e0c7e3c7e3c3c00000c00303c
       HEX 3c187c3c787e7e3e667e06666063663c7c3c7c3c7e66666366667e1e40780800
       HEX 1800600006000e0060180660380000000000000018000000000000

 ORG $F700  ; *************

;atascii
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 0000000018001c00000000000000000000000000000000000000000006006000
       HEX 0000000000000000000000000000000000000000000000000000000000000000
       HEX 000000000000000000000000000000000000000000000000000000
 

