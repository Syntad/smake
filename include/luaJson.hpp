#pragma once
#include <lua.hpp>
#include <rapidjson/document.h>

namespace LuaJSON {
    extern void pushJSONValue(lua_State* L, const rapidjson::Value& value);
}