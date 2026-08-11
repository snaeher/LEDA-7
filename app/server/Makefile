
LROOT   = ../..
#UNIXLIB = -lleda $(XLIB) -lssl -lcrypto
UNIXLIB = -lleda $(XLIB)
DOSLIB  = ..\\..\\leda.lib $(XLIB)

include ../Make.pro

install: ws_server
	-ws.stop
	@echo
	cp ws_server /local/www/leda/demos/app
	ws.start&
	@sleep 1
	@echo
	server
	@echo

