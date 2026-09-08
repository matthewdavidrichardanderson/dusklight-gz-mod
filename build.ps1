param(
 [string]$DuskDir = (Join-Path $PSScriptRoot '../dusklight-upstream'),
 [string]$BuildDir = (Join-Path $PSScriptRoot 'build/upstream-windows'),
 [string]$GameLibrary = '',
 [switch]$ConfigureOnly
)
$ErrorActionPreference = 'Stop'
$DuskDir = (Resolve-Path -LiteralPath $DuskDir).Path
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(-not $vs){throw 'Visual Studio C++ build tools are required.'}
Import-Module "$vs\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'
$cmake = "$vs/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
$ctest = "$vs/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe"
& $cmake -S $PSScriptRoot -B $BuildDir -G Ninja "-DDUSK_DIR=$DuskDir" "-DDUSK_GAME_EXE=$GameLibrary" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
if($ConfigureOnly){exit 0}
& $cmake --build $BuildDir
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
& $ctest --test-dir $BuildDir --output-on-failure
exit $LASTEXITCODE
