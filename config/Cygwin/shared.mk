
OBJECTS = $(shell find src -name "*.obj" | grep -v geowin)

SYSLIBS = user32.lib gdi32.lib msimg32.lib shell32.lib comdlg32.lib advapi32.lib wsock32.lib opengl32.lib

all: leda.lib
	@true

leda.lib: $(OBJECTS)
	@echo; 
	@echo update objects
	@for x in $?; do echo $$x; done;
	@echo
	@echo Build "leda.dll" and "leda.lib"
	@link -nologo -DLL -OUT:leda.dll $(OBJECTS) $(SYSLIBS)
	@touch leda.lib
	@echo " "
	@echo Build "GeoWin.lib"
	@lib -nologo -out:GeoWin.lib src\\graphics\\GeoWin\\*.obj

