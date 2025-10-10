.SUFFIXES: .obj .exe

IPATH = $(LROOT)/incl

e     = .exe

.cpp.obj:
	$(CC) $(CFLAGS) $(DFLAGS) $(WFLAGS) $(IFLAGS) -I$(IPATH) $*.cpp

.obj.exe:
	$(LD) $(LFLAGS) $*.obj $(OBJECTS) $(DOSLIB)
#	mt -nologo -manifest $(LROOT)/manifest/manifest.dpi -outputresource:$*$(e)

include Make.lst

all:  $(SUBDIRS) $(OBJECTS) $(PROGS)
	@true

clean:
	rm -f *.obj */*.obj core

del:
	rm -f *.obj */*.obj $(PROGS) core

gdi: $(PROGS)
	manifest gdi $(PROGS)

