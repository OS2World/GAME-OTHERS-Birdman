#
# makefile.wat - Birdman for OS/2, Open Watcom wmake
#
# Build: wmake -f makefile.wat
# Clean: wmake -f makefile.wat clean
#

CC     = wcc386
RC     = wrc
LINK   = wlink

OS2TK  = $(%OS2TK)
!ifndef OS2TK
OS2TK  = c:\os2tk45
!endif

CFLAGS = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=$(OS2TK)\h -i=src
RCFLAGS = -i=$(OS2TK)\h -i=src
LFLAGS = system os2v2 pm option stack=65536 option heap=4096 option map=bin\birdman.map

EXE = bin\birdman.exe
OBJ = bin\birdman.obj
RES = bin\birdman.res

all: $(EXE)

$(OBJ): src\birdman.c src\birdman.h
	$(CC) $(CFLAGS) -fo=$(OBJ) src\birdman.c

$(RES): src\birdman.rc src\birdman.h src\bird.bmp src\gun.ptr
	$(RC) $(RCFLAGS) -r src\birdman.rc -fo=$(RES)

$(EXE): $(OBJ) $(RES)
	$(LINK) $(LFLAGS) name $(EXE) file $(OBJ)
	$(RC) $(RES) $(EXE)

clean: .symbolic
	@if exist bin\birdman.obj del bin\birdman.obj
	@if exist bin\birdman.res del bin\birdman.res
	@if exist bin\birdman.exe del bin\birdman.exe
	@if exist bin\birdman.map del bin\birdman.map
