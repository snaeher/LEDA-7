@echo off

if not "%LEDAROOT%" == "" goto skip1

set LEDAROOT=D:\LEDA\BETA\win32\bcc

rem echo.
rem echo xlman: LEDAROOT (environment variable) not defined.
rem echo.
rem goto end

:skip1

if exist %LEDAROOT%\demo\xlman goto skip2

echo.
echo xlman: "%LEDAROOT%\demo\xlman" is not a directory.
echo Please check your LEDAROOT environment variable.
echo.
goto end

:skip2

path %LEDAROOT%;%path%


%LEDAROOT%\demo\xlman\xlman

:end

