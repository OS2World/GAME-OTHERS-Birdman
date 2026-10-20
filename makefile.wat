#
# makefile.wat - Birdman for OS/2, Open Watcom wmake
#
# Build: wmake -f makefile.wat
# Clean: wmake -f makefile.wat clean
#

CC     = wcc386
RC     = wrc
LINK   = wlink
IPFC   = wipfc

OS2TK  = $(%OS2TK)
!ifndef OS2TK
OS2TK  = c:\os2tk45
!endif

CFLAGS = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=$(OS2TK)\h -i=src
RCFLAGS = -i=$(OS2TK)\h -i=src
LFLAGS = system os2v2 pm option stack=65536 option heap=4096 option map=bin\birdman.map

HELPDIR = help
HLPDIR  = bin\help
HLPFILES = $(HLPDIR)\birdman_en.hlp $(HLPDIR)\birdman_es.hlp $(HLPDIR)\birdman_nl.hlp $(HLPDIR)\birdman_de.hlp $(HLPDIR)\birdman_fr.hlp $(HLPDIR)\birdman_it.hlp

EXE = bin\birdman.exe
OBJ = bin\birdman.obj
RES = bin\birdman.res

# the output folder is not in the repository: create it (wcc386 stops with
# "E1118 No such file or directory" when bin\ is missing)
.BEFORE
	@if not exist bin mkdir bin

all: $(EXE) $(HLPFILES) .SYMBOLIC

$(OBJ): src\birdman.c src\birdman.h
	$(CC) $(CFLAGS) -fo=$(OBJ) src\birdman.c

$(RES): src\birdman.rc src\birdman.h src\bird.bmp src\gun.ptr
	$(RC) $(RCFLAGS) -r src\birdman.rc -fo=$(RES)

$(EXE): $(OBJ) $(RES)
	$(LINK) $(LFLAGS) name $(EXE) file $(OBJ)
	$(RC) $(RES) $(EXE)

$(HLPDIR)\birdman_en.hlp: $(HELPDIR)\birdman_en.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -o $(HLPDIR)\birdman_en.hlp $(HELPDIR)\birdman_en.ipf

$(HLPDIR)\birdman_es.hlp: $(HELPDIR)\birdman_es.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -o $(HLPDIR)\birdman_es.hlp $(HELPDIR)\birdman_es.ipf

$(HLPDIR)\birdman_nl.hlp: $(HELPDIR)\birdman_nl.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -o $(HLPDIR)\birdman_nl.hlp $(HELPDIR)\birdman_nl.ipf

$(HLPDIR)\birdman_de.hlp: $(HELPDIR)\birdman_de.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -l de_DE -o $(HLPDIR)\birdman_de.hlp $(HELPDIR)\birdman_de.ipf

$(HLPDIR)\birdman_fr.hlp: $(HELPDIR)\birdman_fr.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -l fr_FR -o $(HLPDIR)\birdman_fr.hlp $(HELPDIR)\birdman_fr.ipf

$(HLPDIR)\birdman_it.hlp: $(HELPDIR)\birdman_it.ipf
	@if not exist $(HLPDIR) mkdir $(HLPDIR)
	$(IPFC) -q -o $(HLPDIR)\birdman_it.hlp $(HELPDIR)\birdman_it.ipf

clean: .symbolic
	@if exist bin\birdman.obj del bin\birdman.obj
	@if exist bin\birdman.res del bin\birdman.res
	@if exist bin\birdman.exe del bin\birdman.exe
	@if exist bin\birdman.map del bin\birdman.map
	@if exist $(HLPDIR)\birdman_*.hlp del $(HLPDIR)\birdman_*.hlp
