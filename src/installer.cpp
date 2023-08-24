#include <installer.hpp>
#include <fstream>

namespace Installer
{
    #if defined(__CYGWIN__) || defined(_WIN32)
        #include <Windows.h>
        #include <knownfolders.h>
        #include <shlobj.h>

        fs::path APP_DATA;
        fs::path CONFIGURATION_PATH;
        fs::path GLOBAL_PLUGINS_DIRECTORY;
    #else
        fs::path APP_DATA = fs::path(getenv("HOME")) / ".smake";
        fs::path CONFIGURATION_PATH = APP_DATA / "config.json";
        fs::path GLOBAL_PLUGINS_DIRECTORY = APP_DATA / "plugins";
    #endif

    void Init() {
        #if defined(__CYGWIN__) || defined(_WIN32)
            LPWSTR programDataPath;

            if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &programDataPath)))
            {
                throw std::runtime_error("Could not get AppData path");
            }

            APP_DATA = fs::path(programDataPath) / "Syntad/Smake";
            CONFIGURATION_PATH = APP_DATA / "config.json";
            GLOBAL_PLUGINS_DIRECTORY = APP_DATA / "plugins";
        #endif

        if (!fs::exists(APP_DATA)) {
            fs::create_directory(APP_DATA);
            fs::create_directory(GLOBAL_PLUGINS_DIRECTORY);
            
            std::ofstream configFile(CONFIGURATION_PATH);
            configFile << "{}";
            configFile.close();
        }
    }

    // TODO
    void InstallPlugins() {

    }

    // TODO
    bool CheckForUpdates() {
        return false;
    }

    // TODO
    void Update() {
        
    }
}