@echo off
chcp 65001 >nul

REM Build script for MSVC (cl.exe).
REM Run from "Developer Command Prompt for VS" or after setting up vcvars.

cl /nologo /EHsc /std:c++17 /utf-8 /Fe:dz_itog.exe main.cpp Ploshchadka.cpp

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Build failed. Make sure cl.exe is in PATH.
    pause
    exit /b 1
)

del /Q *.obj 2>nul

echo.
echo Build OK. Run: dz_itog.exe
