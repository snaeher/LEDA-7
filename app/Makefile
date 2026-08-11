#
all:
	@for f in *; do if [ -f $$f/Makefile ]; \
	then echo; echo `pwd`/$$f; $(MAKE) -C $$f all; fi; done
	
obj:
	@for f in *; do if [ -f $$f/Makefile ]; \
	then echo; echo `pwd`/$$f; $(MAKE) -C $$f obj; fi; done

clean:
	@for f in *; do if [ -f $$f/Makefile ]; \
	then echo; echo `pwd`/$$f; $(MAKE) -C $$f clean; fi; done

del:
	@for f in *; do if [ -f $$f/Makefile ]; \
	then echo; echo `pwd`/$$f; $(MAKE) -C $$f del; fi; done

touch:
	@for f in *; do if [ -f $$f/Makefile ]; \
	then echo; echo `pwd`/$$f; $(MAKE) -C $$f touch; fi; done

xx:
	@echo;echo INSTALL APPS
	ws -k
	sleep 1
	@for f in *; do if [ -f $$f/Makefile -a $$f != server ]; \
	then echo;echo `pwd`/$$f; $(MAKE) -C $$f xx; fi; done
