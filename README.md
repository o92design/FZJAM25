# FZJAM25 - Raylib 2D Platformer

Minimal starter for a 2D platformer built with C and raylib. Targets both desktop and web (Emscripten). Controls: keyboard (A/D or arrow keys) and Xbox 360 controller.

## Prerequisites (Windows)
- Git, CMake, and a C compiler (MSVC via Visual Studio Build Tools recommended)
- PowerShell 5+ (built-in)
- Internet access for the first-time dependency download
Notes: No system Python required; emsdk.bat provides its own. Web build script will auto-fetch Ninja into `third_party/tools` if it is not already on PATH.

## One-time dependency setup
From the repo root run:
```powershell
pwsh ./scripts/setup-deps.ps1
```
What it does:
- Clones and activates the Emscripten SDK into `third_party/emsdk` (installs latest)
- Clones raylib source into `third_party/raylib`
- Builds raylib for Web (`build_web`) and Desktop (`build_desktop`)

## Build (web / wasm)
```powershell
pwsh ./scripts/build-web.ps1
```
Outputs `web/index.html`, `web/index.js`, `web/index.wasm`. Serve `web/` with any static server (e.g., `python -m http.server 8080`).

## Build (desktop / Windows)
```powershell
pwsh ./scripts/build-desktop.ps1
```
Outputs `bin/fzjam25.exe` using the locally built raylib.

## Upload to itch.io
First-time setup (authenticate butler):
```powershell
.\third_party\butler\butler.exe login
```
Get your API key from https://itch.io/user/settings/api-keys

Then upload after each web build:
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\upload-itch.ps1
```
Uploads to https://kioskars.itch.io/fz-game-jam-25 (increment `VERSION.txt` for new releases).

## Project layout
- `src/` — game code
- `assets/` — art/audio (empty for now)
- `bin/` — native builds
- `web/` — web build outputs
- `third_party/` — downloaded deps (emsdk + raylib + build trees)
- `scripts/` — helper scripts for setup and builds

## Notes
- Game loop currently shows a placeholder platform and allows left/right movement with simple ground collision.
- Controller input uses the first gamepad (`0`).
- Tweak constants at the top of `src/main.c` for tuning movement.
