
INCL = $(LROOT)\incl
LLIB = $(DOSLIB)


.c.obj:
	$(PLAIN_C) $(CFLAGS) $(DFLAGS) $(WFLAGS) $(IFLAGS) -I$(INCL) $*.c

.cpp.obj:
	$(CC) $(CFLAGS) $(DFLAGS) $(WFLAGS) $(IFLAGS) -I$(INCL) $*.cpp

.obj.exe:
	$(LD) $(LFLAGS) $*.obj $(LLIB)

.c.exe:
	$(CC) $(CFLAGS) $(DFLAGS) $(WFLAGS) $(IFLAGS) -I$(INCL) $*.c
	$(LD) $(LFLAGS) $*.obj $(LLIB)

.cpp.exe:
	$(CC) $(CFLAGS) $(DFLAGS) $(WFLAGS) $(IFLAGS) -I$(INCL) $*.cpp
	$(LD) $(LFLAGS) $*.obj $(LLIB)

e=.exe

include Make.lst

all: $(PROGS)

clean:
	del *.obj

del:
	del *.obj
	del $(PROGS)

