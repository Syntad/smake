#include <luaJson.hpp>
#include <stdexcept>
#include <iostream>

namespace LuaJSON {
    void pushJSONObject(lua_State* L, const rapidjson::Value& value) {
        if (!value.IsObject()) {
            throw std::invalid_argument("Value must be an object");
        }
        
        lua_createtable(L, 0, value.MemberCount());

        for (rapidjson::Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); it++) {
            lua_pushstring(L, it->name.GetString());
            LuaJSON::PushJSONValue(L, it->value);
            lua_rawset(L, -3);
        }
    }

    void pushJSONArray(lua_State* L, const rapidjson::Value& value) {
        if (!value.IsArray()) {
            throw std::invalid_argument("Value must be an array");
        }

        lua_createtable(L, value.Size(), 0);

        int i = 0;
        for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); it++) {
            LuaJSON::PushJSONValue(L, *it);
            lua_rawseti(L, -2, ++i);
        }
    }

    void pushJSONNumber(lua_State* L, const rapidjson::Value& value) {
        if (value.IsInt()) {
            lua_pushinteger(L, value.GetUint64());
        }
        else if (value.IsDouble()) {
            lua_pushnumber(L, value.GetDouble());
        }
        else {
            throw std::invalid_argument("Value must be an int or double");
        }
    }

    void PushJSONValue(lua_State* L, const rapidjson::Value& value) {
        switch (value.GetType()) {
            case rapidjson::Type::kNullType: lua_pushnil(L); break;
            case rapidjson::Type::kFalseType: lua_pushboolean(L, false); break;
            case rapidjson::Type::kTrueType: lua_pushboolean(L, true); break;
            case rapidjson::Type::kObjectType: pushJSONObject(L, value); break;
            case rapidjson::Type::kArrayType: pushJSONArray(L, value); break;
            case rapidjson::Type::kStringType: lua_pushstring(L, value.GetString()); break;
            case rapidjson::Type::kNumberType: pushJSONNumber(L, value); break;
            default: throw std::invalid_argument("Unsupported value type"); break;
        }
    }

    int l_decodeJson(lua_State* L) {
        const char* json = luaL_checkstring(L, 1);
        rapidjson::Document doc;
        doc.Parse(json);
        PushJSONValue(L, doc);

        return 1;
    }

    void Register(lua_State* L) {
        lua_getglobal(L, "smake");

        lua_pushstring(L, "jsonDecode");
        lua_pushcfunction(L, l_decodeJson);
        lua_rawset(L, -3);

        lua_pop(L, 1);
    }
}