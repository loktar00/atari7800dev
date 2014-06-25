; Inclues go below - the order is crucial, since this is also the order in
; which they will appear in the generated assembly file
;

; some macro definitions. no actual code.
macro.h

; header file...
7800basicheader.asm

; the assembly code generated from the user's basic code
7800.asm

; push the routines that follow this include into the last 4k
last4k.asm

; below are collections of subroutines and functions
std_routines.asm

; start-up routines. 
startup.asm

; the cart end vectors...
7800basicfooter.asm

