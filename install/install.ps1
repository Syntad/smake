# Configuration
$InstallLocation = "C:\Program Files\Syntad"
$InstallPlugins = $true
$InstallLibrary = $true
# $AddToPath = $true

Set-Location -Path ".."

if (-Not (Test-Path -Path "./dependencies")) {
    New-Item -Path "./dependencies" -ItemType Directory
}

# Download and build Lua
if (-Not (Test-Path -Path "./dependencies/lua")) {
    Invoke-WebRequest -Uri "https://www.lua.org/ftp/lua-5.4.4.tar.gz" -OutFile "./lua-5.4.4.tar"
    tar -xf ./lua-5.4.4.tar
    Remove-Item "./lua-5.4.4.tar"
    Set-Location "./lua-5.4.4/src"
    make mingw
    Set-Location -Path "../.."

    New-Item -Path "./dependencies/lua" -ItemType Directory
    New-Item -Path "./dependencies/lua/include" -ItemType Directory
    New-Item -Path "./dependencies/lua/lib" -ItemType Directory

    Move-Item -Path "./lua-5.4.4/src/*.h" -Destination "./dependencies/lua/include"
    Move-Item -Path "./lua-5.4.4/src/*.hpp" -Destination "./dependencies/lua/include"
    Move-Item -Path "./lua-5.4.4/src/*.a" -Destination "./dependencies/lua/lib"

    Remove-Item -Path "./lua-5.4.4" -Recurse
}

# Download RapidJSON
if (-Not (Test-Path -Path "./dependencies/rapidjson")) {
    Invoke-WebRequest -Uri "https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip" -OutFile "./rapidjson.zip"
    tar -xf ./rapidjson.zip
    Remove-Item "./rapidjson.zip"

    New-Item -Path "./dependencies/rapidjson" -ItemType Directory

    Move-Item -Path "./rapidjson-master/include" -Destination "./dependencies/rapidjson/include"

    Remove-Item -Path "./rapidjson-master" -Recurse
}

if (-Not (Test-Path -Path $InstallLocation)) {
    New-Item -Path $InstallLocation -ItemType Directory
}

Write-Output "Building Smake"
g++ main.cpp src/*.cpp -std=c++2a -Iinclude -Ldependencies/lua/lib -Idependencies/lua/include -llua -Idependencies/rapidjson/include -static-libgcc -static-libstdc++ -luuid -lole32 -o smake.exe
Move-Item -Force -Path "./smake.exe" -Destination $InstallLocation

Set-Location -Path "./install" # Move back into install directory

Write-Output "Installed Smake"

# Set up Smake app data
$SmakeAppData = "$env:APPDATA\Syntad\Smake" # DO NOT MODIFY

if (-Not (Test-Path -Path $SmakeAppData)) {
    New-Item -Path $SmakeAppData -ItemType Directory # Create Syntad and/or Smake folder
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "plugins") -ItemType Directory # Make plugins folder

    # Make config.json
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "config.json")
    Set-Content -Path (Join-Path -Path $SmakeAppData -ChildPath "config.json") -Value "{}"

    Write-Output "Finished setting up Smake app data"
}

if ($InstallPlugins) {
    $OfficialPluginsDir = (Join-Path -Path $SmakeAppData -ChildPath "plugins/smake")

    Invoke-WebRequest -Uri "https://github.com/Syntad/smake-plugins/archive/refs/heads/main.zip" -OutFile "./plugins.zip"
    tar -xf ./plugins.zip
    Remove-Item "./plugins.zip"

    if (Test-Path -Path $OfficialPluginsDir) {
        Remove-Item $OfficialPluginsDir -Recurse
    }

    Move-Item -Path "./smake-plugins-main/smake" -Destination $OfficialPluginsDir
    Remove-Item -Path "./smake-plugins-main" -Recurse

    Write-Output "Installed Smake plugins"
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

    Write-Output "Installed Smake library"
}

Write-Output "Smake installed successfully"