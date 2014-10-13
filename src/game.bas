
 rem ** 24 sprites display. this is about the limit on real hardware.

  set doublewide on

  dim frame=a
  dim tempplayerx=b
  dim tempplayery=c
  dim tempplayercolor=d
  dim tempplayerdx=e
  dim tempplayerdy=f

  dim playerx=$2200
  dim playery=$2220

  incgraphic gfx/tileset_level.png 160A 0 1 2 3
  incgraphic gfx/bombman_f_1.png

  characterset tileset_level

  BACKGRND=$D6
 
 rem tileset
  P0C1=$0A
  P0C2=$06
  P0C3=$00

 rem white
  P1C1=$5A
  P1C2=$2D
  P1C3=$03

 rem black
  P2C1=$04
  P2C2=$2D
  P2C3=$01

 rem blue
  P3C1=$86
  P3C2=$2D
  P3C3=$91

 rem red
  P4C1=$36
  P4C2=$2D
  P4C3=$31

  playerx[0]=8
  playery[0]=1

  playerx[1]=136
  playery[1]=1

  playerx[2]=8
  playery[2]=9

  playerx[3]=136
  playery[3]=9

  clearscreen
  plotmap screen1map 0 0 0 20 11
  drawscreen
  savescreen

main

 rem ** we split up the logic from the plot commands. this allows our logic
 rem ** to run during the visible screen. issuing a plot command forces 
 rem ** 7800basic to wait for the visible screen to end, to avoid glitches.
 restorescreen

 rem ** we could probably get a couple more sprites if we unrolled this loop
 rem ** and used a hardcoded palette value. but this is a bit more realistic.
 for z=0 to 3
   tempplayerx=playerx[z]
   tempplayery=playery[z]*16
   q=(z&3)+1
   plotsprite bombman_f_1 q tempplayerx tempplayery
 next
 drawscreen

 goto main

 alphachars ' abcdefghijklmnopqrstuvwxyz'

 alphadata screen1map tileset_level
 'cbcbcbcbcbcbcbcbcbcb'
 'c                  b'
 'c  bc  bc  bc  bc  b'
 'c                  b'
 'c  bc  bc  bc  bc  b'
 'c                  b'
 'c  bc  bc  bc  bc  b'
 'c                  b'
 'c  bc  bc  bc  bc  b'
 'c                  b'
 'cbcbcbcbcbcbcbcbcbcb'
end

