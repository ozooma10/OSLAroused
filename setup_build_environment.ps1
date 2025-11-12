# OSLAroused Build Environment Setup Script
# This script helps set up the required build tools

param(
    [string]$VcpkgPath = "C:\vcpkg",
    [switch]$Force
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "OSLAroused Build Environment Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Host "WARNING: Not running as administrator. Some operations may fail." -ForegroundColor Yellow
    Write-Host "Consider running this script as administrator for best results." -ForegroundColor Yellow
    Write-Host
}

# Function to test if a command exists
function Test-Command {
    param($Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

# 1. Check/Install vcpkg
Write-Host "Checking vcpkg..." -ForegroundColor Green
if ($env:VCPKG_ROOT -and (Test-Path "$env:VCPKG_ROOT\vcpkg.exe") -and -not $Force) {
    Write-Host "✓ vcpkg found at: $env:VCPKG_ROOT" -ForegroundColor Green
} else {
    if (Test-Path $VcpkgPath -and -not $Force) {
        Write-Host "vcpkg directory exists at $VcpkgPath" -ForegroundColor Yellow
        $response = Read-Host "Use existing installation? (Y/N)"
        if ($response -eq 'Y') {
            [Environment]::SetEnvironmentVariable("VCPKG_ROOT", $VcpkgPath, [EnvironmentVariableTarget]::User)
            $env:VCPKG_ROOT = $VcpkgPath
            Write-Host "✓ VCPKG_ROOT set to: $VcpkgPath" -ForegroundColor Green
        }
    } else {
        Write-Host "vcpkg not found. Installing..." -ForegroundColor Yellow

        if (Test-Command git) {
            Write-Host "Cloning vcpkg repository..." -ForegroundColor Cyan
            git clone https://github.com/Microsoft/vcpkg.git $VcpkgPath

            Write-Host "Bootstrapping vcpkg..." -ForegroundColor Cyan
            & "$VcpkgPath\bootstrap-vcpkg.bat"

            # Set environment variable
            [Environment]::SetEnvironmentVariable("VCPKG_ROOT", $VcpkgPath, [EnvironmentVariableTarget]::User)
            $env:VCPKG_ROOT = $VcpkgPath

            Write-Host "✓ vcpkg installed at: $VcpkgPath" -ForegroundColor Green
            Write-Host "✓ VCPKG_ROOT environment variable set" -ForegroundColor Green
        } else {
            Write-Host "ERROR: Git is not installed. Please install Git first." -ForegroundColor Red
            exit 1
        }
    }
}

# 2. Check/Install Ninja
Write-Host
Write-Host "Checking Ninja build tool..." -ForegroundColor Green
if (Test-Command ninja) {
    $ninjaVersion = & ninja --version
    Write-Host "✓ Ninja found (version $ninjaVersion)" -ForegroundColor Green
} else {
    Write-Host "Ninja not found. Installing..." -ForegroundColor Yellow

    if ($env:VCPKG_ROOT) {
        Write-Host "Installing Ninja via vcpkg..." -ForegroundColor Cyan
        & "$env:VCPKG_ROOT\vcpkg.exe" install ninja

        # Add vcpkg tools to PATH
        $vcpkgToolsPath = "$env:VCPKG_ROOT\installed\x64-windows\tools"
        if (Test-Path $vcpkgToolsPath) {
            $currentPath = [Environment]::GetEnvironmentVariable("Path", [EnvironmentVariableTarget]::User)
            if ($currentPath -notlike "*$vcpkgToolsPath*") {
                [Environment]::SetEnvironmentVariable("Path", "$currentPath;$vcpkgToolsPath", [EnvironmentVariableTarget]::User)
                $env:Path += ";$vcpkgToolsPath"
                Write-Host "✓ Added vcpkg tools to PATH" -ForegroundColor Green
            }
        }
    } else {
        Write-Host "Please download Ninja manually from: https://github.com/ninja-build/ninja/releases" -ForegroundColor Yellow
        Write-Host "Extract ninja.exe to a directory in your PATH" -ForegroundColor Yellow
    }
}

# 3. Check Visual Studio
Write-Host
Write-Host "Checking Visual Studio installation..." -ForegroundColor Green

$vsFound = $false
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    # Query for any VS installation with C++ workload (2019, 2022, 2026, etc.)
    $vsInstalls = & $vsWhere -products * -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsInstalls) {
        $vsVersion = & $vsWhere -products * -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property catalog_productDisplayVersion
        Write-Host "✓ Visual Studio found at: $vsInstalls" -ForegroundColor Green
        Write-Host "  Version: $vsVersion" -ForegroundColor Cyan
        $vsFound = $true
    } else {
        Write-Host "ERROR: Visual Studio not found!" -ForegroundColor Red
        Write-Host "Please install Visual Studio (2019, 2022, or 2026) with C++ development workload" -ForegroundColor Yellow
    }
} else {
    # Check common paths for various VS versions
    $vsPaths = @(
        "C:\Program Files\Microsoft Visual Studio\2026\Community",
        "C:\Program Files\Microsoft Visual Studio\2026\Professional",
        "C:\Program Files\Microsoft Visual Studio\2026\Enterprise",
        "C:\Program Files\Microsoft Visual Studio\2026\Preview",
        "C:\Program Files\Microsoft Visual Studio\2022\Community",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
        "C:\Program Files\Microsoft Visual Studio\2019\Community",
        "C:\Program Files\Microsoft Visual Studio\2019\Professional",
        "C:\Program Files\Microsoft Visual Studio\2019\Enterprise"
    )

    foreach ($path in $vsPaths) {
        if (Test-Path "$path\VC\Auxiliary\Build\vcvars64.bat") {
            Write-Host "✓ Visual Studio found at: $path" -ForegroundColor Green
            $vsFound = $true
            break
        }
    }

    if (-not $vsFound) {
        Write-Host "ERROR: Visual Studio not found!" -ForegroundColor Red
        Write-Host "Please install Visual Studio (2019, 2022, or 2026) with C++ development workload" -ForegroundColor Yellow
    }
}

# 4. Check CMake
Write-Host
Write-Host "Checking CMake..." -ForegroundColor Green
if (Test-Command cmake) {
    $cmakeVersion = & cmake --version | Select-String -Pattern "cmake version (\d+\.\d+\.\d+)"
    Write-Host "✓ CMake found: $cmakeVersion" -ForegroundColor Green
} else {
    Write-Host "ERROR: CMake not found!" -ForegroundColor Red
    Write-Host "Please install CMake from: https://cmake.org/download/" -ForegroundColor Yellow
}

# 5. Summary
Write-Host
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Setup Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$ready = $true

if ($env:VCPKG_ROOT -and (Test-Path "$env:VCPKG_ROOT\vcpkg.exe")) {
    Write-Host "✓ vcpkg: Ready" -ForegroundColor Green
} else {
    Write-Host "✗ vcpkg: Not configured" -ForegroundColor Red
    $ready = $false
}

if (Test-Command ninja) {
    Write-Host "✓ Ninja: Ready" -ForegroundColor Green
} else {
    Write-Host "✗ Ninja: Not found" -ForegroundColor Red
    $ready = $false
}

if (Test-Command cmake) {
    Write-Host "✓ CMake: Ready" -ForegroundColor Green
} else {
    Write-Host "✗ CMake: Not found" -ForegroundColor Red
    $ready = $false
}

if ($vsFound) {
    Write-Host "✓ Visual Studio: Ready" -ForegroundColor Green
} else {
    Write-Host "✗ Visual Studio: Not found" -ForegroundColor Red
    $ready = $false
}

Write-Host

if ($ready) {
    Write-Host "Environment is ready! You can now build the project:" -ForegroundColor Green
    Write-Host
    Write-Host "  .\build_with_tests.bat" -ForegroundColor Cyan
    Write-Host
    Write-Host "Or manually:" -ForegroundColor White
    Write-Host "  cmake --preset build-debug-msvc -DBUILD_TESTS=ON" -ForegroundColor Cyan
    Write-Host "  cmake --build build/debug-msvc --preset debug-msvc" -ForegroundColor Cyan
} else {
    Write-Host "Some components are missing. Please install them and run this script again." -ForegroundColor Yellow
    Write-Host
    Write-Host "You may need to restart your terminal after installation for PATH changes to take effect." -ForegroundColor Yellow
}

Write-Host
Write-Host "Note: If you just installed components, you may need to:" -ForegroundColor Yellow
Write-Host "  1. Close and reopen your terminal" -ForegroundColor Yellow
Write-Host "  2. Or run: refreshenv (if using Chocolatey)" -ForegroundColor Yellow