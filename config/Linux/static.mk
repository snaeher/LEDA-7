.PHONY: libleda.a

CDPATH =

OBJECTS = `find src -name "*.o"`

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




