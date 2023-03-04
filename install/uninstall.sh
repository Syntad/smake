InstallLocation="/usr/local/bin"
SmakeAppData="$HOME/.smake"

if [ -d $InstallLocation ]; then
    sudo rm "$InstallLocation/smake"
fi

if [ -d $SmakeAppData ]; then
    rm -rf $SmakeAppData
fi