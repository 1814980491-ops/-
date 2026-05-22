param(
    [string]$BuildDir = "build-win32"
)

$ErrorActionPreference = "Stop"

$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$EngineDir = Join-Path $Root "vendor\cocos2d-x-3.17.2"
$OutDir = Join-Path $Root $BuildDir
$VsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path (Join-Path $EngineDir "cocos"))) {
    throw "cocos2d-x was not found. Run tools/setup_cocos2dx_3_17.ps1 first."
}

$CMakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
$CMakeExe = if ($CMakeCommand) { $CMakeCommand.Source } else { $null }

if (-not $CMakeExe -and (Test-Path $VsWhere)) {
    $VsInstall = & $VsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($VsInstall) {
        $VsCMake = Join-Path $VsInstall "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        if (Test-Path $VsCMake) {
            $CMakeExe = $VsCMake
        }
    }
}

if (-not $CMakeExe) {
    throw "cmake was not found."
}

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

& $CMakeExe -S $Root -B $OutDir -G "Visual Studio 17 2022" -A Win32 "-DCOCOS2DX_ROOT=$EngineDir"
if ($LASTEXITCODE -ne 0) {
    throw "cmake configure failed with exit code $LASTEXITCODE"
}

& $CMakeExe --build $OutDir --config Debug
if ($LASTEXITCODE -ne 0) {
    throw "cmake build failed with exit code $LASTEXITCODE"
}
