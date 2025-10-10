@echo off

SETLOCAL EnableDelayedExpansion

set directory=

if "%1" == "lib"  set directory=src
if "%1" == "demo" set directory=demo
if "%1" == "test" set directory=test

if "%directory%" == "" (
  echo. 
  echo build.bat [lib^|demo^|test]  {clean^|del}
  echo.
  exit /b 1
)

set target=%2
if %1 == lib if "%2" == "del" set target=clean

echo.
echo directory = %directory%
echo target = %target%
echo.

set make=nmake -nologo

set root=%cd%

echo %root%

REM  directory = src|demo|prog
REM  call make in every subdirectory with a Make.lst file
REM  but only in x86 subdir of src/numbers/asm branch


if "%Platform%" == "x64" (
  set asm_dir=
) else (
  set asm_dir=x86_32
) 

for /D /R %directory%\ %%x IN (*) DO (

  set call_make=true

  if not exist %%x\Make.lst set call_make=false
   
  echo %%x | FINDSTR /C:"numbers\asm" > NUL 

  if not ERRORLEVEL 1 (
    REM x contains substring 'numbers\asm'
    if not %%x == %root%\src\numbers\asm\%asm_dir% set call_make=false
  )

  if %%x == %root%\src\graphics\x11 set call_make=false

  if %%x == %root%\src\graphics\xx  set call_make=false
   
 if !call_make! == true (
   echo %%x
   cd %%x
   %make% -i %target%
   cd %root%
 )
   
)

if not "%1" == "lib" exit /b 0

if "%2" == "del" (
  if exist leda.lib del leda.lib
  if exist leda.dll del leda.dll
  if exist leda.exp del leda.exp
  if exist GeoWin.lib del GeoWin.lib
  exit /b 0
)

if exist make_lib.bat call make_lib.bat
if exist make_dll.bat call make_dll.bat

