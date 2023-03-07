#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <unistd.h>
#include <iostream>
namespace fs = std::filesystem;

namespace Plugins {
    lua_State* L;
    int argc;
    char** argv;

    ExecuteResult runPluginFile(std::string& name) {
        if (!name.ends_with(".lua")) {
            name += ".lua";
        }

        fs::path path = Configuration::relativePluginsDirectory / name;

        if (!fs::exists(path)) {
            path = Configuration::globalPluginsDirectory / name;
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

    bool pushPluginFunction(const char* name, int r = 0, bool pushArgs = false) {
        lua_getglobal(L, "Plugin");
        lua_pushstring(L, name);
        lua_rawget(L, -2);
        lua_remove(L, -2); // Remove Plugin table from stack

        if (lua_isfunction(L, -1)) {
            return true;
        }

        lua_pop(L, 1);

        return false;
    }

    bool callPluginFunction(const char* name, int n, int r) {
        if (pushPluginFunction(name)) {
            lua_call(L, n, r); // pushArgs ? argc - 3 : 0
            return true;
        }

        return false;
    }

    void makeGlobal(lua_State* L, int idx) {
        lua_pushglobaltable(L);

        lua_pushnil(L);
        while (lua_next(L, idx)) {
            lua_pushvalue(L, -2);
            lua_pushvalue(L, -2);
            lua_rawset(L, -5);
            lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }

    #pragma region Lua Functions

    int l_parseFlags(lua_State* L) {
        int opt;
        std::string optstring(luaL_checkstring(L, 1));
        lua_settop(L, 0);
        optind = 3; // Reset to the argument after the plugin

        lua_newtable(L);

        while ((opt = getopt(argc, argv, optstring.c_str())) != -1) {
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

    int l_import(lua_State* L) {
        int nargs = lua_gettop(L);

        if (nargs > 2) {
            std::cerr << "Invalid number of arguments provided to import" << std::endl;
            return 0;
        }

        bool global = false;

        if (nargs == 2) {
            if (lua_isnil(L, 2)) {
                lua_pop(L, 1);
            }
            else {
                luaL_checktype(L, 2, LUA_TBOOLEAN);
                global = lua_toboolean(L, 2);
                lua_pop(L, 1);
            }
        }

        std::string name(luaL_checkstring(L, 1));
        lua_pop(L, 1);

        // Setup new Plugin table and backup old one if it exists

        int pluginTableBackup = -1;
        lua_getglobal(L, "Plugin");

        if (lua_istable(L, -1)) {
            pluginTableBackup = lua_gettop(L);
        }
        else {
            lua_pop(L, 1);
        }

        lua_newtable(L);
        lua_setglobal(L, "Plugin");

        // Execute plugin

        ExecuteResult result = runPluginFile(name);

        if (result != ExecuteResult::Ok) {
            if (result == ExecuteResult::NotFound) {
                return luaL_error(L, "Plugin '%s' not found", name.c_str());
            }

            return 0;
        }

        callPluginFunction("Import", 0, LUA_MULTRET);

        // Remove Plugin from the environment or replace it with the backup if it exists
    
        if (pluginTableBackup != -1) {
            lua_rotate(L, pluginTableBackup, lua_gettop(L) - pluginTableBackup); // Move plugin table table backup to the top of the stack
            lua_setglobal(L, "Plugin");
        }
        else {
            lua_pushnil(L);
            lua_setglobal(L, "Plugin");
        }

        int nret = lua_gettop(L);

        // Make global if second argument is true
        if (global) {
            for (int i = 1; i <= nret; i++) {
                if (lua_istable(L, i)) {
                    makeGlobal(L, i);
                }
            }
        }

        return nret;
    }

    #pragma endregion

    const luaL_Reg lib[] = {
        { "ParseFlags", Plugins::l_parseFlags },
        { "SerializeFlags", Plugins::l_serializeFlags },
        { NULL, NULL }
    };

    void Register(lua_State* L) {
        lua_register(L, "import", l_import);
    }

    void Init(lua_State* L, int argc, char* argv[]) {
        Plugins::L = L;
        Plugins::argc = argc;
        Plugins::argv = argv;
    }

    ExecuteResult Execute(std::string name) {
        // Setup lua state for CLI execution

        luaL_newlib(L, lib);
        lua_newtable(L);

        // Make Plugin library global
        lua_setglobal(L, "Plugin");

        // Execute
        ExecuteResult result = runPluginFile(name);

        if (result != ExecuteResult::Ok) {
            return result;
        }

        if (pushPluginFunction("Command")) {
            // Push arguments
            for (int i = 3; i < argc; i++) {
                lua_pushstring(L, argv[i]);
            }

            lua_call(L, argc - 3, 0);
        }

        return ExecuteResult::Ok;
    }
}