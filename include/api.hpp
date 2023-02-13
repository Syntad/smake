#pragma once
#include <lua.hpp>

namespace API {
    extern void Register(lua_State* L);
    extern bool CallSmakeFunction(lua_State* L, const char* func);
};