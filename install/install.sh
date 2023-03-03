# Configuration
InstallLocation="/usr/local/bin"
InstallPlugins=true
InstallLibrary=true

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

    # Make config.json
    touch "$SmakeAppData/config.json"
    printf '{}' > $HOME/.smake/config.json
fi

if [ $InstallPlugins ]; then
    curl "https://github.com/Syntad/smake-plugins/archive/refs/heads/main.zip" -L -o plugins.zip
    tar -xf plugins.zip
    rm plugins.zip
    rm -rf $HOME/.smake/plugins/smake
    mv ./smake-plugins-main $HOME/.smake/plugins/smake
fi

if [ $InstallLibrary ]; then
    curl "https://github.com/Syntad/smake-lsp-library/archive/refs/heads/main.zip" -L -o library.zip
    tar -xf library.zip
    rm library.zip
    rm -rf $HOME/.smake/library
    rm -rf ./smake-lsp-library-main/library/plugins
    mv ./smake-lsp-library-main/library $HOME/.smake/library
fi