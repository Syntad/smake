#include <platform.hpp>
#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <unistd.h>
#include <vector>
#include <iostream>

void run(lua_State* L, int argc, char* argv[]) {
    const char* file_name = "smake.lua";

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

    std::vector<const char*> args;

    if (argc > 1) {
        std::copy(&argv[1], &argv[argc], std::back_inserter(args));

        for (int i = 0; i < args.size(); i++) {
            if (!strcmp(args[i], "-f")) {
                args.erase(args.begin() + i, args.begin() + i + 2);
            }
            else if (!strcmp(args[i], "-i") || !strcmp(args[i], "-b") || !strcmp(args[i], "-r")) {
                args.erase(args.begin() + i);
            }
        }
    }

    // Run
    if (luaL_dofile(L, file_name) != LUA_OK) {
        printf("Error executing file: %s\n", lua_tostring(L, -1));
        return;
    }

    if (!args.empty() && API::PushSmakeFunction(L, args[0])) {
        for (int i = 1; i < args.size(); i++) {
            lua_pushstring(L, args[i]);
        }

        lua_call(L, args.size() - 1, 0);

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