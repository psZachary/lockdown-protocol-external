@echo off
setlocal enabledelayedexpansion

:: === CONFIG ===
set "GITHUB_USER=psZachary"
set "REPO_NAME=lockdown-protocol-external"
set "BRANCH=master"
set "ZIP_URL=https://github.com/%GITHUB_USER%/%REPO_NAME%/archive/refs/heads/%BRANCH%.zip"
set "ZIP_FILE=%REPO_NAME%.zip"
set "EXTRACT_DIR=%REPO_NAME%_extracted"
set "PROJECT_SUBDIR=%REPO_NAME%-%BRANCH%"
set "PROJECT_FILE=protocol.vcxproj"
set "VCVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "CONFIGURATION=Release"
set "PLATFORM=x64"

:: === CLEANUP ===
if exist "%ZIP_FILE%" del /f /q "%ZIP_FILE%"
if exist "%EXTRACT_DIR%" rmdir /s /q "%EXTRACT_DIR%"

:: === DOWNLOAD ===
echo [*] Downloading repository ZIP...
curl -s -L -o "%ZIP_FILE%" "%ZIP_URL%"
if errorlevel 1 (
    echo [X] Download failed.
    exit /b 1
)

:: === EXTRACT ZIP ===
echo [*] Extracting ZIP...
powershell -NoProfile -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath '%EXTRACT_DIR%' -Force" >nul 2>&1

:: === REMOVE ZIP FILE ===
if exist "%ZIP_FILE%" del /f /q "%ZIP_FILE%"
if errorlevel 1 (
    echo [X] Failed to remove ZIP file.
    exit /b 1
)

if not exist "%EXTRACT_DIR%\%PROJECT_SUBDIR%" (
    echo [X] Extracted folder not found.
    exit /b 1
)

:: === SETUP MSVC ENVIRONMENT ===
echo [*] Setting up Visual Studio environment...
call "%VCVARS_PATH%" >nul 2>&1
if errorlevel 1 (
    echo [X] vcvars64.bat failed.
    exit /b 1
)

:: === BUILD PROJECT ===
cd /d "%EXTRACT_DIR%\%PROJECT_SUBDIR%"
echo [*] Building %PROJECT_FILE%...
MSBuild.exe "%PROJECT_FILE%" /t:Clean,Build /m /p:Configuration=%CONFIGURATION%;Platform=%PLATFORM% /verbosity:q >nul 2>&1

:: Go back to the original directory where the script was started
cd /d "%~dp0"
:: Copy the built executable to the current directory (where batch script runs)
if exist "%EXTRACT_DIR%\%PROJECT_SUBDIR%\%PLATFORM%\%CONFIGURATION%\protocol.exe" (
    copy /y "%EXTRACT_DIR%\%PROJECT_SUBDIR%\%PLATFORM%\%CONFIGURATION%\protocol.exe" "." >nul
) else (
    echo [X] Built executable not found!
    exit /b 1
)
:: === CLEANUP EXTRACTED DIRECTORY ===
rmdir /s /q "%EXTRACT_DIR%"
if errorlevel 1 (
    echo [X] Failed to remove extracted directory.
    exit /b 1
)
echo [*] Build completed successfully.
echo [*] Build path: %cd%\protocol.exe



endlocal
pause
