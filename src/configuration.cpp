#include <configuration.hpp>
#include <installer.hpp>
#include <fstream>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <luaJson.hpp>
#include <iostream>
namespace fs = std::filesystem;

namespace Configuration {
    #pragma region Variables

    const std::filesystem::path RELATIVE_CONFIGURATION_FILE = "smake.json";
    std::filesystem::path relativePluginsDirectory = "./plugins";
    std::string smakeFileName = "smake.lua";
    std::string defaultCommand = "build";
    std::unordered_map<std::string, std::string> aliases;

    #pragma endregion

    void parseJsonFile(rapidjson::Document& doc, const fs::path& path) {
        std::ifstream file(path);
        rapidjson::IStreamWrapper streamWrapper(file);
        doc.ParseStream(streamWrapper);
    }

    void setConfig(lua_State* L, const rapidjson::Document& doc) {
        lua_getglobal(L, "smake");

        lua_pushstring(L, "config");
        LuaJSON::PushJSONValue(L, doc);
        lua_rawset(L, -3);
    }

    void mergeConfig(lua_State* L, const rapidjson::Document& doc) {
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
    }

    void loadShared(const rapidjson::Document& doc) {
        if (doc.HasMember("smakeFileName") && doc["smakeFileName"].IsString()) {
            smakeFileName = fs::path(doc["smakeFileName"].GetString());
        }

        if (doc.HasMember("defaultCommand") && doc["defaultCommand"].IsString()) {
            defaultCommand = doc["defaultCommand"].GetString();
        }

        if (doc.HasMember("aliases") && doc["aliases"].IsObject()) {
            rapidjson::GenericObject obj = doc["aliases"].GetObject();

            for (rapidjson::Value::ConstMemberIterator itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr) {
                const rapidjson::Value& objName = obj[itr->name.GetString()];

                if (itr->name.IsString() && itr->value.IsString()) {
                    aliases[itr->name.GetString()] = itr->value.GetString();
                }
            }
        }
    }

    void loadGlobal(lua_State* L) {
        if (!fs::exists(Installer::CONFIGURATION_PATH)) {
            return;
        }

        rapidjson::Document doc;
        parseJsonFile(doc, Installer::CONFIGURATION_PATH);

        loadShared(doc);
        setConfig(L, doc);

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

        loadShared(doc);
        mergeConfig(L, doc);
        
        // Clean stack
        lua_pop(L, 3);
    }

    void Load(lua_State* L) {
        loadGlobal(L);
        loadLocal(L);       
    }
}