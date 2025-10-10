@echo off

REM %1   compiler
REM %2-4 dll mt debug

SETLOCAL EnableDelayedExpansion


set confdir=config\Windows

if "%1" == "" goto :usage

REM check arguments and set options

set cc=%1
set cc_file=config\cc\%cc%
if not exist %cc_file% goto :usage

set build_dll=0
set options=

for %%x in (%*) do (

  if not %%x == %cc% set options=!options! %%x

  if %%x == dll    set build_dll=1

  if %%x == mt     set cc_file=%cc_file%-mt
  if %%x == debug  set cc_file=%cc_file%-debug
)

if "%Platform%" == "" set Platform=x86

echo.
echo compiler: %cc% (%Platform%)
echo options: %options%


set make_src=%confdir%\Make.src
set make_pro=%confdir%\Make.pro

if %build_dll% == 1 (

  set make_src=%confdir%\Make.src.dll
  set make_pro=%confdir%\Make.pro.dll

  if not exist %make_src% (
    echo.
    echo Sorry - DLLs are not supported in this configuration.
    echo.
    goto :exit 
  )

  echo.
  echo "Setting up makefiles to build a DLL (leda.dll) with %cc% (%Platform%) ..."

) else (

  echo.
  echo "Setting up makefiles to build a static LIB (leda.lib) with %cc% (%Platform%) ..."
)

REM copy config\cmd\*.bat . > NUL

echo.
echo cc_file  = %cc_file%
echo make.src = %make_src%
echo make.pro = %make_pro%
echo.


rem SETUP Make.src and Make.pro Files

copy %cc_file% cc.tmp  > NUL

if "%Platform%" == "x64" ( 
  echo ASM = yasm -m amd64 -f win32 >> cc.tmp 
) else ( 
  echo ASM = nasm -Dwin32 -f win32 >> cc.tmp 
)
echo. >> cc.tmp

if %build_dll% == 0 (
  echo XLIB = user32.lib gdi32.lib msimg32.lib comdlg32.lib shell32.lib advapi32.lib wsock32.lib >> cc.tmp
  echo. >> cc.tmp
) 


type cc.tmp

copy cc.tmp + %make_src% src\Make.src  > NUL
copy cc.tmp + %make_pro% app\Make.pro  > NUL
copy cc.tmp + %make_pro% demo\Make.pro > NUL
copy cc.tmp + %make_pro% test\Make.pro > NUL
copy cc.tmp + %make_pro% res\Make.pro  > NUL
copy cc.tmp + %make_pro% prog\Make.pro > NUL

del cc.tmp > NUL

rem END OF Make.[src|pro] SETUP


copy config\cmd\build.bat . > NUL

echo.
if %build_dll% == 1 (
  copy config\cmd\make_dll.bat . > NUL
  echo Now type "build lib" to build leda.dll
) else (
  copy config\cmd\make_lib.bat . > NUL
  echo Now type "build lib" to build leda.lib
)
echo.

goto :exit


:usage

echo.
echo Usage: lconfig cc [dll] [mt] [debug]
echo.
echo Possible compilers (cc) and options (opts):
echo.
echo msc:         Microsoft Visual C++      (opts: dll mt debug)
echo.

:exit

set cc=
set opts=
set confdir=
set build_dll=

exit /b 0

