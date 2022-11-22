// TODO: Add -i flag for install, add smake global table. Calls smake.install()
// TODO: Add -b flag for build. Calls smake.build(). Only necessary for explicit build function, smake with no args just runs file
// TODO: Add -f flag for file

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <platform.h>
#include <iostream>
#include <build.h>
#include <stack>
#include <chrono>
#include <unistd.h>

// Helpers

void register_platform(lua_State* L) {
    lua_createtable(L, 0, 6);

    lua_pushstring(L, "is_windows");
    lua_pushboolean(L, platform == "windows");
    lua_settable(L, -3);

    lua_pushstring(L, "is_osx");
    lua_pushboolean(L, platform == "osx");
    lua_settable(L, -3);

    lua_pushstring(L, "is_linux");
    lua_pushboolean(L, platform == "linux");
    lua_settable(L, -3);

    lua_pushstring(L, "is_unix");
    lua_pushboolean(L, platform == "unix");
    lua_settable(L, -3);

    lua_pushstring(L, "is_unknown");
    lua_pushboolean(L, platform == "unknown");
    lua_settable(L, -3);

    lua_pushstring(L, "name");
    lua_pushstring(L, platform);
    lua_settable(L, -3);

    lua_setglobal(L, "platform");
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
    printf("\e[?25l\r\033[33mBuilding...\033[0m");
    
    auto start = std::chrono::high_resolution_clock::now();
    system(build::get_cmd().c_str());
    auto end = std::chrono::high_resolution_clock::now();
    
    printf("\r\033[32mBuild Successful! \033[33m(%dms)\033[0m\e[?25h\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    
    return 0;
}

void call_smake_func(lua_State* L, const char* func) {
    lua_getglobal(L, "smake");
                
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, func);
        
        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_call(L, 0, 0);
        }
        else {
            lua_pop(L, 1);
        }
    }
    else {
        lua_pop(L, 1);
    }  
}

int main(int argc, char *argv[]) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    // Constants
    register_platform(L);
    lua_newtable(L);
    lua_setglobal(L, "smake");

    // Build options
    lua_register(L, "standard", standard);
    lua_register(L, "input", input);
    lua_register(L, "include", include);
    lua_register(L, "output", output);
    lua_register(L, "run", run);
    lua_register(L, "flags", flags);
    lua_register(L, "build", build);

    string file_name = "make.lua";
    
    // Parse arguments
    int opt;
    bool is_install = false, is_build = false;

    while ((opt = getopt(argc, argv, "ibf:")) != -1) {
        switch (opt) {
            case 'f': {
                file_name = string(optarg);
                break;
            }
            case 'i': {
                is_install = true;
                break;
            }
            case 'b': {
                is_build = true;
                break;
            }
        }
    }

    // Run
    if (luaL_loadfile(L, file_name.c_str()) != LUA_OK) {
        printf("Error loading file: %s\n", lua_tostring(L, -1));
        return 0;
    }

    for (int i = 2; i < argc; i++) {
        lua_pushstring(L, argv[i]);
    }

    if (lua_pcall(L, argc > 1 ? argc - 2 : 0, 0, 0) != LUA_OK) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }
    
    call_smake_func(L, "init");

    if (is_install) {
        call_smake_func(L, "install");
    }

    if (is_build || !is_install) {
        call_smake_func(L, "build");
    }

    return 0;
}