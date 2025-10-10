WFLAGS = -DWINMAIN

LROOT   = ../..
#UNIXLIB = -lleda $(XLIB)  -lssl -lcrypto
UNIXLIB = -lleda $(XLIB)
DOSLIB  = ..\\..\\leda.lib $(XLIB)

include ../Make.pro


install: all
	@dir=~/htdocs/leda/demos/app/speed/bin;\
	mkdir -p $$dir;\
	for x in $(PROGS);do echo $$dir/$$x; cp $$x $$dir; done;

