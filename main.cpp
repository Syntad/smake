#include <platform.hpp>
#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <unistd.h>
#include <iostream>

void run(lua_State* L, int argc, char* argv[]) {
    const char* file_name = "make.lua";

    // Parse arguments
    int opt;
    bool install = false, build = false, run = false;

    if (argc >= 3 && !strcmp(argv[1], "-p")) {
        Plugins::ExecuteResult result = Plugins::Execute(argv[2]);

        if (result == Plugins::ExecuteResult::NotFound) {
            std::cerr << "Plugin not found" << std::endl;
        }
        
        return;
    }

    while ((opt = getopt(argc, argv, "ibrf:p:")) != -1) {
        switch (opt) {
            case 'f': {
                file_name = optarg;
                break;
            }
            case 'i': {
                install = true;
                break;
            }
            case 'b': {
                build = true;
                break;
            }
            case 'r': {
                run = true;
                break;
            }
        }
    }

    // Run
    if (luaL_loadfile(L, file_name) != LUA_OK) {
        printf("Error loading file: %s\n", lua_tostring(L, -1));
        return;
    }

    for (int i = 2; i < argc; i++) {
        lua_pushstring(L, argv[i]);
    }

    if (lua_pcall(L, argc > 1 ? argc - 2 : 0, 0, 0) != LUA_OK) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    if (argc == 2 && API::CallSmakeFunction(L, argv[1])) {
        return;
    }

    API::CallSmakeFunction(L, "init");

    if (install) {
        API::CallSmakeFunction(L, "install");
    }

    if (build || (!install && !run)) {
        API::CallSmakeFunction(L, "build");
    }

    if (run) {
        API::CallSmakeFunction(L, "run");
    }
}

int main(int argc, char *argv[]) {
    Configuration::Load();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    API::Register(L);
    Plugins::Init(L, argc, argv);
    run(L, argc, argv);

    return 0;
}