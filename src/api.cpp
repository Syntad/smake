#include <api.hpp>
#include <platform.hpp>
#include <plugins.hpp>
#include <string>
#include <stack>
#include <filesystem>
#include <unistd.h>
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

    #pragma endregion

    void Register(lua_State* L) {

        // Classes
        Platform::Register(L);
        Plugins::Register(L);

        // Globals
        lua_newtable(L);
        lua_setglobal(L, "smake");

        lua_register(L, "run", l_run);
        lua_register(L, "runIn", l_runIn);
    }

    bool PushSmakeFunction(lua_State* L, const char* func) {
        lua_getglobal(L, "smake");

        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_pushstring(L, func);
            lua_rawget(L, -2);

            if (lua_type(L, -1) == LUA_TFUNCTION) {
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