# Configuration
InstallLocation="/usr/local/bin"
AddToPath=true

cd ..

if [ ! -d "lua" ]; then
    curl "https://www.lua.org/ftp/lua-5.4.4.tar.gz" -o ./lua-5.4.4.tar
    tar -xf ./lua-5.4.4.tar
    rm ./lua-5.4.4.tar
    mv ./lua-5.4.4/src ./lua
    rm -rf ./lua-5.4.4
    cd lua
    make
    rm ./*.c
    cd ..
fi

g++ main.cpp src/*.cpp -std=c++2a -Iinclude -Llua -Ilua -llua -Idependencies/include -o smake
sudo mv ./smake $InstallLocation

# Set up Smake app data
SmakeAppData="$HOME/.smake"

if [ ! -d $SmakeAppData ]; then
    mkdir $SmakeAppData
    mkdir "$SmakeAppData/plugins"
    touch "$SmakeAppData/config.json"
fi