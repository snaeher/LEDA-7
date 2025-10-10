.PHONY: libleda.a libleda.so

CDPATH =

OBJECTS = `find src -name "*.o"`

libleda.so: libleda.a
	@echo Building shared library libleda.so
	@$(CCLIB) -o libleda.so -Wl,--whole-archive libleda.a -Wl,--no-whole-archive

libleda.a: 
	@echo " "
	@echo Building static library libleda.a
	@rm -r -f libleda.a libleda.so 
	@ar cr libleda.a $(OBJECTS)

#libleda.a: 
#	@echo " "
#	@echo Building static library libleda.a
#	@rm -r -f lib.tmp libleda.a libleda.so 
#	@mkdir lib.tmp
#	@cp $(OBJECTS) lib.tmp
#	@cd lib.tmp; ar cr ../libleda.a *.o
#	@rm -r -f lib.tmp





