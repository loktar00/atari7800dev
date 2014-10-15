  set doublewide on
  displaymode 160A

  incgraphic gfx/tileset_level.png 160A
  incgraphic gfx/bombman_f_1.png

  characterset tileset_level

  dim frame = a
  dim tempplayerx = b
  dim tempplayery = c

  dim player1posX = d
  dim player1posY = e
  dim player1CharN = f
  dim player1CharS = g
  dim player1CharE = h
  dim player1CharW = I

  dim screendata = $2200 
  dim playerx=$22DC
  dim playery=$22e0

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
  playery[0]=16

  playerx[1]=136
  playery[1]=16

  playerx[2]=8
  playery[2]=144

  playerx[3]=136
  playery[3]=144

  clearscreen
  memcpy screendata defaultMap 220
  plotmap screendata 0 0 0 20 11

  savescreen

main
  restorescreen

  player1posX = (playerx[0]+8)/8
  player1posY = (playery[0]+4)/16

  rem *pokechar screendata player1posX player1posY 20 11 3

  tempplayery = player1posY + 1
  player1CharS = peekchar(screendata, player1posX, tempplayery, 20,11)

  tempplayery = player1posY - 1
  player1CharN = peekchar(screendata, player1posX, tempplayery, 20,11)

  tempplayerx = player1posX - 1
  player1CharW = peekchar(screendata, tempplayerx, player1posY, 20,11)

  tempplayerx = player1posX + 1
  player1CharE = peekchar(screendata, tempplayerx, player1posY, 20,11)

  if joy0down && playery[0] < 144 && player1CharS = 0 then playery[0]=playery[0]+1:goto donePlayerWalk
  if joy0up && playery[0] > 16  && player1CharN = 0 then playery[0]=playery[0]-1:goto donePlayerWalk

  if joy0left && playerx[0] > 8  && player1CharW = 0 then playerx[0]=playerx[0]-1:goto donePlayerWalk
  if joy0right && playerx[0] < 136  && player1CharE = 0 then playerx[0]=playerx[0]+1:goto donePlayerWalk


donePlayerWalk

 for z=0 to 3
   tempplayerx = playerx[z]
   tempplayery = playery[z]
   q=(z&3)+1
   plotsprite bombman_f_1 q tempplayerx tempplayery
 next

 drawscreen
 goto main

 alphachars ' abcdefghijklmnopqrstuvwxyz'

 alphadata defaultMap tileset_level
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

