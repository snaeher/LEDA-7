SHELL = /bin/sh

all:
	@if [ "`uname -m`" != "SX-5" ]; then \
	if [ "`uname -s`" = HP-UX ]; then \
	echo "cd hppa; $(MAKE)"; cd hppa; $(MAKE);else \
	if [ "`uname -p`" = powerpc ]; then \
	echo "cd powerpc;$(MAKE)"; cd powerpc;$(MAKE);fi; \
	if [ "`uname -p`" = sparc ]; then \
	echo "cd sparc; $(MAKE)"; cd sparc; $(MAKE);fi; \
	case `uname -m` in i*86*) \
	echo "cd x86;  $(MAKE)"; cd x86;  $(MAKE);;esac; \
	if [ "`uname -m`" = "x86_64" ]; then \
	echo "cd x86;$(MAKE)"; cd x86; $(MAKE); fi; \
	if [ "`uname -p`" = mips ]; then \
	echo "cd mips;$(MAKE)"; cd mips;$(MAKE);fi; \
	if [ "`uname -m`" = "alpha" ]; then \
	echo "cd alpha;$(MAKE)"; cd alpha;$(MAKE);fi;fi;fi

obj: 
	@if [ "`uname -m`" != "SX-5" ]; then \
	if [ "`uname -s`" = HP-UX ]; then \
	echo "cd hppa; $(MAKE)"; cd hppa; $(MAKE) obj;else \
	if [ "`uname -p`" = powerpc ]; then \
	echo "cd powerpc;$(MAKE)"; cd powerpc;$(MAKE) obj;fi; \
	if [ "`uname -p`" = sparc ]; then \
	echo "cd sparc; $(MAKE)"; cd sparc; $(MAKE) obj;fi; \
	case `uname -m` in i*86*) \
	echo "cd x86;  $(MAKE)"; cd x86;  $(MAKE) obj;;esac; \
	if [ "`uname -m`" = "x86_64" ]; then \
	echo "cd x86;$(MAKE)"; cd x86; $(MAKE) obj; fi; \
	if [ "`uname -p`" = mips ]; then \
	echo "cd mips;$(MAKE)"; cd mips;$(MAKE) obj;fi; \
	if [ "`uname -p`" = alpha ]; then \
	echo "cd alpha;$(MAKE)"; cd alpha;$(MAKE) obj;fi;fi;fi


clean: 
	@if [ "`uname -m`" != "SX-5" ]; then \
	if [ "`uname -s`" = HP-UX ]; then \
	echo "cd hppa; $(MAKE)"; cd hppa; $(MAKE) clean;else \
	if [ "`uname -p`" = powerpc ]; then \
	echo "cd powerpc;$(MAKE)"; cd powerpc;$(MAKE) clean;fi; \
	if [ "`uname -p`" = sparc ]; then \
	echo "cd sparc; $(MAKE)"; cd sparc; $(MAKE) clean;fi; \
	case `uname -m` in i*86*) \
	echo "cd x86;  $(MAKE)"; cd x86;  $(MAKE) clean;;esac; \
	if [ "`uname -m`" = "x86_64" ]; then \
	echo "cd x86;$(MAKE)"; cd x86; $(MAKE) clean; fi; \
	if [ "`uname -p`" = mips ]; then \
	echo "cd mips;$(MAKE)"; cd mips;$(MAKE) clean;fi; \
	if [ "`uname -p`" = alpha ]; then \
	echo "cd alpha;$(MAKE)"; cd alpha;$(MAKE) clean;fi;fi;fi


touch: 
	@if [ "`uname -m`" != "SX-5" ]; then \
	if [ "`uname -s`" = HP-UX ]; then \
	echo "cd hppa; $(MAKE)"; cd hppa; $(MAKE) touch;else \
	if [ "`uname -p`" = powerpc ]; then \
	echo "cd powerpc;$(MAKE)"; cd powerpc;$(MAKE) touch;fi; \
	if [ `uname -p` = sparc ]; then \
	echo "cd sparc; $(MAKE)"; cd sparc; $(MAKE) touch;fi; \
	case `uname -m` in i*86*) \
	echo "cd x86;  $(MAKE)"; cd x86;  $(MAKE) touch;;esac; \
	if [ "`uname -m`" = "x86_64" ]; then \
	echo "cd x86;$(MAKE)"; cd x86; $(MAKE) touch; fi; \
	if [ "`uname -p`" = mips ]; then \
	echo "cd mips;$(MAKE)"; cd mips;$(MAKE) touch;fi; \
	if [ "`uname -p`" = alpha ]; then \
	echo "cd alpha;$(MAKE)"; cd alpha;$(MAKE) touch;fi;fi;fi

