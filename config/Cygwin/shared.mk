
OBJECTS1 = $(shell find src -name "*.obj" | grep -v geowin)

OBJECTS2 = $(shell find src/graphics/geowin -name "*.obj")

SYSLIBS = user32.lib gdi32.lib msimg32.lib shell32.lib comdlg32.lib advapi32.lib wsock32.lib opengl32.lib

all: leda.lib GeoWin.lib
	@true

leda.lib: $(OBJECTS1)
	@echo; 
	@echo update objects
	@for x in $?; do echo $$x; done;
	@echo
	@echo Build "leda.dll" and "leda.lib"
	@link -nologo -DLL -OUT:leda.dll $(OBJECTS1) $(SYSLIBS)
	@touch leda.lib

GeoWin.lib: $(OBJECTS2)
	@echo; 
	@echo update objects
	@for x in $?; do echo $$x; done;
	@echo
	@echo Build "GeoWin.lib"
	@lib -nologo -out:GeoWin.lib $(OBJECTS2)


