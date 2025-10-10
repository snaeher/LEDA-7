
.PHONY: leda.dll

OBJECTS = `find src -name "*.o"`

leda.dll:
	@echo
	@echo Build DLL "leda.dll" and import lib "libleda.so"
	@g++ -shared -o leda.dll \
	-Wl,--out-implib=libleda.so \
	-Wl,--export-all-symbols \
	-Wl,--enable-auto-import \
	$(OBJECTS) -lX11 -lXft -luser32 -lgdi32 -lmsimg32 -lcomdlg32 -lshell32 -lwsock32
	@echo

#	-Wl,--whole-archive libleda.a -Wl,--no-whole-archive \

