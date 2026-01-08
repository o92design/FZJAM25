# Build WebAssembly target using emsdk + raylib
# Prereq: run scripts/setup-deps.ps1 once

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$emsdkDir = Join-Path $root 'third_party/emsdk'
$raylibDir = Join-Path $root 'third_party/raylib'
$buildDir = Join-Path $root 'web'
$toolsDir = Join-Path $root 'third_party/tools'

function Ensure-Ninja {
    $ninja = Get-Command ninja -ErrorAction SilentlyContinue
    if ($ninja) { return $ninja.Path }

    New-Item -ItemType Directory -Force -Path $toolsDir | Out-Null
    $localNinja = Join-Path $toolsDir 'ninja.exe'
    if (Test-Path $localNinja) { return $localNinja }

    $zipUrl = 'https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip'
    $zipPath = Join-Path $toolsDir 'ninja-win.zip'
    Write-Host "Downloading Ninja (1.12.1)..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri $zipUrl -OutFile $zipPath
    Expand-Archive -Path $zipPath -DestinationPath $toolsDir -Force
    Remove-Item $zipPath -Force
    if (-Not (Test-Path $localNinja)) { throw 'Failed to fetch ninja.exe' }
    return $localNinja
}

function Build-Raylib-Web {
    Write-Host 'Building raylib for web (emcmake cmake)...' -ForegroundColor Yellow
    $rlBuildDir = Join-Path $raylibDir 'build_web'
    New-Item -ItemType Directory -Force -Path $rlBuildDir | Out-Null
    Push-Location $rlBuildDir
    $ninjaPath = Ensure-Ninja
    Write-Host "Using Ninja: $ninjaPath" -ForegroundColor Cyan
    $env:PATH = "$($ninjaPath | Split-Path);$env:PATH"
    emcmake cmake .. -G "Ninja" -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF
    cmake --build . --config Release
    Pop-Location
    $libraylib = Join-Path $raylibDir 'build_web/raylib/libraylib.a'
    if (-Not (Test-Path $libraylib)) {
        throw 'Failed to build raylib for web; check cmake output above.'
    }
}

if (-Not (Test-Path (Join-Path $emsdkDir 'emsdk_env.ps1'))) {
    throw 'emsdk not found. Run scripts/setup-deps.ps1 first.'
}

& "$emsdkDir/emsdk_env.ps1" | Out-Null

$libraylib = Join-Path $raylibDir 'build_web/raylib/libraylib.a'
if (-Not (Test-Path $libraylib)) {
    Build-Raylib-Web
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

Write-Host "Compiling game with emcc..." -ForegroundColor Cyan
$srcFile = Join-Path $root 'src/main.c'
$outFile = Join-Path $buildDir 'index.html'
$includeDir = Join-Path $raylibDir 'src'
$libDir = Join-Path $raylibDir 'build_web/raylib'

& emcc $srcFile -o $outFile -O2 -I $includeDir -L $libDir -lraylib `
    -s USE_GLFW=3 `
    -s ASYNCIFY `
    -s FORCE_FILESYSTEM=1 `
    -s TOTAL_MEMORY=128MB `
    -s ALLOW_MEMORY_GROWTH=1 `
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'

# Auto-increment version
$versionFile = Join-Path $root 'VERSION.txt'
if (Test-Path $versionFile) {
    $version = Get-Content $versionFile -Raw
    $version = $version.Trim()
    if ($version -match '^(\d+)\.(\d+)\.(\d+)$') {
        $major = [int]$matches[1]
        $minor = [int]$matches[2]
        $patch = [int]$matches[3] + 1
        $newVersion = "$major.$minor.$patch"
        Set-Content -Path $versionFile -Value $newVersion -NoNewline
        Write-Host "Version bumped: $version -> $newVersion" -ForegroundColor Cyan
    }
}

Write-Host 'Web build complete -> web/index.html (serve web/ with a static file server).' -ForegroundColor Green
