# Configuration
$InstallLocation = "C:\Program Files\Syntad"
$AddToPath = $true

# Installer
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

# Set up Smake app data
$SmakeAppData = "$env:APPDATA\Syntad\Smake" # DO NOT MODIFY

if (-Not (Test-Path -Path $SmakeAppData)) {
    New-Item -Path $SmakeAppData -ItemType Directory # Create Syntad and/or Smake folder
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "plugins") -ItemType Directory # Make plugins folder
    New-Item -Path (Join-Path -Path $SmakeAppData -ChildPath "config.json") # Make config.json
}