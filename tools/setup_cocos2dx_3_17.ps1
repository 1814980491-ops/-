$ErrorActionPreference = "Stop"

$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$VendorDir = Join-Path $Root "vendor"
$EngineDir = Join-Path $VendorDir "cocos2d-x-3.17.2"
$EngineZip = Join-Path $VendorDir "cocos2d-x-3.17.2.zip"
$EngineExtract = Join-Path $VendorDir "cocos-engine-extract"
$VsDepsZip = Join-Path $VendorDir "v3-deps-158.zip"
$DepsExtract = Join-Path $VendorDir "cocos-deps-extract"

function Invoke-Git {
    param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Arguments)
    git @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Arguments -join ' ') failed with exit code $LASTEXITCODE"
    }
}

New-Item -ItemType Directory -Force -Path $VendorDir | Out-Null

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "git was not found."
}

if (-not (Test-Path (Join-Path $EngineDir "cocos"))) {
    try {
        Invoke-Git clone --depth 1 --branch cocos2d-x-3.17.2 https://github.com/cocos2d/cocos2d-x.git $EngineDir
    } catch {
        Write-Host "git clone failed, falling back to source zip."
        if (Test-Path $EngineDir) {
            Remove-Item -LiteralPath $EngineDir -Recurse -Force
        }
        if (Test-Path $EngineExtract) {
            Remove-Item -LiteralPath $EngineExtract -Recurse -Force
        }
        if (-not (Test-Path $EngineZip)) {
            Invoke-WebRequest -Uri "https://github.com/cocos2d/cocos2d-x/archive/refs/tags/cocos2d-x-3.17.2.zip" -OutFile $EngineZip
        }
        New-Item -ItemType Directory -Force -Path $EngineExtract | Out-Null
        Expand-Archive -LiteralPath $EngineZip -DestinationPath $EngineExtract -Force
        $extractedRoot = Join-Path $EngineExtract "cocos2d-x-cocos2d-x-3.17.2"
        Move-Item -LiteralPath $extractedRoot -Destination $EngineDir
    }
}

if (Test-Path (Join-Path $EngineDir ".git")) {
    Push-Location $EngineDir
    try {
        Invoke-Git config url.https://github.com/.insteadOf git://github.com/
        Invoke-Git config submodule.tools/cocos2d-console.url https://github.com/cocos2d/cocos2d-console.git
        Invoke-Git submodule update --init --depth 1 tools/cocos2d-console
    } finally {
        Pop-Location
    }
}

if (-not (Test-Path (Join-Path $EngineDir "external\CMakeLists.txt"))) {
    $url = "https://github.com/cocos2d/cocos2d-x-3rd-party-libs-bin/archive/v3-deps-158.zip"
    if (-not (Test-Path $VsDepsZip)) {
        Invoke-WebRequest -Uri $url -OutFile $VsDepsZip
    }

    if (Test-Path $DepsExtract) {
        Remove-Item -LiteralPath $DepsExtract -Recurse -Force
    }
    New-Item -ItemType Directory -Force -Path $DepsExtract | Out-Null
    Expand-Archive -LiteralPath $VsDepsZip -DestinationPath $DepsExtract -Force

    $src = Join-Path $DepsExtract "cocos2d-x-3rd-party-libs-bin-3-deps-158"
    Copy-Item -Path (Join-Path $src "*") -Destination (Join-Path $EngineDir "external") -Recurse -Force

    $fbx = Join-Path $EngineDir "external\fbx-conv"
    if (Test-Path $fbx) {
        $dst = Join-Path $EngineDir "tools\fbx-conv"
        if (Test-Path $dst) {
            Remove-Item -LiteralPath $dst -Recurse -Force
        }
        Move-Item -LiteralPath $fbx -Destination $dst
    }
}

Write-Host "cocos2d-x environment is ready: $EngineDir"
