#pragma once
#include <string>
#include <filesystem>

namespace Configuration
{
    extern void Load();

    // Const

    extern const std::filesystem::path APP_DATA;
    extern const std::filesystem::path GLOBAL_CONFIGURATION_PATH;
    extern const std::filesystem::path RELATIVE_CONFIGURATION_FILE;

    // Configuration
    extern std::filesystem::path globalPluginsDirectory;
    extern std::filesystem::path relativePluginsDirectory;
}