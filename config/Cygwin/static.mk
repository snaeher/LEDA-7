CDPATH=

VERS = 
compiler = msc

CC      = cl -nologo -W3 -WX -c -Zm300 -TP
PLAIN_C = cl -nologo -W3 -WX -c -Zm300 -TC
LD      = cl -nologo -MT
CFLAGS  = -EHsc -Ox -MT

OBJECTS = `find src -name "*.obj"`

all: $(compiler)

msc:
	@echo " "
	@echo Building static library "leda.lib"
	@rm -r -f lib.tmp
	@mkdir lib.tmp
	@cp $(OBJECTS) lib.tmp
	@cd lib.tmp; lib -nologo -out:../leda.lib *.obj
	@rm -r -f lib.tmp





