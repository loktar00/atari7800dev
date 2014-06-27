@echo off
if X"%bas7800dir%" == X goto nobasic
7800preprocess <"%~f1" | 7800basic.exe -i "%bas7800dir%"
if errorlevel 1 goto basicerror
if X%2 == X-O goto optimize
7800postprocess -i "%bas7800dir%" > "%~f1.asm"
goto nooptimize
:optimize
7800postprocess -i "%bas7800dir%" | 7800optimize >"%~f1.asm"
:nooptimize
dasm "%~f1.asm" -I"%bas7800dir%"/includes -f3 -o"%~f1.bin" | 7800filter
7800sign -w "%~f1.bin"
7800header -n "%~f1.bin" -f a78info.cfg <  "%~f1.bin" > "%~f1.a78"
goto end

:nobasic
echo bas7800dir environment variable not set.

:basicerror
echo Compilation failed.

:end
