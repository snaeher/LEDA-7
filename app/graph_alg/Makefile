WFLAGS = -DWINMAIN

LROOT   = ../..
#UNIXLIB = -lleda $(XLIB)  -lssl -lcrypto
UNIXLIB = -lleda $(XLIB)
DOSLIB  = ..\\..\\leda.lib $(XLIB)
DOSLIB1 = ..\\..\\GeoWin.lib

include ../Make.pro


install: all
	@dir=~/htdocs/leda/demos/app/graph_alg;\
	mkdir -p $$dir;\
	for x in $(PROGS);do echo $$dir/$$x; cp $$x $$dir; done;

