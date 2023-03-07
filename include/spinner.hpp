#pragma once
#include <lua.hpp>
#include <iostream>
#include <exception>

namespace Spinner {
    extern int l_start(lua_State* L);
    extern int l_stop(lua_State* L);
    extern void Register(lua_State* L);
};