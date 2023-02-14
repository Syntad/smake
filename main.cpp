#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <iostream>
<<<<<<< HEAD
#include <cstring>
=======
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b

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

    if (argc > 1 && API::PushSmakeFunction(L, argv[1])) {
        for (int i = 2; i < argc; i++) {
            lua_pushstring(L, argv[i]);
        }

        lua_call(L, argc - 2, 0);

        return;
    }

    API::CallSmakeFunction(L, "build");
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