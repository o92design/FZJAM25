# Upload web build to itch.io using butler
# Prereq: Run this once to login: .\third_party\butler\butler.exe login

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$webDir = Join-Path $root 'web'
$butlerDir = Join-Path $root 'third_party/butler'
$butlerExe = Join-Path $butlerDir 'butler.exe'

# itch.io project details
$itchUser = 'kioskars'
$itchGame = 'fz-game-jam-25'
$channel = 'html5'  # Standard channel name for web games

function Ensure-Butler {
    if (Test-Path $butlerExe) { return $butlerExe }

    New-Item -ItemType Directory -Force -Path $butlerDir | Out-Null
    $zipUrl = 'https://broth.itch.zone/butler/windows-amd64/LATEST/archive/default'
    $zipPath = Join-Path $butlerDir 'butler.zip'
    
    Write-Host 'Downloading butler (itch.io CLI)...' -ForegroundColor Cyan
    Invoke-WebRequest -Uri $zipUrl -OutFile $zipPath
    Expand-Archive -Path $zipPath -DestinationPath $butlerDir -Force
    Remove-Item $zipPath -Force
    
    if (-Not (Test-Path $butlerExe)) { throw 'Failed to download butler.exe' }
    return $butlerExe
}

$butler = Ensure-Butler

if (-Not (Test-Path $webDir)) {
    throw 'web/ directory not found. Run scripts/build-web.ps1 first.'
}

$indexHtml = Join-Path $webDir 'index.html'
if (-Not (Test-Path $indexHtml)) {
    throw 'web/index.html not found. Run scripts/build-web.ps1 first.'
}

# Check if butler is authenticated
Write-Host 'Checking butler authentication...' -ForegroundColor Cyan

# Set API key from environment variable if provided
if ($env:BUTLER_API_KEY) {
    Write-Host 'Using API key from BUTLER_API_KEY environment variable.' -ForegroundColor Green
} else {
    # Check authentication status
    $loginCheck = & $butler login --validate 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host ''
        Write-Host '================================================' -ForegroundColor Yellow
        Write-Host 'Butler not authenticated. Please run:' -ForegroundColor Yellow
        Write-Host "  $butler login" -ForegroundColor Cyan
        Write-Host ''
        Write-Host 'Get your API key from: https://itch.io/user/settings/api-keys' -ForegroundColor Yellow
        Write-Host '================================================' -ForegroundColor Yellow
        throw 'Butler authentication required. Run the command above first.'
    }
}

Write-Host "Uploading web build to $itchUser/$itchGame..." -ForegroundColor Cyan
& $butler push $webDir "$itchUser/${itchGame}:$channel" --userversion-file (Join-Path $root 'VERSION.txt')

if ($LASTEXITCODE -eq 0) {
    Write-Host ''
    Write-Host 'Upload complete! View at: https://kioskars.itch.io/fz-game-jam-25' -ForegroundColor Green
} else {
    throw 'Butler upload failed. Check output above.'
}
