#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

std::vector<std::string> getFunctionNames(lua_State* L) {
    std::vector<std::string> names;
    lua_getglobal(L, "smake");
    lua_pushnil(L);

    while (lua_next(L, -2)) {
        if (lua_isfunction(L, -1) && lua_isstring(L, -2)) {
            names.emplace_back(lua_tostring(L, -2));
        }

        lua_pop(L, 1);
    }

    return names;
}

void run(lua_State* L, int argc, char* argv[]) {
    // Plugin CLI
    if (argc >= 3 && !strcmp(argv[1], "-p")) {
        Plugins::ExecuteResult result = Plugins::Execute(argv[2]);

        if (result == Plugins::ExecuteResult::NotFound) {
            std::cerr << "Plugin not found" << std::endl;
        }

        return;
    }

    // Run
    if (luaL_dofile(L, Configuration::smakeFileName.c_str()) != LUA_OK) {
        printf("Error executing file: %s\n", lua_tostring(L, -1));
        return;
    }

    if (argc > 1) {
        const char* argument = argv[1];

        if (API::PushSmakeFunction(L, argument)) { // Check for function first to allow overriding
            for (int i = 2; i < argc; i++) {
                lua_pushstring(L, argv[i]);
            }

            lua_call(L, argc - 2, 0);
        }
        else if (!strcmp(argument, "help") || !strcmp(argument, "--help")) {
            std::string help =
            "\033[4mFunctions\033[0m";
            std::vector<std::string> names = getFunctionNames(L);

            for (const auto& name : names) {
                help += "\n- " + name;
            }

            std::cout << help << std::endl;
        }

        return;
    }

    API::CallSmakeFunction(L, "build");
}

int main(int argc, char *argv[]) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    API::Register(L);
    Configuration::Load(L);
    Plugins::Init(L, argc, argv);
    run(L, argc, argv);

    return 0;
}