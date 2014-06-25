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
