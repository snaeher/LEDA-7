SHELL=/bin/sh

default: lib apps

all: lib apps demos tests

#all: lib apps demos tests dvi man

.PHONY: lib apps demos tests dvi man

#------------------------------------------------------------------------------
# libraries
#------------------------------------------------------------------------------

.lconfig:
	@if [ ! -f .lconfig ]; then echo; \
        echo "LEDA is not configured: Please run lconfig !"; fi


lib: .lconfig
	@if [ -f .lconfig ]; then $(MAKE) -C src obj; \
	if [ -f shared.mk ]; then $(MAKE) shared; \
	elif [ -f static.mk ]; then $(MAKE) static; fi; fi

touch: .lconfig
	@if [ -f .lconfig]; then $(MAKE) -C src touch; fi
	@if [ -f shared.mk ]; then $(MAKE) shared; \
	elif [ -f static.mk ]; then $(MAKE) static; fi


objects: .lconfig
	$(MAKE) -C src obj

x11: .lconfig
	$(MAKE) -C src/graphics/x11

mswin: .lconfig
	$(MAKE) -C src/graphics/mswin 

xx: .lconfig
	$(MAKE) -C src/graphics/xx install
	$(MAKE) -C app install

shared: .lconfig
	@$(MAKE) -f shared.mk

static: .lconfig
	@$(MAKE) -f static.mk



#------------------------------------------------------------------------------
# programs
#------------------------------------------------------------------------------

apps: .lconfig
	@if [ -f .lconfig ]; then $(MAKE) -C app; fi

demos: .lconfig
	@if [ -f .lconfig ]; then $(MAKE) -C demo; fi

tests: .lconfig
	@if [ -f .lconfig ]; then $(MAKE) -C test; fi

res: .lconfig
	@if [ -f .lconfig ]; then $(MAKE) -C res; fi


#------------------------------------------------------------------------------
# manual
#------------------------------------------------------------------------------

man: .lconfig
	$(MAKE) -C Manual/MANUAL

pdfman: .lconfig
	$(MAKE) -C Manual/MANUAL  pdf

dvi: .lconfig
	$(MAKE) -C Manual/MANUAL  dvi



#------------------------------------------------------------------------------
# cleaning up
#------------------------------------------------------------------------------

del:
	@if [ -d src   ]; then $(MAKE) -C src    clean; fi
	@if [ -d app   ]; then $(MAKE) -C app    del; fi
	@if [ -d demo  ]; then $(MAKE) -C demo   del; fi
	@if [ -d test  ]; then $(MAKE) -C test   del; fi
	@if [ -d prog  ]; then $(MAKE) -C prog   del; fi
	@if [ -d res   ]; then $(MAKE) -C res    del; fi
	rm -f lib*.a lib*.so lib*.sl lib*.lib leda.dll leda.lib

clean:
	@if [ -d src   	]; then $(MAKE) -C src    clean; fi
	@if [ -d app   	]; then $(MAKE) -C app    clean; fi
	@if [ -d demo  	]; then $(MAKE) -C demo   clean; fi
	@if [ -d test  	]; then $(MAKE) -C test   clean; fi
	@if [ -d prog  	]; then $(MAKE) -C prog   clean; fi
	@if [ -d res  	]; then $(MAKE) -C res    clean; fi

