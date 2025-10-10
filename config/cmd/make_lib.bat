@echo off

:: (Re-)Build LIB from all existing obj-files

if exist leda.lib del leda.lib
if exist lib.tmp rmdir /q /s lib.tmp

:: collect all object files to lib.tmp

mkdir lib.tmp

for /R src\ %%x IN (*.obj) DO (
   echo %%x
   copy %%x lib.tmp > NUL
)


:: build leda.lib

echo.

cd lib.tmp
lib -nologo -out:..\leda.lib *.obj
cd ..

