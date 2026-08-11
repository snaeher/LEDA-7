.PHONY: libleda.a

CDPATH =

libleda.a: 
	@echo " "
	@echo "Building static library libleda.a (macOS safe)"
	@rm -f libleda.a libleda.so libleda.dylib
	@find src -name "*.o" -exec ar r libleda.a {} +
	@ranlib libleda.a