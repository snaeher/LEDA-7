.PHONY: libleda.a

OBJECTS = `find src -name "*.o"`

libleda.a: 
	@echo " "
	@echo Building static library libleda.a
	@rm -f libleda.a
	@ar cr libleda.a $(OBJECTS)
	@echo " "




