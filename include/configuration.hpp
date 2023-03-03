#pragma once
#include <string>
#include <filesystem>
#include <lua.hpp>

namespace Configuration
{
    extern void Load(lua_State* L);

    // Const

    extern std::filesystem::path APP_DATA;
    extern std::filesystem::path GLOBAL_CONFIGURATION_PATH;
    extern const std::filesystem::path RELATIVE_CONFIGURATION_FILE;

    // Configuration
    extern std::filesystem::path globalPluginsDirectory;
    extern std::filesystem::path relativePluginsDirectory;
    extern std::string smakeFileName;
}