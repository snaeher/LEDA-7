
.SUFFIXES: $(so) .a

.a$(so):
	@rm -f -r $*.a_tmp
	@mkdir $*.a_tmp
	@cd $*.a_tmp; ar x ../$*.a; rm -f _dladdr.o
	$(CCLIB) -o $*$(so) $*.a_tmp/*.o -lX11 -lXft
	@rm -f -r $*.a_tmp

default: libleda$(so)

