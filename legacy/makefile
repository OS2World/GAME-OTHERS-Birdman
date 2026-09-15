#
#	birdman saver module makefile for nmake
#
CFLAGS = /W2 /Wall /Ss /Gd- /O+ /G5

all: birdman.exe birdman.dll

clean:
    -del birdman.obj 2>NUL
    -del birdman.res 2>NUL
    -del birdman.dll 2>NUL
    -del birdman.def 2>NUL
    -del birdman.map 2>NUL
    -del birdman.asm 2>NUL
    -del birdexe.def 2>NUL

birdman.res: birdman.rc birdman.h bitmaps\bird.bmp bitmaps\gun.ptr makefile
    rc -r -x2 -n birdman.rc

birdman.dll: birdman.c birdman.h birdman.res birdman.def makefile
    icc $(CFLAGS) /Ge- /Gm /Ms birdman.c birdman.def /B"/NOE /MAP:FULL" /Fe birdman.dll
    rc -x2 -n birdman.res birdman.dll

birdman.exe: birdman.c birdman.h birdman.res birdexe.def makefile
    icc $(CFLAGS) /D_EXE_ /Gd- /Ge+ birdman.c birdexe.def /B"/NOE /MAP:FULL" /Fe birdman.exe
    rc -x2 -n birdman.res birdman.exe

birdman.def: makefile
    @echo creating <<$@
;
;	birdman.def
;	birdman saver module definition file
;
;	(C) 1998 Mike, Vienna
;

LIBRARY      birdman INITINSTANCE TERMINSTANCE
DESCRIPTION  'ScreenSaverModule BIRDMAN (C) 1998 Mike, Vienna'

DATA	MULTIPLE NONSHARED
EXPORTS
	StartScreenSaver @2
	StopScreenSaver  @3
<<keep

birdexe.def: makefile
    @echo creating <<$@
NAME        Birdman  WINDOWAPI
EXETYPE     OS2
DATA        MULTIPLE
STACKSIZE   16364
HEAPSIZE    0
PROTMODE
<<keep
