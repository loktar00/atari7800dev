 processor 6502
 include "7800.h"
 ;include "macro.h"
 include "7800basic_variable_redefs.h"

 ;************ 7800 overall RAM map **************

 ;         40-FF         zero page RAM
 ;        140-1FF        RAM (stack)
 ;       1800-203F       RAM
 ;       2100-213F       RAM
 ;       2200-27FF       RAM

 ;************ 7800basic RAM usage map **************

 ;         40-FF         numerous defines, listed below
 ;        140-1FF        RAM (stack)

 ;   if zone height = 16...
 ;              1800-1833       DLL (2*3)+(12*3)+(5*3) = 57 or  = 0x39
 ;              1840-1D67       DLs (12*110bytes)
 ;              1D68-1FFF       Free

 ; 
 ;   if zone height = 8...
 ;              1800-185D       DLL (2*3)+(24*3)+(5*3) = 93 = 0x5D
 ;              1864-1FE3       DLs (24*80bytes)
 ;              1FE4-1FFF       Free

 ;       2000-203F       Reserved
 ;       2100-213F       Reserved
 ;       2200-27FF       Free

DLLMEM          = $1800
VALBUFFER       = $2000 ; to $202F 
score0          = $2034 ; $2035 $2036
score1          = $2037 ; $2038 $2039
pausebuttonflag = $203A
valbufend       = $203B
valbufendsave   = $203C
finescrollx     = $203D
finescrolly     = $203E
joybuttonmode   = $203F ; used to track any joysticks that were changed to one-button mode

pausestate     = $2100
dlzero         = $2101 ; force end of $2100 DL, which we use in vblank and overscan
sINPT1         = $2102 ; shadow register for joy button joy0
sINPT3         = $2103 ; shadow register for joy button joy1
currentbank    = $2104
currentrambank = $2105
charactermode  = $2106
pokeydetected  = $2107

rand       = $40
rand16     = $41
temp1      = $42
temp2      = $43
temp3      = $44
temp4      = $45
temp5      = $46
temp6      = $47
temp7      = $48
temp8      = $49
temp9      = $4a

dlpnt      = $4b ; to $4c
dlend      = $4d ; to $64 for zone height=8, to $59 for zone height=16
dlendsave  = $65 ; to $7c for zone height=8, to $71 for zone height=16

visibleover    = $7d

pokeybase      = $7e
pokeybaselo    = $7e
pokeybasehi    = $7f

sfx1pointlo  = $80
sfx1pointhi  = $81
sfx2pointlo  = $82
sfx2pointhi  = $83
sfx1priority = $84
sfx2priority = $85
sfx1poffset  = $86
sfx2poffset  = $87
sfx1frames   = $88
sfx2frames   = $89
sfx1tick     = $8a
sfx2tick     = $8b

; ** RESERVED $8c to $E5

A = $e6
a = $e6
B = $e7
b = $e7
C = $e8
c = $e8
D = $e9
d = $e9
E = $ea
e = $ea
F = $eb
f = $eb
G = $ec
g = $ec
H = $ed
h = $ed
I = $ee
i = $ee
J = $ef
j = $ef
K = $f0
k = $f0
L = $f1
l = $f1
M = $f2
m = $f2
N = $f3
n = $f3
O = $f4
o = $f4
P = $f5
p = $f5
Q = $f6
q = $f6
R = $f7
r = $f7
S = $f8
s = $f8
T = $f9
t = $f9
U = $fa
u = $fa
V = $fb
v = $fb
W = $fc
w = $fc
X = $fd
x = $fd
Y = $fe
y = $fe
Z = $ff
z = $ff

; var0-var99 variables use the top of the stack. The stack still will have 92 bytes free.
var0 = $140
var1 = $141
var2 = $142
var3 = $143
var4 = $144
var5 = $145
var6 = $146
var7 = $147
var8 = $148
var9 = $149
var10 = $14a
var11 = $14b
var12 = $14c
var13 = $14d
var14 = $14e
var15 = $14f
var16 = $150
var17 = $151
var18 = $152
var19 = $153
var20 = $154
var21 = $155
var22 = $156
var23 = $157
var24 = $158
var25 = $159
var26 = $15a
var27 = $15b
var28 = $15c
var29 = $15d
var30 = $15e
var31 = $15f
var32 = $160
var33 = $161
var34 = $162
var35 = $163
var36 = $164
var37 = $165
var38 = $166
var39 = $167
var40 = $168
var41 = $169
var42 = $16a
var43 = $16b
var44 = $16c
var45 = $16d
var46 = $16e
var47 = $16f
var48 = $170
var49 = $171
var50 = $172
var51 = $173
var52 = $174
var53 = $175
var54 = $176
var55 = $177
var56 = $178
var57 = $179
var58 = $17a
var59 = $17b
var60 = $17c
var61 = $17d
var62 = $17e
var63 = $17f
var64 = $180
var65 = $181
var66 = $182
var67 = $183
var68 = $184
var69 = $185
var70 = $186
var71 = $187
var72 = $188
var73 = $189
var74 = $18a
var75 = $18b
var76 = $18c
var77 = $18d
var78 = $18e
var79 = $18f
var80 = $190
var81 = $191
var82 = $192
var83 = $193
var84 = $194
var85 = $195
var86 = $196
var87 = $197
var88 = $198
var89 = $199
var90 = $19a
var91 = $19b
var92 = $19c
var93 = $19d
var94 = $19e
var95 = $19f
var96 = $1a0
var97 = $1a1
var98 = $1a2
var99 = $1a3

