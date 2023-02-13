#include <plugins.hpp>
#include <configuration.hpp>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;

// Uses sandboxed Lua state

namespace Plugins {
    lua_State* L;
    int argc;
    char** argv;

    #pragma region Lua Functions

    // Plugins.Register('luau-bundle', 'fbc', function(plugin))

    int l_parseFlags(lua_State* L) {
        int opt;
        const char* optstring = luaL_checkstring(L, 1);
        optind = 3; // Reset to the argument after the plugin

        lua_newtable(L);

        while ((opt = getopt(argc, argv, optstring)) != -1) {
            lua_pushstring(L, (char*)&opt);

            if (optarg) {
                lua_pushstring(L, optarg);
            }
            else {
                lua_pushboolean(L, true);
            }

            lua_rawset(L, -3);
        }

        return 1;
    }

    int l_serializeFlags(lua_State* L) {
        std::string serialized;

        for (int i = 3; i < argc; i++) {
            serialized += argv[i];
            serialized += " ";
        }

        lua_pushstring(L, serialized.c_str());
        
        return 1;
    }

    #pragma endregion

    const luaL_Reg lib[] = {
        { "ParseFlags", Plugins::l_parseFlags },
        { "SerializeFlags", Plugins::l_serializeFlags }
    };

    void Init(lua_State* L, int argc, char* argv[]) {
        Plugins::L = L;
        Plugins::argc = argc;
        Plugins::argv = argv;

        luaL_newlib(L, lib);

        // Plugins.Arguments

        lua_pushstring(L, "Arguments");
        lua_createtable(L, argc, 0);

        for (int i = 3; i < argc; i++) {
            lua_pushstring(L, argv[i]);
            lua_rawseti(L, -2, i - 2);
        }

        lua_settable(L, -3);

        lua_setglobal(L, "Plugin");
    }

    ExecuteResult Execute(std::string name) {
        if (!name.ends_with(".lua")) {
            name += ".lua";
        }

        fs::path path = Configuration::globalPluginsDirectory / name;

        if (!fs::exists(path)) {
            path = Configuration::relativePluginsDirectory / name;
        }

        if (fs::exists(path)) {
            if (luaL_dofile(L, path.c_str()) != LUA_OK) {
                printf("Error loading plugin (%s): %s\n", path.c_str(), lua_tostring(L, -1));
                return ExecuteResult::Error;
            }

            return ExecuteResult::Ok;
        }

        return ExecuteResult::NotFound;
    }
}