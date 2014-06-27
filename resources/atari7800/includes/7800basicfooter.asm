  ifconst DEV
    ifnconst ZONEHEIGHT
      echo "* the 4k 7800basic area has",[($FFF8 - *)]d,"bytes free."
    else
      if ZONEHEIGHT =  8
        echo "* the 2k 7800basic area has",[($FFF8 - *)]d,"bytes free."
      else
        echo "* the 4k 7800basic area has",[($FFF8 - *)]d,"bytes free."
      endif
    endif
  endif

  ifnconst bankswitchmode 
    ORG $FFF8
  else
    ifconst ROM128K
      ORG $27FF8
      RORG $FFF8
    endif
    ifconst ROM256K
      ORG $47FF8
      RORG $FFF8
    endif
    ifconst ROM512K
      ORG $87FF8
      RORG $FFF8
    endif
  endif


  .byte   $FF	; region verification. $FF=all regions
  .byte   $F7	; high nibble:  encryption check from $N000 to $FF7F. we only hash the last 4k for faster boot.
		; low nibble :  N=7 atari rainbow start, N=3 no atari rainbow

  ;Vectors
  .word NMI
  .word START
  .word IRQ

