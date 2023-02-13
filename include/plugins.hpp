#pragma once
#include <lua.hpp>
#include <string>

namespace Plugins {
    enum class ExecuteResult {
        Ok,
        NotFound,
        Error
    };

    extern void Init(lua_State* L, int argc, char* argv[]);
    extern ExecuteResult Execute(std::string name);
};