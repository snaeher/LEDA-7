
LROOT   = ../..
#UNIXLIB = -lleda $(XLIB) -lssl -lcrypto
UNIXLIB = -lleda $(XLIB)
DOSLIB  = ..\\..\\leda.lib $(XLIB)

include ../Make.pro


install: ws_server
	-ws.stop
	cp ws_server ~/htdocs/leda/demos/app
	ws.start&
	sleep 1
	server


