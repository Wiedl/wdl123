@echo off
setlocal
cd /d "%~dp0"

echo ========================================
echo   DDNet / TClient - Windows build
echo ========================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    echo Install CMake and reopen this window.
    goto :fail
)

if not exist ".git" (
    echo ERROR: Run this script from a Git repository checkout.
    goto :fail
)

echo [1/3] Initializing Git submodules...
git submodule update --init --recursive
if errorlevel 1 goto :fail

echo.
echo [2/3] Configuring the Debug x64 build...
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Debug -DDEV=ON -DDOWNLOAD_GTEST=ON -DEXCEPTION_HANDLING=ON -DVULKAN=ON -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=.
if errorlevel 1 goto :fail

echo.
echo [3/3] Building DDNet and related targets...
cmake --build build --config Debug --target everything --parallel
if errorlevel 1 goto :fail

echo.
echo ========================================
echo Build completed successfully.
echo Look for DDNet.exe in the build folder.
echo ========================================
pause
exit /b 0

:fail
echo.
echo BUILD FAILED. Check the error messages above.
pause
exit /b 1
