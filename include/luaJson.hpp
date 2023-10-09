#pragma once
#include <lua.hpp>
#include <rapidjson/document.h>

namespace LuaJSON {
    extern void PushJSONValue(lua_State* L, const rapidjson::Value& value);
    extern void Register(lua_State* L);
}