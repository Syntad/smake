#include <platform.hpp>
#include <cstring>

namespace Platform {
    void Register(lua_State* L) {
        lua_createtable(L, 0, 6);

        lua_pushstring(L, "is_windows");
        lua_pushboolean(L, !strcmp(platform, "windows"));
        lua_settable(L, -3);

        lua_pushstring(L, "is_osx");
        lua_pushboolean(L, !strcmp(platform, "osx"));
        lua_settable(L, -3);

        lua_pushstring(L, "is_linux");
        lua_pushboolean(L, !strcmp(platform, "linux"));
        lua_settable(L, -3);

        lua_pushstring(L, "is_unix");
        lua_pushboolean(L, !strcmp(platform, "unix"));
        lua_settable(L, -3);

        lua_pushstring(L, "is_unknown");
        lua_pushboolean(L, !strcmp(platform, "unknown"));
        lua_settable(L, -3);

        lua_pushstring(L, "name");
        lua_pushstring(L, platform);
        lua_settable(L, -3);

        lua_setglobal(L, "platform");
    }
}