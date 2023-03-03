# Configuration
$InstallLocation = "C:\Program Files\Syntad"
$InstallPlugins = $true
$InstallLibrary = $true
# $AddToPath = $true

Set-Location -Path ".."

if (-Not (Test-Path -Path "lua")) {
    Invoke-WebRequest -Uri "https://www.lua.org/ftp/lua-5.4.4.tar.gz" -OutFile "./lua-5.4.4.tar"
    tar -xf ./lua-5.4.4.tar
    Remove-Item "./lua-5.4.4.tar"
    Move-Item -Path "./lua-5.4.4/src" -Destination "./lua"
    Remove-Item "./lua-5.4.4" -Recurse
    Set-Location -Path "./lua"
    make mingw
    Remove-Item "./*.c"
    Set-Location -Path ".."
}

if (-Not (Test-Path -Path $InstallLocation)) {
    New-Item -Path $InstallLocation -ItemType Directory
}

g++ main.cpp src/*.cpp -std=c++2a -Iinclude -Llua -Ilua -llua -Idependencies/include -static-libgcc -static-libstdc++ -luuid -o smake.exe
Move-Item -Force -Path "./smake.exe" -Destination $InstallLocation

Set-Location -Path "./install" # Move back into install directory

# Set up Smake app data
$SmakeAppData = "$env:APPDATA\Syntad\Smake" # DO NOT MODIFY

if (-Not (Test-Path -Path $SmakeAppData)) {
    New-Item -Path $SmakeAppData -ItemType Directory # Create Syntad and/or Smake folder
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "plugins") -ItemType Directory # Make plugins folder

    # Make config.json
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "config.json")
    Set-Content -Path (Join-Path -Path $SmakeAppData -ChildPath "config.json") -Value "{}"
}

if ($InstallPlugins) {
    $OfficialPluginsDir = (Join-Path -Path $SmakeAppData -ChildPath "plugins/smake")

    Invoke-WebRequest -Uri "https://github.com/Syntad/smake-plugins/archive/refs/heads/main.zip" -OutFile "./plugins.zip"
    tar -xf ./plugins.zip
    Remove-Item "./plugins.zip"

    if (Test-Path -Path $OfficialPluginsDir) {
        Remove-Item $OfficialPluginsDir -Recurse
    }

    Move-Item -Path "./smake-plugins-main" -Destination $OfficialPluginsDir
}

if ($InstallLibrary) {
    $LibraryDir = (Join-Path -Path $SmakeAppData -ChildPath "library")

    Invoke-WebRequest -Uri "https://github.com/Syntad/smake-lsp-library/archive/refs/heads/main.zip" -OutFile "./library.zip"
    tar -xf ./library.zip
    Remove-Item "./library.zip"
    Remove-Item -Path "./smake-lsp-library-main/library/plugins" -Recurse

    if (Test-Path -Path $LibraryDir) {
        Remove-Item $LibraryDir -Recurse
    }

    Move-Item -Path "./smake-lsp-library-main/library" -Destination $LibraryDir
    Remove-Item -Path "./smake-lsp-library-main" -Recurse
}