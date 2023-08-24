#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <lua.hpp>

namespace Configuration
{
    extern void Load(lua_State* L);

    // Const

    extern const std::filesystem::path RELATIVE_CONFIGURATION_FILE;

    // Configuration

    extern std::filesystem::path relativePluginsDirectory;
    extern std::string smakeFileName;
    extern std::string defaultCommand;
    extern std::unordered_map<std::string, std::string> aliases;
}