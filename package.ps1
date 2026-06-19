# Packages contrib/Distribution into a versioned FOMOD archive for distribution.
#
# This does NOT build. Run the C++ and Papyrus builds first so the artifacts in
# contrib/Distribution/ are current:
#   xmake f -m releasedbg; xmake build OSLAroused        (refreshes PluginRelease/OSLAroused.dll)
#   PapyrusCompiler with Release.ppj                     (refreshes PapyrusRelease/*.pex)
#
# Usage:
#   .\package.ps1                 # builds OSLAroused-<version>.zip in dist/
#   .\package.ps1 -OutDir out     # custom output directory
#   .\package.ps1 -IncludePdb     # keep OSLAroused.pdb in the archive (default: excluded)

param(
    [string]$OutDir = "dist",
    [switch]$IncludePdb
)

$ErrorActionPreference = "Stop"
$root = $PSScriptRoot
$distSource = Join-Path $root "contrib\Distribution"

if (-not (Test-Path $distSource)) {
    throw "Distribution folder not found: $distSource"
}

# Read the version from xmake.lua's set_version("x.y.z") so the archive name matches the build.
$xmake = Get-Content (Join-Path $root "xmake.lua") -Raw
$match = [regex]::Match($xmake, 'set_version\("([^"]+)"\)')
if (-not $match.Success) {
    throw "Could not find set_version(...) in xmake.lua"
}
$version = $match.Groups[1].Value
Write-Host "Packaging OSL Aroused version $version" -ForegroundColor Cyan

# Sanity check: warn if compiled artifacts are older than their sources (likely a stale build).
$dll = Join-Path $distSource "PluginRelease\OSLAroused.dll"
if (-not (Test-Path $dll)) {
    throw "Missing $dll - build the C++ plugin (Release) first."
}
$staleWarnings = @()
Get-ChildItem (Join-Path $distSource "PapyrusSources") -Filter *.psc | ForEach-Object {
    $pex = Join-Path $distSource "PapyrusRelease\$($_.BaseName).pex"
    if ((Test-Path $pex) -and ($_.LastWriteTime -gt (Get-Item $pex).LastWriteTime)) {
        $staleWarnings += "  $($_.Name) is newer than its compiled .pex - recompile Papyrus."
    }
}
if ($staleWarnings.Count -gt 0) {
    Write-Host "WARNING: stale compiled scripts detected:" -ForegroundColor Yellow
    $staleWarnings | ForEach-Object { Write-Host $_ -ForegroundColor Yellow }
    Write-Host ""
}

# Stage the distribution contents into a temp folder so we can exclude files without
# touching the working tree, then zip from there (fomod/ ends up at the archive root).
$staging = Join-Path ([System.IO.Path]::GetTempPath()) "OSLAroused-pkg-$([guid]::NewGuid())"
New-Item -ItemType Directory -Path $staging | Out-Null
try {
    Copy-Item -Path (Join-Path $distSource "*") -Destination $staging -Recurse -Force

    # Batch/script files are dev tooling, never runtime assets, and ship-time .bat files
    # trip users' antivirus. Strip them unconditionally so none can leak into the archive.
    Get-ChildItem -Path $staging -Include *.bat, *.cmd -Recurse | ForEach-Object {
        Write-Host "Excluding $($_.Name)" -ForegroundColor DarkGray
        Remove-Item $_.FullName -Force
    }

    if (-not $IncludePdb) {
        Get-ChildItem -Path $staging -Filter *.pdb -Recurse | ForEach-Object {
            Write-Host "Excluding $($_.Name)" -ForegroundColor DarkGray
            Remove-Item $_.FullName -Force
        }
    }

    $outDirFull = Join-Path $root $OutDir
    New-Item -ItemType Directory -Path $outDirFull -Force | Out-Null
    $archive = Join-Path $outDirFull "OSLAroused-$version.zip"
    if (Test-Path $archive) { Remove-Item $archive -Force }

    Compress-Archive -Path (Join-Path $staging "*") -DestinationPath $archive -CompressionLevel Optimal
    $sizeMB = [math]::Round((Get-Item $archive).Length / 1MB, 2)
    Write-Host "Created $archive ($sizeMB MB)" -ForegroundColor Green
}
finally {
    Remove-Item $staging -Recurse -Force -ErrorAction SilentlyContinue
}
