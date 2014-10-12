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

