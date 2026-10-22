param(
    [string]$UppSource = 'E:/upp-18468/uppsrc',
    [string]$UiSource = 'E:/apps/github/upp_Ui',
    [string]$AnimationSource = 'E:/apps/github/upp_animation'
)

$ErrorActionPreference = 'Stop'
$repoPath = Split-Path -Parent $PSScriptRoot
$versionHeader = Get-Content -LiteralPath (Join-Path $repoPath 'DirLister/AppInfo.h') -Raw
if ($versionHeader -notmatch 'DIRLISTER_VERSION\s*=\s*"([0-9]+\.[0-9]+\.[0-9]+)"') {
    throw 'Cannot read the application version.'
}
$version = $Matches[1]
$name = "DirLister-$version-windows-x64"
$releaseRoot = Join-Path $repoPath 'bin/release'
$packagePath = Join-Path $releaseRoot $name
$zipPath = Join-Path $releaseRoot "$name.zip"
if ((Test-Path -LiteralPath $packagePath) -or (Test-Path -LiteralPath $zipPath)) {
    throw "Release $name already exists. Move the previous package aside before repackaging."
}
$executable = Join-Path $repoPath 'bin/windows-x64/DirLister.exe'
if (!(Test-Path -LiteralPath $executable)) { throw 'Build the Release executable first.' }

# Validate inputs before creating the release directory.
$notices = [ordered]@{}
foreach ($package in @('Core', 'Draw', 'CtrlCore', 'CtrlLib', 'RichText', 'Painter', 'plugin/png', 'plugin/z', 'plugin/bmp')) {
    $notices[('Upp-' + $package.Replace('/', '-') + '.txt')] = Join-Path $UppSource "$package/Copying"
}
$notices['Ui-LICENSE.txt'] = Join-Path $UiSource 'LICENSE'
$notices['Animation-LICENSE.txt'] = Join-Path $AnimationSource 'LICENSE'
foreach ($path in $notices.Values) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing dependency notice: $path" }
}
$helpSource = Get-Content -LiteralPath (Join-Path $repoPath 'DirLister/AppInfo.cpp') -Raw
if ($helpSource -notmatch '(?s)R"HELP\((.*?)\)HELP"') { throw 'Cannot extract the application help guide.' }
$guide = "DirLister $version" + $Matches[1]

New-Item -ItemType Directory -Path (Join-Path $packagePath 'licenses') -Force | Out-Null
Copy-Item -LiteralPath $executable -Destination (Join-Path $packagePath 'DirLister.exe')
foreach ($file in @('LICENSE', 'CHANGELOG.md')) {
    Copy-Item -LiteralPath (Join-Path $repoPath $file) -Destination $packagePath
}
foreach ($notice in $notices.GetEnumerator()) {
    Copy-Item -LiteralPath $notice.Value -Destination (Join-Path $packagePath "licenses/$($notice.Key)")
}
# Include any adjacent runtime DLLs if a future build uses them.
Get-ChildItem -LiteralPath (Split-Path -Parent $executable) -Filter '*.dll' -File |
    Copy-Item -Destination $packagePath
$guide | Set-Content -LiteralPath (Join-Path $packagePath 'USER-GUIDE.txt') -Encoding UTF8
@"
DirLister $version - Windows x64

Extract the whole ZIP to a folder, then run DirLister.exe. No installer is needed.

Choose a source folder and set Scan depth limit below Location History.
To filter: choose a process, enter its parameters, and click Add. Enable must be
checked; the Filter badge shows ON when saved rules are active. Apply at level 0
applies the rule at every scanned level. Generate List previews the selection.

The top-right icons switch Dark / Olive Light, open Help, and exit.
See USER-GUIDE.txt or the built-in Help for filter and rename recipes.
LICENSE covers DirLister; licenses/ contains dependency notices.

Source and updates: https://github.com/Trilec/upp_directory_lister
"@ | Set-Content -LiteralPath (Join-Path $packagePath 'README.txt') -Encoding UTF8

Compress-Archive -LiteralPath $packagePath -DestinationPath $zipPath -CompressionLevel Optimal
$hash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $name.zip" | Set-Content -LiteralPath "$zipPath.sha256" -Encoding ASCII
Write-Output "Release ZIP: $zipPath"
Write-Output "SHA256: $zipPath.sha256"
