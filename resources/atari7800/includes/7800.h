; ******************** 7800 Hardware Adresses ***************************
;
;       MEMORY MAP USAGE OF THE 7800
;
;       00-1F           TIA REGISTERS
;       20-3F           MARIA REGISTERS
;       40-FF           ZERO PAGE RAM
;       100-13F         SHADOW OF TIA AND MARIA REGISTERS -- USED FOR
;                       PUSHING ACCUMULATOR ONTO REGISTERS
;       140-1FF         RAM (STACK)
;       200-27F         NOT USED
;       280-2FF         PIA PORTS AND TIMERS
;       300-17FF        NOT USED
;       1800-203F       RAM
;       2040-20FF       SHADOW OF ZERO PAGE RAM
;       2100-213F       RAM
;       2140-21FF       SHADOW OF STACK RAM
;       2200-27FF       RAM
;       2800-3FFF       DUPLICATION OF ADDRESS SPACE 2000-27FF
;       4000-FF7F       UNUSED ADDRESS SPACE
;       FF80-FFF9       RESERVED FOR ENCRYPTION
;       FFFA-FFFF       6502 VECTORS


;****** 00-1F ********* TIA REGISTERS ******************

INPTCTRL = $01     ;Input control

INPT0    = $08     ;Paddle Control Input 0
INPT1    = $09     ;Paddle Control Input 1
INPT2    = $0A     ;Paddle Control Input 2
INPT3    = $0B     ;Paddle Control Input 3

; ** some common alternate names for INPT0/1/2/3
INPT4B   = $08     ;Joystick 0 Fire 1
INPT4A   = $09     ;Joystick 0 Fire 1
INPT5B   = $0A     ;Joystick 1 Fire 0
INPT5A   = $0B     ;Joystick 1 Fire 1
INPT4R   = $08     ;Joystick 0 Fire 1
INPT4L   = $09     ;Joystick 0 Fire 1
INPT5R   = $0A     ;Joystick 1 Fire 0
INPT5L   = $0B     ;Joystick 1 Fire 1


INPT4    = $0C     ;Player 0 Fire Button Input
INPT5    = $0D     ;Player 1 Fire Button Input

AUDC0    = $15     ;Audio Control Channel 0
AUDC1    = $16     ;Audio Control Channel 1
AUDF0    = $17     ;Audio Frequency Channel 0
AUDF1    = $18     ;Audio Frequency Channel 1
AUDV0    = $19     ;Audio Volume Channel 0
AUDV1    = $1A     ;Audio Volume Channel 1

;****** 20-3F ********* MARIA REGISTERS ***************

BACKGRND = $20     ;Background Color
P0C1     = $21     ;Palette 0 - Color 1
P0C2     = $22     ;Palette 0 - Color 2
P0C3     = $23     ;Palette 0 - Color 3
WSYNC    = $24     ;Wait For Sync
P1C1     = $25     ;Palette 1 - Color 1
P1C2     = $26     ;Palette 1 - Color 2
P1C3     = $27     ;Palette 1 - Color 3
MSTAT    = $28     ;Maria Status
P2C1     = $29     ;Palette 2 - Color 1
P2C2     = $2A     ;Palette 2 - Color 2
P2C3     = $2B     ;Palette 2 - Color 3
DPPH     = $2C     ;Display List List Pointer High
P3C1     = $2D     ;Palette 3 - Color 1
P3C2     = $2E     ;Palette 3 - Color 2
P3C3     = $2F     ;Palette 3 - Color 3
DPPL     = $30     ;Display List List Pointer Low
P4C1     = $31     ;Palette 4 - Color 1
P4C2     = $32     ;Palette 4 - Color 2
P4C3     = $33     ;Palette 4 - Color 3
CHARBASE = $34     ;Character Base Address
P5C1     = $35     ;Palette 5 - Color 1
P5C2     = $36     ;Palette 5 - Color 2
P5C3     = $37     ;Palette 5 - Color 3
OFFSET   = $38     ;Unused - Store zero here
P6C1     = $39     ;Palette 6 - Color 1
P6C2     = $3A     ;Palette 6 - Color 2
P6C3     = $3B     ;Palette 6 - Color 3
CTRL     = $3C     ;Maria Control Register
P7C1     = $3D     ;Palette 7 - Color 1
P7C2     = $3E     ;Palette 7 - Color 2
P7C3     = $3F     ;Palette 7 - Color 3


;****** 280-2FF ******* PIA PORTS AND TIMERS ************

SWCHA    = $280    ;P0, P1 Joystick Directional Input
SWCHB    = $282    ;Console Switches
CTLSWA   = $281    ;I/O Control for SCHWA
CTLSWB   = $283    ;I/O Control for SCHWB

INTIM    = $284    ;Iterval Timer Read
TIM1T    = $294    ;Set 1    CLK Interval (838   nsec/interval)
TIM8T    = $295    ;Set 8    CLK Interval (6.7   usec/interval)
TIM64T   = $296    ;Set 64   CLK Interval (63.6  usec/interval)
T1024T   = $297    ;Set 1024 CLK Interval (858.2 usec/interval)
TIM64TI  = $29E    ;Interrupt timer 64T

; Pokey register relative locations, since its base may be different
; depending on the hardware.
PAUDF0   = $0    ; extra audio channels and frequencies
PAUDC0   = $1
PAUDF1   = $2
PAUDC1   = $3
PAUDF2   = $4
PAUDC2   = $5
PAUDF3   = $6
PAUDC3   = $7
PAUDCTL  = $8    ; Audio Control
PRANDOM  = $A    ; 17 bit polycounter pseudo random
PSKCTL   = $F    ; Serial Port control
