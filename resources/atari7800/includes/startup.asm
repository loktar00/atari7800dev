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

