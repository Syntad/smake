#pragma once
#include <string>
#include <filesystem>

namespace Configuration
{
    extern void Load();

    // Const

<<<<<<< HEAD
    extern std::filesystem::path APP_DATA;
    extern std::filesystem::path GLOBAL_CONFIGURATION_PATH;
=======
    extern const std::filesystem::path APP_DATA;
    extern const std::filesystem::path GLOBAL_CONFIGURATION_PATH;
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
    extern const std::filesystem::path RELATIVE_CONFIGURATION_FILE;

    // Configuration
    extern std::filesystem::path globalPluginsDirectory;
    extern std::filesystem::path relativePluginsDirectory;
    extern std::string smakeFileName;
}