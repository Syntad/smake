#include <configuration.hpp>
#include <fstream>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <luaJson.hpp>
#include <iostream>
namespace fs = std::filesystem;

namespace Configuration {
    #pragma region Variables

    #ifdef __CYGWIN__
        #include <Windows.h>
        #include <knownfolders.h>
        #include <shlobj.h>

        fs::path APP_DATA;
        fs::path CONFIGURATION_PATH;
        fs::path globalPluginsDirectory;
    #else
        fs::path APP_DATA = fs::path(getenv("HOME")) / ".smake";
        fs::path CONFIGURATION_PATH = APP_DATA / "config.json";
        fs::path globalPluginsDirectory = APP_DATA / "plugins";
    #endif

    const std::filesystem::path RELATIVE_CONFIGURATION_FILE = "smake.json";
    std::filesystem::path relativePluginsDirectory = "./plugins";
    std::string smakeFileName = "smake.lua";

    #pragma endregion

    void parseJsonFile(rapidjson::Document& doc, const fs::path& path) {
        std::ifstream file(path);
        rapidjson::IStreamWrapper streamWrapper(file);
        doc.ParseStream(streamWrapper);
    }

    void loadGlobal(lua_State* L) {
        if (!fs::exists(CONFIGURATION_PATH)) {
            return;
        }

        rapidjson::Document doc;
        parseJsonFile(doc, CONFIGURATION_PATH);

        if (doc.HasMember("pluginsDirectory") && doc["pluginsDirectory"].IsString()) {
            globalPluginsDirectory = fs::path(doc["pluginsDirectory"].GetString());
        }

        if (doc.HasMember("smakeFileName") && doc["smakeFileName"].IsString()) {
            smakeFileName = fs::path(doc["smakeFileName"].GetString());
        }

        // Set global configuration
        lua_getglobal(L, "smake");

        lua_pushstring(L, "config");
        LuaJSON::PushJSONValue(L, doc);
        lua_rawset(L, -3);

        lua_pop(L, 1);
    }

    void loadLocal(lua_State* L) {
        if (!fs::exists(RELATIVE_CONFIGURATION_FILE)) {
            return;
        }

        rapidjson::Document doc;
        parseJsonFile(doc, RELATIVE_CONFIGURATION_FILE);

        if (doc.HasMember("pluginsDirectory") && doc["pluginsDirectory"].IsString()) {
            relativePluginsDirectory = fs::path(doc["pluginsDirectory"].GetString());
        }
    
        if (doc.HasMember("smakeFileName") && doc["smakeFileName"].IsString()) {
            smakeFileName = fs::path(doc["smakeFileName"].GetString());
        }

        // Merge configurations
        
        // Get config
        lua_getglobal(L, "smake");
        lua_pushstring(L, "config");
        lua_rawget(L, -2);

        // Push local config
        LuaJSON::PushJSONValue(L, doc);

        // Copy local config to global config
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            lua_pushvalue(L, -2); // clone key
            lua_rotate(L, -2, 1); // move value to top
            lua_rawset(L, -5);
        }

        // Clean stack
        lua_pop(L, 3);
    }

    void Load(lua_State* L) {
        #if __CYGWIN__
            LPWSTR programDataPath;

            if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &programDataPath)))
            {
                throw std::runtime_error("Could not get AppData path");
            }

            Configuration::APP_DATA = fs::path(programDataPath) / "Syntad/Smake";
            Configuration::CONFIGURATION_PATH = APP_DATA / "config.json";
            Configuration::globalPluginsDirectory = APP_DATA / "plugins";
        #endif

        loadGlobal(L);
        loadLocal(L);       
    }
}