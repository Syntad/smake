#include <paths.hpp>

#if defined(__CYGWIN__) || defined(_WIN32)
    #include <Windows.h>
    #include <knownfolders.h>
    #include <shlobj.h>
#else
    #include <unistd.h>
    #include <pwd.h>
#endif

namespace Paths {
    fs::path APP_DATA;
    fs::path CONFIGURATION_PATH;
    fs::path GLOBAL_PLUGINS_DIRECTORY;

    void Init() {
        #if defined(__CYGWIN__) || defined(_WIN32)
            LPWSTR programDataPath;

            if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &programDataPath)))
            {
                throw std::runtime_error("Could not get AppData path");
            }

            APP_DATA = fs::path(programDataPath) / "Syntad/Smake";

            CoTaskMemFree(programDataPath);
        #else
            passwd* pw = getpwuid(getuid());

            if (!pw) {
                throw std::runtime_error("Could not get user info");
            }

            APP_DATA = fs::path(pw->pw_dir) / ".smake";
        #endif

        CONFIGURATION_PATH = APP_DATA / "config.json";
        GLOBAL_PLUGINS_DIRECTORY = APP_DATA / "plugins";
    }
}