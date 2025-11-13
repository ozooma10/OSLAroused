@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Building OSLAroused and Running Tests
echo ========================================
echo.

REM Check if build type argument is provided, default to Debug
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Debug

REM Convert to lowercase for consistency
if /I "%BUILD_TYPE%"=="debug" (
    set BUILD_TYPE=Debug
    set BUILD_PRESET=build-debug-msvc
    set BUILD_DIR=build/debug-msvc
    set BUILD_CONFIG=debug-msvc
) else if /I "%BUILD_TYPE%"=="release" (
    set BUILD_TYPE=Release
    set BUILD_PRESET=build-release-msvc
    set BUILD_DIR=build/release-msvc
    set BUILD_CONFIG=release-msvc
) else (
    echo Invalid build type. Use "Debug" or "Release"
    echo Usage: %0 [Debug^|Release]
    exit /b 1
)

REM Check if VCPKG_ROOT is set
if not defined VCPKG_ROOT (
    echo ERROR: VCPKG_ROOT environment variable is not set!
    echo Please set VCPKG_ROOT to your vcpkg installation directory.
    exit /b 1
)

echo Building in %BUILD_TYPE% mode...
echo VCPKG_ROOT: %VCPKG_ROOT%
echo.

REM Clean the build directory if requested
if "%2"=="clean" (
    echo Cleaning existing build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    echo.
)

echo Step 1: Configuring CMake with tests enabled...
echo -----------------------------------------------
cmake --preset %BUILD_PRESET% -DBUILD_TESTS=ON
if !errorlevel! neq 0 (
    echo ERROR: CMake configuration failed!
    echo.
    echo Troubleshooting tips:
    echo 1. Ensure VCPKG_ROOT is set correctly: %VCPKG_ROOT%
    echo 2. Try running with clean option: %0 %BUILD_TYPE% clean
    echo 3. Check that Visual Studio 2022 is installed
    echo 4. Verify Ninja is in your PATH
    exit /b !errorlevel!
)

echo.
echo Step 2: Building the project...
echo -----------------------------------------------
cmake --build %BUILD_DIR% --preset %BUILD_CONFIG%
if !errorlevel! neq 0 (
    echo ERROR: Build failed!
    exit /b !errorlevel!
)

echo.
echo Step 3: Running tests...
echo -----------------------------------------------
ctest --preset tests-all --test-dir %BUILD_DIR%
if !errorlevel! neq 0 (
    echo.
    echo WARNING: Some tests failed. Check the output above for details.
    echo You can run specific test suites:
    echo   ctest --preset tests-unit       - Unit tests only
    echo   ctest --preset tests-integration - Integration tests only
    echo   ctest --preset tests-e2e         - End-to-end tests only
) else (
    echo.
    echo SUCCESS: All tests passed!
)

echo.
echo Build complete. Output is in: %BUILD_DIR%