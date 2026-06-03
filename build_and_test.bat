@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Building OSLAroused and Running Tests
echo ========================================
echo.

REM Build mode: "debug" or "release" (release maps to xmake's releasedbg). Default: debug.
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=debug

if /I "%BUILD_TYPE%"=="debug" (
    set XMAKE_MODE=debug
) else if /I "%BUILD_TYPE%"=="release" (
    set XMAKE_MODE=releasedbg
) else (
    echo Invalid build type. Use "debug" or "release"
    echo Usage: %0 [debug^|release] [clean]
    exit /b 1
)

where xmake >nul 2>nul
if !errorlevel! neq 0 (
    echo ERROR: xmake was not found on PATH. Install from https://xmake.io
    exit /b 1
)

if /I "%2"=="clean" (
    echo Cleaning previous build...
    xmake clean -a
    echo.
)

echo Step 1: Configuring xmake (mode=%XMAKE_MODE%, tests enabled)...
echo -----------------------------------------------
xmake f -m %XMAKE_MODE% --build_tests=y -y
if !errorlevel! neq 0 (
    echo ERROR: xmake configuration failed!
    exit /b !errorlevel!
)

echo.
echo Step 2: Building the project...
echo -----------------------------------------------
xmake build
if !errorlevel! neq 0 (
    echo ERROR: Build failed!
    exit /b !errorlevel!
)

echo.
echo Step 3: Running tests...
echo -----------------------------------------------
xmake run OSLArousedTests
if !errorlevel! neq 0 (
    echo.
    echo WARNING: Some tests failed. Check the output above for details.
    echo You can filter tests by tag, e.g.:
    echo   xmake run OSLArousedTests "[LRUCache]"
    echo   xmake run OSLArousedTests "~[integration]~[e2e]"   ^(unit tests only^)
) else (
    echo.
    echo SUCCESS: All tests passed!
)

echo.
echo Build complete. Output is in: build/windows/x64/%XMAKE_MODE%/
endlocal
