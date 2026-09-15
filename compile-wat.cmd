@echo off
rem compile-wat.cmd - Birdman for OS/2, Open Watcom build script

set LOGFILE=compile-wat.log
echo Birdman Build Log > %LOGFILE%
echo ================== >> %LOGFILE%
echo. >> %LOGFILE%

rem Auto-detect WATCOM
if exist c:\watcom2\binp\wcc386.exe set WATCOM=c:\watcom2
if exist c:\watcom\binp\wcc386.exe  set WATCOM=c:\watcom
if "%WATCOM%"=="" goto noWatcom

rem Default OS2TK
if "%OS2TK%"=="" set OS2TK=c:\os2tk45

echo WATCOM : %WATCOM% >> %LOGFILE%
echo OS2TK  : %OS2TK% >> %LOGFILE%
echo. >> %LOGFILE%

set PATH=%WATCOM%\binp;%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\os2;%INCLUDE%

echo Cleaning... >> %LOGFILE%
wmake -f makefile.wat clean >> %LOGFILE%

echo. >> %LOGFILE%
echo Building... >> %LOGFILE%
wmake -f makefile.wat all >> %LOGFILE%

if not exist bin\birdman.exe goto failed

echo.
echo BUILD OK - bin\birdman.exe
echo BUILD OK >> %LOGFILE%
goto end

:noWatcom
echo ERROR: Open Watcom not found (tried c:\watcom and c:\watcom2)
echo ERROR: Open Watcom not found >> %LOGFILE%
goto end

:failed
echo.
echo BUILD FAILED - see %LOGFILE%
echo BUILD FAILED >> %LOGFILE%

:end
