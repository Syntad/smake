extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <clocale>
#include <iostream>
#include <build.h>
#include <stack>
#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
constexpr bool windows = true;
constexpr bool mac = false;
constexpr bool linux = false;
#elif __APPLE__
constexpr bool windows = false;
constexpr bool mac = true;
constexpr bool linux = false;
#else
constexpr bool windows = false;
constexpr bool mac = false;
constexpr bool linux = true;
#endif

// Helpers

void register_os(lua_State* L) {
    lua_pushboolean(L, windows);
    lua_setglobal(L, "is_windows");
    
    lua_pushboolean(L, mac);
    lua_setglobal(L, "is_mac");
    
    lua_pushboolean(L, mac);
    lua_setglobal(L, "is_linux");
    
    lua_pushstring(L, windows ? "windows" : (mac ? "mac" : "linux"));
    lua_setglobal(L, "os_name");
}

// Options

int standard(lua_State* L) {
    build::standard = luaL_checkstring(L, -1);
    lua_pop(L, 1);
    
    return 0;
}

int input(lua_State* L) {
    int nargs = lua_gettop(L);

    for (int i = 0; i < nargs; i++) {
        build::input.push_back(luaL_checkstring(L, -1));
        lua_pop(L, 1);
    }
    
    return 0;
}

int include(lua_State* L) {
    int nargs = lua_gettop(L);
    
    if (nargs == 0 || nargs > 3) {
        return luaL_error(L, "Include takes 1-3 args but got %d args.", nargs);
    }
    
    switch (nargs) {
        case 1: {
            int type = lua_type(L, -1);
            
            if (type == LUA_TSTRING) {
                build::include.push_back(lua_tostring(L, -1));
            }
            else if (type == LUA_TTABLE) {
                lua_pushnil(L);

                while (lua_next(L, -2)) {
                    build::include.push_back(lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
            else {
                luaL_error(L, "include expects type 'string' or 'table', but got '%s'", lua_typename(L, type));
            }
            
            lua_pop(L, 1);
            
            break;
        }
        case 2: {
            build::link_paths.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            
            build::include.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            
            break;
        }
        case 3: {
            build::link_names.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            
            build::link_paths.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            
            build::include.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            
            break;
        }
    };
    
    return 0;
}

int output(lua_State* L) {
    build::output = luaL_checkstring(L, -1);
    lua_pop(L, 1);
    
    return 0;
}

int run(lua_State* L) {
    std::stack<string> commands;
    int nargs = lua_gettop(L);

    for (int i = 0; i < nargs; i++) {
        commands.push(luaL_checkstring(L, -1));
        lua_pop(L, 1);
    }
    
    while (!commands.empty()) {
        system(commands.top().c_str());
        commands.pop();
    }
    
    return 0;
}

int flags(lua_State* L) {
    int nargs = lua_gettop(L);

    for (int i = 0; i < nargs; i++) {
        build::flags.push_back(luaL_checkstring(L, -1));
        lua_pop(L, 1);
    }
    
    return 0;
}

int build(lua_State* L) {
    std::cout << "Building..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    system(build::get_cmd().c_str());
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Build took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    
    return 0;
}

int main(int argc, char *argv[]) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    // constants
    register_os(L);

    // main build options
    lua_register(L, "standard", standard);
    lua_register(L, "input", input);
    lua_register(L, "include", include);
    lua_register(L, "output", output);
    lua_register(L, "run", run);
    lua_register(L, "flags", flags);
    lua_register(L ,"build", build);

    string file_name = "make.lua";

    if (luaL_loadfile(L, file_name.c_str()) != LUA_OK) {
        printf("Error loading file: %s\n", lua_tostring(L, -1));
        return 0;
    }

    if (argc > 1) {
        file_name = string(argv[1]);

        for (int i = 2; i < argc; i++) {
            lua_pushstring(L, argv[i]);
        }
    }

    if (lua_pcall(L, argc > 1 ? argc - 2 : 0, 0, 0) != LUA_OK) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    return 0;
}