@echo off
setlocal

echo ========================================
echo OSLAroused Build Script with Tests
echo ========================================

REM Check for vcpkg
if "%VCPKG_ROOT%"=="" (
    echo ERROR: VCPKG_ROOT environment variable is not set!
    echo Please install vcpkg and set VCPKG_ROOT to its installation directory.
    echo.
    echo To install vcpkg:
    echo   1. git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    echo   2. C:\vcpkg\bootstrap-vcpkg.bat
    echo   3. setx VCPKG_ROOT "C:\vcpkg"
    echo.
    exit /b 1
)

REM Check for ninja
where ninja >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Ninja build tool not found!
    echo Please install ninja and add it to PATH.
    echo.
    echo To install ninja:
    echo   1. Download from: https://github.com/ninja-build/ninja/releases
    echo   2. Extract ninja.exe to a directory in your PATH
    echo.
    echo Alternatively, install via vcpkg: vcpkg install ninja
    echo.
    exit /b 1
)

REM Setup Visual Studio environment
echo Setting up Visual Studio environment...

REM Try to use vswhere to find any VS installation with C++ tools
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    echo Using vswhere to locate Visual Studio...
    for /f "usebackq delims=" %%i in (`"%VSWHERE%" -products * -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set VSINSTALLPATH=%%i
    )
    if defined VSINSTALLPATH (
        if exist "%VSINSTALLPATH%\VC\Auxiliary\Build\vcvars64.bat" (
            echo Found Visual Studio at: %VSINSTALLPATH%
            call "%VSINSTALLPATH%\VC\Auxiliary\Build\vcvars64.bat"
            goto :build
        )
    )
)

REM Fallback: Try common VS installation paths (2026, 2022, 2019)
echo Searching for Visual Studio in common paths...

REM VS 2026
set VS2026PREVIEW=C:\Program Files\Microsoft Visual Studio\2026\Preview
if exist "%VS2026PREVIEW%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2026 Preview
    call "%VS2026PREVIEW%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2026COM=C:\Program Files\Microsoft Visual Studio\2026\Community
if exist "%VS2026COM%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2026 Community
    call "%VS2026COM%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2026PRO=C:\Program Files\Microsoft Visual Studio\2026\Professional
if exist "%VS2026PRO%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2026 Professional
    call "%VS2026PRO%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2026ENT=C:\Program Files\Microsoft Visual Studio\2026\Enterprise
if exist "%VS2026ENT%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2026 Enterprise
    call "%VS2026ENT%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

REM VS 2022
set VS2022COM=C:\Program Files\Microsoft Visual Studio\2022\Community
if exist "%VS2022COM%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2022 Community
    call "%VS2022COM%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2022PRO=C:\Program Files\Microsoft Visual Studio\2022\Professional
if exist "%VS2022PRO%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2022 Professional
    call "%VS2022PRO%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2022ENT=C:\Program Files\Microsoft Visual Studio\2022\Enterprise
if exist "%VS2022ENT%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2022 Enterprise
    call "%VS2022ENT%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

REM VS 2019
set VS2019COM=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
if exist "%VS2019COM%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2019 Community
    call "%VS2019COM%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2019PRO=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional
if exist "%VS2019PRO%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2019 Professional
    call "%VS2019PRO%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

set VS2019ENT=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise
if exist "%VS2019ENT%\VC\Auxiliary\Build\vcvars64.bat" (
    echo Found VS 2019 Enterprise
    call "%VS2019ENT%\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

echo ERROR: Visual Studio not found!
echo Please install Visual Studio 2019, 2022, or 2026 with C++ development workload.
exit /b 1

:build
echo.
echo Compiler: %CXX%
echo Windows SDK: %WindowsSDKVersion%
echo.
echo Configuring CMake (Debug with Tests)...
cmake --preset build-debug-msvc -DBUILD_TESTS=ON
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    exit /b 1
)

echo.
echo Building project...
cmake --build build/debug-msvc --preset debug-msvc
if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo To run tests:
echo   ctest --preset tests-unit
echo   - or -
echo   build\debug-msvc\OSLArousedTests.exe
echo.

endlocal