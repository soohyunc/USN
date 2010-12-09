@echo off
rem RCS: @(#) $Id: rmd.bat,v 1.1.1.1 2005/06/16 23:34:33 soohyunc Exp $

if not exist %1\nul goto end

echo Removing directory %1

if "%OS%" == "Windows_NT" goto winnt

deltree /y %1
if errorlevel 1 goto end
goto success

:winnt
rmdir /s /q %1
if errorlevel 1 goto end

:success
echo Deleted directory %1

:end

