
VPATH = ".\"

IPATH = $(LROOT)/incl
o     = .o


.c.asm:
	$(CC) $(CFLAGS) $(DFLAGS) $(IFLAGS) -I$(IPATH) -S $*.c

.cpp.o:
	$(CC) $(CFLAGS) $(DFLAGS) $(IFLAGS) -I$(IPATH) -c $*.cpp

.o:
	$(LD) $(LFLAGS) -o $* $*.o $(OBJECTS) $(XLIB_PATH) -L$(LROOT) $(UNIXLIB)

.cpp:
	$(CC) $(CFLAGS) $(DFLAGS) $(IFLAGS)  -I$(IPATH) -c $*.cpp
	$(LD) $(LFLAGS) -o $* $*.o $(OBJECTS) $(XLIB_PATH) -L$(LROOT) $(UNIXLIB)


include Make.lst

all:  $(SUBDIRS) $(OBJECTS) $(PROGS)
	@true


xx:   $(PROGS)
#	ws -k
	@cwd=`pwd`;\
	dir=/local/www/leda/demos/app/`basename $$cwd`;\
	mkdir -p $$dir;\
	for x in $(PROGS);do echo $$dir/$$x; cp $$x $$dir; done;


clean:
	rm -f *.o */*.o core

del:
	rm -f *.o */*.o $(PROGS) core

