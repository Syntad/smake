# Configuration
InstallLocation="/usr/local/bin"
InstallPlugins=true
InstallLibrary=true

cd ..

if [ ! -d "./dependencies" ]; then
    mkdir ./dependencies
fi

# Download and build lua
if [ ! -d "./dependencies/lua" ]; then
    curl "https://www.lua.org/ftp/lua-5.4.4.tar.gz" -o ./lua-5.4.4.tar
    tar -xf ./lua-5.4.4.tar
    rm ./lua-5.4.4.tar
    cd ./lua-5.4.4/src
    make
    cd ../..

    mkdir dependencies/lua dependencies/lua/include dependencies/lua/lib

    mv ./lua-5.4.4/src/*.h ./dependencies/lua/include
    mv ./lua-5.4.4/src/*.hpp ./dependencies/lua/include
    mv ./lua-5.4.4/src/*.a ./dependencies/lua/lib

    rm -rf ./lua-5.4.4
fi

# Download RapidJSON
if [ ! -d "./dependencies/rapidjson" ]; then
    curl "https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip" -L -o ./rapidjson.zip
    unzip -q ./rapidjson.zip
    rm ./rapidjson.zip
    
    mkdir ./dependencies/rapidjson

    mv ./rapidjson-master/include ./dependencies/rapidjson/include

    rm -rf ./rapidjson-master
fi

g++ main.cpp src/*.cpp -std=c++2a -Iinclude -Ldependencies/lua/lib -Idependencies/lua/include -llua -Idependencies/rapidjson/include -o smake.o
sudo mv ./smake.o "$InstallLocation/smake"

# Set up Smake app data
SmakeAppData="$HOME/.smake"

if [ ! -d $SmakeAppData ]; then
    mkdir $SmakeAppData
    mkdir "$SmakeAppData/plugins"

    # Make config.json
    touch "$SmakeAppData/config.json"
    printf '{}' > $HOME/.smake/config.json
fi

if [ $InstallPlugins ]; then
    curl "https://github.com/Syntad/smake-plugins/archive/refs/heads/main.zip" -L -o plugins.zip
    unzip -q plugins.zip
    rm plugins.zip
    rm -rf $HOME/.smake/plugins/smake
    mv ./smake-plugins-main/smake $HOME/.smake/plugins/smake
    rm -rf ./smake-plugins-main
fi

if [ $InstallLibrary ]; then
    curl "https://github.com/Syntad/smake-lsp-library/archive/refs/heads/main.zip" -L -o library.zip
    unzip -q library.zip
    rm library.zip
    rm -rf $HOME/.smake/library
    rm -rf ./smake-lsp-library-main/library/plugins
    mv ./smake-lsp-library-main/library $HOME/.smake/library
    rm -rf ./smake-lsp-library-main
fi