 processor 6502
 ;include "macro.h"
 include "7800basic.h"
 include "7800basic_variable_redefs.h"

 ;start address of cart...
 ifconst ROM48k
   ORG $4000
 else
   ifconst bankswitchmode
     ORG  $8000
     RORG $8000
   else
     ORG $8000
   endif
 endif

