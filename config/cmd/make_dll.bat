@echo off

:: (Re-) Build DLL from all existing obj-files

set SYSLIBS=user32.lib gdi32.lib msimg32.lib shell32.lib comdlg32.lib advapi32.lib wsock32.lib opengl32.lib

if exist leda.lib del leda.lib
if exist leda.dll del leda.dll
if exist leda.exp del leda.exp

:: collect object files to lib.tmp (omit geowin files)

echo.
echo Building leda.dll and leda.lib
echo.

if exist objects.txt del objects.txt

for /R src %%x IN (*.obj) DO (
  echo %%x | FINDSTR /C:"geowin" > NUL 
  if ERRORLEVEL 1 echo %%x >> objects.txt
)

if exist objects.txt (
:: cl -LD -Feleda  @objects.txt %SYSLIBS%
   link -DLL -OUT:leda.dll @objects.txt %SYSLIBS%
)

:: if exist lib.tmp rmdir /q /s lib.tmp
:: mkdir lib.tmp
:: for /R src %%x IN (*.obj) DO (
::   echo %%x | FINDSTR /C:"geowin" > NUL 
::   if ERRORLEVEL 1 copy %%x lib.tmp > NUL
:: )
:: 
:: cd lib.tmp
:: link -DLL -OUT:..\leda.dll *.obj %SYSLIBS%
:: cd ..


echo.
echo Build static GeoWin.lib
echo.

cd src\graphics\Geowin
lib -nologo -out:..\..\..\GeoWin.lib *.obj
cd ..\..\..

