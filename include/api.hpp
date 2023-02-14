#pragma once
#include <lua.hpp>

namespace API {
    extern void Register(lua_State* L);
    extern bool IsSmakeFunction(lua_State* L, const char* func);
    extern bool PushSmakeFunction(lua_State* L, const char* func);
    extern bool CallSmakeFunction(lua_State* L, const char* func);
};