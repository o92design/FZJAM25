# Build Windows desktop binary using locally built raylib (MSVC)
# Prereq: run scripts/setup-deps.ps1 once

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$raylibDir = Join-Path $root 'third_party/raylib'
$buildDir = Join-Path $root 'bin'
$objDir = Join-Path $buildDir 'obj'

function Find-MSVC {
    # Check if cl is already available
    if (Get-Command cl -ErrorAction SilentlyContinue) {
        return $true
    }

    Write-Host 'MSVC not on PATH, searching for Visual Studio installation...' -ForegroundColor Yellow

    # Find vswhere (VS installer tool)
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-Not (Test-Path $vswhere)) {
        throw 'Visual Studio not found. Install VS Build Tools: winget install Microsoft.VisualStudio.2022.BuildTools'
    }

    # Find latest VS installation with C++ tools
    $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-Not $vsPath) {
        throw 'Visual Studio C++ tools not found. Install via VS Installer or: winget install Microsoft.VisualStudio.2022.BuildTools --override "--add Microsoft.VisualStudio.Workload.VCTools"'
    }

    # Load VS environment
    $vcvarsall = Join-Path $vsPath 'VC\Auxiliary\Build\vcvarsall.bat'
    if (-Not (Test-Path $vcvarsall)) {
        throw "vcvarsall.bat not found at: $vcvarsall"
    }

    Write-Host "Found Visual Studio at: $vsPath" -ForegroundColor Cyan
    Write-Host 'Activating MSVC environment (x64)...' -ForegroundColor Cyan

    # Run vcvarsall and capture environment variables
    $tempFile = [System.IO.Path]::GetTempFileName()
    cmd /c "`"$vcvarsall`" x64 && set" > $tempFile
    
    Get-Content $tempFile | ForEach-Object {
        if ($_ -match '^([^=]+)=(.*)$') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
    Remove-Item $tempFile

    # Verify cl is now available
    if (-Not (Get-Command cl -ErrorAction SilentlyContinue)) {
        throw 'Failed to activate MSVC environment'
    }

    Write-Host 'MSVC environment activated.' -ForegroundColor Green
    return $true
}

function Build-Raylib-Desktop {
    Write-Host 'Building raylib for desktop...' -ForegroundColor Yellow
    $rlBuildDir = Join-Path $raylibDir 'build_desktop'
    New-Item -ItemType Directory -Force -Path $rlBuildDir | Out-Null
    Push-Location $rlBuildDir
    cmake .. -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=OFF
    cmake --build . --config Debug
    Pop-Location
    
    # Check multiple possible locations for raylib.lib
    $possiblePaths = @(
        (Join-Path $raylibDir 'build_desktop/raylib/Debug/raylib.lib'),
        (Join-Path $raylibDir 'build_desktop/Debug/raylib.lib'),
        (Join-Path $raylibDir 'build_desktop/raylib/Release/raylib.lib'),
        (Join-Path $raylibDir 'build_desktop/raylib/raylib.lib'),
        (Join-Path $raylibDir 'build_desktop/Release/raylib.lib')
    )
    
    $libraylib = $possiblePaths | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-Not $libraylib) {
        throw "Failed to find raylib.lib in expected locations. Checked: $($possiblePaths -join ', ')"
    }
    
    Write-Host "Found raylib.lib at: $libraylib" -ForegroundColor Green
    return $libraylib
}

Find-MSVC

$libraylib = Join-Path $raylibDir 'build_desktop/raylib/Debug/raylib.lib'
if (-Not (Test-Path $libraylib)) {
    # Try alternate location
    $libraylib = Join-Path $raylibDir 'build_desktop/raylib/raylib.lib'
    if (-Not (Test-Path $libraylib)) {
        Build-Raylib-Desktop
        # Re-check for library after build
        $libraylib = Join-Path $raylibDir 'build_desktop/raylib/Debug/raylib.lib'
        if (-Not (Test-Path $libraylib)) {
            $libraylib = Join-Path $raylibDir 'build_desktop/raylib/raylib.lib'
        }
    }
}

$includeDir = Join-Path $raylibDir 'src'
$libDir = (Get-Item $libraylib).Directory.FullName

if (-Not (Test-Path $libraylib)) {
    throw "raylib.lib not found at: $libraylib"
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
New-Item -ItemType Directory -Force -Path $objDir | Out-Null

# Clean previous build artifacts to avoid stale conflicts
Write-Host 'Cleaning previous desktop build artifacts...' -ForegroundColor Yellow
Remove-Item -Force -ErrorAction SilentlyContinue (Join-Path $objDir '*.obj')
Remove-Item -Force -ErrorAction SilentlyContinue (Join-Path $buildDir 'fzjam25.exe')
Remove-Item -Force -ErrorAction SilentlyContinue (Join-Path $buildDir 'fzjam25.pdb')
Remove-Item -Force -ErrorAction SilentlyContinue (Join-Path $buildDir 'fzjam25.ilk')

Write-Host 'Compiling game with MSVC...' -ForegroundColor Cyan
Write-Host "Using raylib.lib from: $libDir" -ForegroundColor Cyan

$srcDir = Join-Path $root 'src'
$srcFiles = Get-ChildItem -Path $srcDir -Filter *.c | ForEach-Object { $_.FullName }

$clArgs = @(
    '/nologo'
    '/Zi'          # Generate debug info
    '/Od'          # Disable optimizations
    '/MDd'         # Use dynamic debug runtime to match raylib Debug build
    ("/Fo$objDir\\")  # Put object files in bin/obj
    '/I'
    $includeDir
    $srcFiles
    "/Fe:$(Join-Path $buildDir 'fzjam25.exe')"
    '/link'
    '/DEBUG'       # Link with debug info
    "/LIBPATH:$libDir"
    'raylib.lib'
    'winmm.lib'
    'gdi32.lib'
    'opengl32.lib'
    'user32.lib'
    'shell32.lib'
)

cl @clArgs

Write-Host 'Desktop build complete -> bin/fzjam25.exe' -ForegroundColor Green
