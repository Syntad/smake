#include <api.h>
#include <gpp.h>
#include <platform.h>
#include <string>
using std::string;

namespace API {
    #pragma region Lua Functions

    int l_using(lua_State* L) {
        string name = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        if (name == "g++" || name == "gpp") {
            gpp::Register(L);
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