
leda.dll: libleda.a
	@echo
	@echo Build "leda.dll" and "libleda_dll.a"
	@g++ -shared -o leda.dll \
	-Wl,--out-implib=libleda_dll.a \
	-Wl,--export-all-symbols \
	-Wl,--enable-auto-import \
	-Wl,--whole-archive libleda_static.a -Wl,--no-whole-archive \
	-luser32 -lgdi32 -lmsimg32 -lcomdlg32 -lshell32 -lwsock32
	@echo

#.PHONY: dll
#
#OBJECTS = `find src -name "*.o"`
#
#dll:
#	@echo
#	@echo Build "leda.dll" and "libleda.a"
#	@g++ -shared -o leda.dll \
#	-Wl,--out-implib=libleda.a \
#	-Wl,--export-all-symbols \
#	-Wl,--enable-auto-import \
#	$(OBJECTS) -luser32 -lgdi32 -lmsimg32 -lcomdlg32 -lshell32 -lwsock32
#	@echo


