#include <api.hpp>
#include <platform.hpp>
#include <plugins.hpp>
#include <spinner.hpp>
#include <string>
#include <stack>
#include <filesystem>
#include <unistd.h>
#include <thread>
using std::string;
namespace fs = std::filesystem;

namespace API {
    #pragma region Lua Functions

    int l_run(lua_State* L) {
        std::stack<string> commands;
        int nargs = lua_gettop(L);

        for (int i = 0; i < nargs; i++) {
            commands.push(luaL_checkstring(L, -1));
            lua_pop(L, 1);
        }
        
        while (!commands.empty()) {
            system(commands.top().c_str());
            commands.pop();
        }

        return 0;
    }

    int l_runIn(lua_State* L) {
        fs::path cwd = fs::current_path();

        const char* path = luaL_checkstring(L, 1);
        lua_remove(L, 1);

        chdir(path);
        l_run(L);
        chdir(cwd.c_str());

        return 0;
    }

    int l_runInThread(lua_State* L) {
        std::stack<string> commands;
        int nargs = lua_gettop(L);

        for (int i = 0; i < nargs; i++) {
            commands.push(luaL_checkstring(L, -1));
            lua_pop(L, 1);
        }

        std::thread([commands]() mutable {
            while (!commands.empty()) {
                system(commands.top().c_str());
                commands.pop();
            }
        });

        return 0;
    }

    #pragma endregion

    void Register(lua_State* L) {
        // Globals
        lua_newtable(L);
        lua_setglobal(L, "smake");

        lua_register(L, "run", l_run);
        lua_register(L, "runIn", l_runIn);
        lua_register(L, "runInThread", l_runInThread);

        // Classes
        Platform::Register(L);
        Plugins::Register(L);
        Spinner::Register(L);
    }

    bool PushSmakeFunction(lua_State* L, const char* func) {
        lua_getglobal(L, "smake");

        if (lua_istable(L, -1)) {
            lua_pushstring(L, func);
            lua_rawget(L, -2);

            if (lua_isfunction(L, -1)) {
                lua_remove(L, 1);
                return true;
            }
        }

        lua_pop(L, 1);
        return false;
    }

    bool CallSmakeFunction(lua_State* L, const char* func) {
        if (PushSmakeFunction(L, func)) {
            lua_call(L, 0, 0);
            return true;
        }

        return false;
    }
}