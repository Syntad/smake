#include <plugins.hpp>
#include <api.hpp>
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

    ExecuteResult runPluginFile(std::string name) {
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

    bool callPluginFunction(const char* name, int r = 0) {
        lua_getglobal(L, "Plugin");
        lua_pushstring(L, name);
        lua_rawget(L, -2);
        lua_remove(L, 1); // Remove Plugin table from stack

        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_call(L, 0, r);

            return true;
        }

        return false;
    }

    #pragma region Lua Functions

    int l_parseFlags(lua_State* L) {
        int opt;
        const char* optstring = luaL_checkstring(L, 1);
        lua_settop(L, 0);
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

    int l_include(lua_State* L) {
        const char* name = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        lua_newtable(L);
        lua_setglobal(L, "Plugin");

        // Execute plugin

        ExecuteResult result = runPluginFile(name);

        if (result != ExecuteResult::Ok) {
            if (result == ExecuteResult::NotFound) {
                return luaL_error(L, "Plugin '%s' not found", name);
            }

            return 0;
        }

        callPluginFunction("Include", LUA_MULTRET);

        // Remove Plugin from the environment
        lua_pushnil(L);
        lua_setglobal(L, "Plugin");

        return lua_gettop(L);
    }

    #pragma endregion

    const luaL_Reg lib[] = {
        { "ParseFlags", Plugins::l_parseFlags },
        { "SerializeFlags", Plugins::l_serializeFlags }
    };

    void Register(lua_State* L) {
        lua_register(L, "include", l_include);
    }

    void Init(lua_State* L, int argc, char* argv[]) {
        Plugins::L = L;
        Plugins::argc = argc;
        Plugins::argv = argv;
    }

    ExecuteResult Execute(std::string name) {
        // Setup lua state for CLI execution

        luaL_newlib(L, lib);
        lua_pushstring(L, "Arguments");
        lua_createtable(L, argc, 0);

        for (int i = 3; i < argc; i++) {
            lua_pushstring(L, argv[i]);
            lua_rawseti(L, -2, i - 2);
        }

        lua_settable(L, -3);

        lua_setglobal(L, "Plugin");

        // Execute
        ExecuteResult result = runPluginFile(name);

        if (result != ExecuteResult::Ok) {
            return result;
        }

        callPluginFunction("Command");

        return ExecuteResult::Ok;
    }
}