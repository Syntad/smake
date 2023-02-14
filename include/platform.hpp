#pragma once
#include <lua.hpp>
#if defined(_WIN32) || defined(__CYGWIN__)
    constexpr const char* platform = "windows";
#elif defined(__linux__)
    constexpr const char* platform = "linux";
#elif defined(__APPLE__) && defined(__MACH__)
    constexpr const char* platform = "osx";
#elif defined(unix) || defined(__unix__) || defined(__unix)
    constexpr const char* platform = "unix";
#else
    constexpr const char* platform = "unknown";
#endif

namespace Platform {
    extern void Register(lua_State* L);
}