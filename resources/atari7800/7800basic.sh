#!/bin/sh

# do some quick sanity checking...

if [ ! "$bas7800dir" ] ; then
  echo "### WARNING: the bas7800dir envionronment variable isn't set."
fi

OSTYPE=$(uname -s)
ARCH=$(uname -m)
for EXT in "" .$OSTYPE.x86 .$OSTYPE.$ARCH .$OSTYPE ; do
  echo | 7800preprocess$EXT 2>/dev/null >&2 && break
done

echo | 7800preprocess$EXT 2>/dev/null >&2 && break
if [ ! $? = 0 ] ; then
  echo "### ERROR: couldn't find 7800basic binaries for $OSTYPE($ARCH). Exiting."
  exit 1
fi

#do dasm separately, because it's distributed separately
for DASMEXT in "" .$OSTYPE.x86 .$OSTYPE.$ARCH .$OSTYPE ; do
  dasm$DASMEXT 2>/dev/null >&2 
  [ $? = 1 ] && break
done
dasm$DASMEXT 2>/dev/null >&2 
if [ ! $? = 1 ] ; then
  echo "### ERROR: couldn't find dasm binary for $OSTYPE($ARCH). Exiting."
  exit 1
fi

DV=$(dasm$DASMEXT 2>/dev/null | grep ^DASM | head -n1)
echo "Using dasm version: $DV" 
  
echo "Starting build of $1"
7800preprocess$EXT<"$1" | 7800basic$EXT -i "$bas7800dir" 
if [ "$?" -ne "0" ]
 then
  echo "Compilation failed."
  exit
fi
if [ "$2" = "-O" ]
  then
   7800postprocess$EXT -i "$bas7800dir" | 7800optimize$EXT > "$1.asm"
  else
   7800postprocess$EXT -i "$bas7800dir" > "$1.asm"
fi

dasm$DASMEXT $1.asm -I"$bas7800dir/includes" -f3 -l"$1.list.txt" -s"$1.symbol.txt" -o"$1.bin" | 7800filter$EXT 
7800sign$EXT -w "$1.bin"
7800header$EXT  -n "$1" -f a78info.cfg < "$1.bin" > "$1.a78"
	
exit 0
