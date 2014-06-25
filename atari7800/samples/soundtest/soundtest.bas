
 displaymode 320A

 rem **uncomment the line below for single-channel TIA sound effects...
 rem set tiasfx mono

 rem **background color...
 BACKGRND=$0

 rem **set the height of characters and sprites...
 set zoneheight 8

 rem **import the characterset png...
 incgraphic atascii.png 320A 

 rem **set color of 320A text palette 0...
 P0C2=$0F

 rem **set the current character set...
 characterset atascii

 rem **set the letters represent each graphic character...
 alphachars ASCII

 dim sound2play=a
 dim sound2playbcd=b
 dim maxsounds=c
 maxsounds=13

main 
 clearscreen
 sound2playbcd=converttobcd(sound2play)
 plotchars 'sound to play:' 0 0 0
 plotvalue atascii 0 sound2playbcd 2 60 0
 plotmap sounddescriptions 0 72 0 16 1 0 sound2play 16

 if sfx1pointlo=0 && sfx1pointhi=0 then plotchars 'channel 0: off' 0 0 2 else plotchars 'channel 0: on' 0 0 2
 if sfx2pointlo=0 && sfx2pointhi=0 then plotchars 'channel 1: off' 0 80 2 else plotchars 'channel 1: on' 0 80 2

 plotchars 'move joystick to change sound' 0 0 20
 plotchars 'press fire to play' 0 0 21
 drawscreen

 if joy0right && sound2play<maxsounds then sound2play=sound2play+1:gosub debouncejoymove
 if joy0left && sound2play>0 then sound2play=sound2play-1:gosub debouncejoymove
 if !joy0fire then goto main

 rem ** the salvo shot provides an example of randomizing the sound pitch...
 if sound2play=0  then temp7=rand&7:playsfx sfx_salvolasershot temp7
 if sound2play=1  then playsfx sfx_spaceinvshoot
 if sound2play=2  then playsfx sfx_berzerkrobotdeath
 if sound2play=3  then playsfx sfx_echo1
 if sound2play=4  then playsfx sfx_echo2
 if sound2play=5  then playsfx sfx_jumpman
 if sound2play=6  then playsfx sfx_cavalry
 if sound2play=7  then playsfx sfx_alientrill1
 if sound2play=8  then playsfx sfx_alientrill2
 if sound2play=9  then playsfx sfx_pitfalljump
 if sound2play=10 then playsfx sfx_advpickup
 if sound2play=11 then playsfx sfx_advdrop
 if sound2play=12 then playsfx sfx_advbite
 if sound2play=13 then playsfx sfx_advdragonslain
 gosub debouncejoyfire
 goto main 

debouncejoyfire
 drawscreen
 if joy0fire then goto debouncejoyfire
 return

debouncejoymove
 drawscreen
 if joy0left || joy0right then goto debouncejoymove
 return

 data sfx_salvolasershot
  $10,$18,$01 ; version, priority, frames per chunk
  $04,$08,$08 ; first chunk of freq,channel,volume data 
  $05,$08,$07
  $04,$08,$07
  $05,$08,$07
  $06,$08,$07
  $07,$08,$06
  $06,$08,$06
  $07,$08,$06
  $08,$08,$06
  $09,$08,$06
  $08,$08,$06
  $09,$08,$06
  $0a,$08,$04
  $09,$08,$04
  $0a,$08,$04
  $0b,$08,$04
  $0a,$08,$04
  $0b,$08,$04
  $0c,$08,$04
  $0b,$08,$02
  $0c,$08,$02
  $0d,$08,$02
  $00,$00,$00
  $00,$00,$00
end

 data sfx_spaceinvshoot
  $10,$10,$03 ; version, priority, frames per chunk
  $18,$08,$08 ; first chunk of freq,channel,volume data 
  $19,$08,$05
  $19,$08,$05
  $19,$08,$05
  $19,$08,$05
  $1C,$08,$02
  $1C,$08,$02
  $1C,$08,$02
  $1C,$08,$02
  $1C,$08,$02
  $1E,$08,$01
  $1E,$08,$01
  $1E,$08,$01
  $1E,$08,$01
  $1E,$08,$01
  $00,$00,$00
end

 data sfx_berzerkrobotdeath
  $10,$10,$00 ; version, priority, frames per chunk
  $00,$08,$0F ; first chunk of freq,channel,volume data 
  $01,$08,$0E
  $02,$08,$0D
  $03,$08,$0C
  $04,$08,$0B
  $05,$08,$0A
  $06,$08,$09
  $07,$08,$08
  $08,$08,$07
  $09,$08,$06
  $0a,$08,$05
  $0b,$08,$04
  $0c,$08,$03
  $0d,$08,$02
  $0e,$08,$01
  $00,$00,$00
end

 data sfx_echo1
  $10,$08,$08 ; version, priority, frames per chunk
  $18,$06,$0a ; first chunk of freq,channel,volume data 
  $08,$06,$0a
  $01,$00,$00 
  $18,$06,$05
  $08,$06,$05
  $01,$00,$00 
  $18,$06,$02
  $08,$06,$02
  $00,$00,$00 
end

  data sfx_echo2
  $10,$05,$04 ; version, priority, frames per chunk
  $1F,$04,$0A ; first chunk of freq,channel,volume data 
  $01,$00,$00
  $1F,$04,$05
  $01,$00,$00
  $1F,$04,$02
  $00,$00,$00 
end
 
  data sfx_jumpman
  $10,$05,$04 ; version, priority, frames per chunk
  $1E,$04,$08 ; first chunk of freq,channel,volume data 
  $1B,$04,$08
  $18,$04,$08
  $11,$04,$08
  $16,$04,$08
  $00,$00,$00 
end

  data sfx_cavalry
  $10,$07,$05 ; version, priority, frames per chunk
  $1D,$04,$08 ; first chunk of freq,channel,volume data 
  $1A,$04,$08
  $17,$04,$08
  $13,$04,$08
  $17,$04,$08
  $13,$04,$08
  $13,$04,$08
  $00,$00,$00 
end

  data sfx_alientrill1
  $10,$05,$01 ; version, priority, frames per chunk
  $1B,$04,$08 ; first chunk of freq,channel,volume data 
  $1E,$04,$08
  $1B,$04,$08
  $1E,$04,$08
  $18,$04,$08
  $00,$00,$00 
end

  data sfx_alientrill2
  $10,$05,$01 ; version, priority, frames per chunk
  $18,$04,$08 ; first chunk of freq,channel,volume data 
  $1E,$04,$08
  $18,$04,$08
  $1E,$04,$08
  $14,$04,$08
  $00,$00,$00 
end
 
 data sfx_pitfalljump
  $10,$05,$03 ; version, priority, frames per chunk
  $06,$01,$04 ; first chunk of freq,channel,volume data 
  $04,$01,$04 ; first chunk of freq,channel,volume data 
  $03,$01,$04 ; first chunk of freq,channel,volume data 
  $02,$01,$04 ; first chunk of freq,channel,volume data 
  $04,$01,$04 ; first chunk of freq,channel,volume data 
  $00,$00,$00
end

 data sfx_advpickup
  $10,$04,$02 ; version, priority, frames per chunk
  $03,$06,$08 ; first chunk of freq,channel,volume data 
  $02,$06,$08 
  $01,$06,$08 
  $00,$06,$08 
  $00,$00,$00 
end

 data sfx_advdrop
  $10,$04,$02 ; version, priority, frames per chunk
  $00,$06,$08 ; first chunk of freq,channel,volume data 
  $01,$06,$08 
  $02,$06,$08 
  $03,$06,$08 
  $00,$00,$00 
end

 data sfx_advbite
  $10,$10,$02 ; version, priority, frames per chunk
  $1F,$03,$0F ; first chunk of freq,channel,volume data 
  $1F,$08,$0E 
  $1F,$03,$0D 
  $1F,$08,$0C 
  $1F,$03,$0B 
  $1F,$08,$0A 
  $1F,$03,$09 
  $1F,$08,$08 
  $1F,$03,$07 
  $1F,$08,$06 
  $1F,$03,$05 
  $1F,$08,$04 
  $1F,$03,$03 
  $1F,$08,$02 
  $1F,$03,$01 
  $00,$00,$00 
end

 data sfx_advdragonslain
  $10,$10,$02 ; version, priority, frames per chunk
  $10,$04,$0F ; first chunk of freq,channel,volume data 
  $11,$04,$0E 
  $12,$04,$0D 
  $13,$04,$0C 
  $14,$04,$0B 
  $15,$04,$0A 
  $16,$04,$09 
  $17,$04,$08 
  $18,$04,$07 
  $19,$04,$06 
  $1A,$04,$05 
  $1B,$04,$04 
  $1C,$04,$03 
  $1D,$04,$02 
  $1E,$04,$01 
  $00,$00,$00 
end

 alphadata sounddescriptions atascii
 'salvo laser shot'
 'spcinvader shot '
 'berzerk r.death '
 'echo 1          '
 'echo 2          '
 'jumpman         '
 'cavalry         '
 'alien trill 1   '
 'alien trill 2   '
 'pitfall jump    '
 'adventure pickup'
 'adventure drop  '
 'adv dragon bite '
 'adv dragon slain'
end
