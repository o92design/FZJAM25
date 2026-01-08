# Setup raylib + emsdk into third_party
# Requires: git, cmake, PowerShell 5+, internet access

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$thirdParty = Join-Path $root 'third_party'
$emsdkDir = Join-Path $thirdParty 'emsdk'
$raylibDir = Join-Path $thirdParty 'raylib'

New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null

function Ensure-Emsdk {
    if (-Not (Test-Path $emsdkDir)) {
        Write-Host 'Cloning emsdk...' -ForegroundColor Cyan
        git clone https://github.com/emscripten-core/emsdk.git $emsdkDir
    } else {
        Write-Host 'emsdk already present; pulling updates...' -ForegroundColor Yellow
        Push-Location $emsdkDir; git pull --ff-only; Pop-Location
    }

    $emsdkBat = Join-Path $emsdkDir 'emsdk.bat'
    if (-Not (Test-Path $emsdkBat)) { throw 'emsdk.bat missing after clone.' }

    Push-Location $emsdkDir
    Write-Host 'Installing latest emsdk (bundled Python included)...' -ForegroundColor Cyan
    cmd /c "`"$emsdkBat`" install latest"
    Write-Host 'Activating latest emsdk...' -ForegroundColor Cyan
    cmd /c "`"$emsdkBat`" activate latest"
    Pop-Location

    Write-Host 'emsdk ready. Remember: `& "$emsdkDir/emsdk_env.ps1"` before building.' -ForegroundColor Green
}

function Ensure-Raylib {
    if (-Not (Test-Path (Join-Path $raylibDir '.git'))) {
        Write-Host 'Cloning raylib (v5.0)...' -ForegroundColor Cyan
        git clone --branch 5.0 --depth 1 https://github.com/raysan5/raylib.git $raylibDir
    } else {
        Write-Host 'raylib already present; skipping clone.' -ForegroundColor Yellow
    }
}

function Build-Raylib-Web {
    Write-Host 'Building raylib for Web (Emscripten)...' -ForegroundColor Cyan
    & "$emsdkDir/emsdk_env.ps1" | Out-Null
    $buildDir = Join-Path $raylibDir 'build_web'
    New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
    Push-Location $buildDir
    emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    Pop-Location
}

function Build-Raylib-Desktop {
    Write-Host 'Building raylib for Desktop (Windows)...' -ForegroundColor Cyan
    $buildDir = Join-Path $raylibDir 'build_desktop'
    New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
    Push-Location $buildDir
    cmake .. -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    Pop-Location
}

Ensure-Emsdk
Ensure-Raylib
Build-Raylib-Web
Build-Raylib-Desktop

Write-Host 'Done. Use build-web.ps1 or build-desktop.ps1 next.' -ForegroundColor Green
