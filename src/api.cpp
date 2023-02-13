#include <api.hpp>
#include <gpp.hpp>
#include <platform.hpp>
#include <string>
#include <stack>
using std::string;

namespace API {
    #pragma region Lua Functions

    int l_using(lua_State* L) {
        string name = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        if (name == "g++" || name == "gpp") {
            GPP::Register(L);
        }

        return 0;
    }

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

    #pragma endregion

    void Register(lua_State* L) {
        lua_register(L, "using", l_using);

        // Classes
        Platform::Register(L);

        // Globals
        lua_newtable(L);
        lua_setglobal(L, "smake");

        lua_register(L, "run", l_run);
    }

    void CallSmakeFunction(lua_State* L, const char* func) {
        lua_getglobal(L, "smake");

        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, func);
            
            if (lua_type(L, -1) == LUA_TFUNCTION) {
                lua_call(L, 0, 0);
            }
            else {
                lua_pop(L, 1);
            }
        }
        else {
            lua_pop(L, 1);
        }  
    }
}