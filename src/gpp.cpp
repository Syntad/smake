#include <gpp.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

// Initialize

namespace GPP {
    const string compiler = "g++";
    string standard;
    vector<string> input;
    vector<string> include;
    vector<string> link_paths;
    vector<string> link_names;
    vector<string> flags;
    string output;

    string getCommand() {
        string cmd = compiler;
        
        if (standard.length() > 0) {
            cmd += " -std=" + standard;
        }
        
        for (string i : input) {
            cmd += " " + i;
        }
        
        for (string i : include) {
            cmd += " -I" + i;
        }
        
        for (string l : link_paths) {
            cmd += " -L" + l;
        }
        
        for (string l : link_names) {
            cmd += " -l" + l;
        }
        
        for (string f : flags) {
            cmd += " " + f;
        }
        
        if (output.length() > 0) {
            cmd += " -o" + output;
        }
        
        return cmd;
    }

    #pragma region Lua Functions

    int l_standard(lua_State* L) {
        standard = luaL_checkstring(L, -1);
        lua_pop(L, 1);
        
        return 0;
    }

    int l_input(lua_State* L) {
        int nargs = lua_gettop(L);

        for (int i = 0; i < nargs; i++) {
            input.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
        }

        return 0;
    }

    int l_include(lua_State* L) {
        int nargs = lua_gettop(L);

        if (nargs == 0 || nargs > 3) {
            return luaL_error(L, "Include takes 1-3 args but got %d args.", nargs);
        }

        switch (nargs) {
            case 1: {
                int type = lua_type(L, -1);
                
                if (type == LUA_TSTRING) {
                    include.push_back(lua_tostring(L, -1));
                }
                else if (type == LUA_TTABLE) {
                    lua_pushnil(L);

                    while (lua_next(L, -2)) {
                        include.push_back(lua_tostring(L, -1));
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
                link_paths.push_back(luaL_checkstring(L, -1));
                lua_pop(L, 1);
                
                include.push_back(luaL_checkstring(L, -1));
                lua_pop(L, 1);
                
                break;
            }
            case 3: {
                link_names.push_back(luaL_checkstring(L, -1));
                lua_pop(L, 1);
                
                link_paths.push_back(luaL_checkstring(L, -1));
                lua_pop(L, 1);
                
                include.push_back(luaL_checkstring(L, -1));
                lua_pop(L, 1);
                
                break;
            }
        };
        
        return 0;
    }

    int l_output(lua_State* L) {
        output = luaL_checkstring(L, -1);
        lua_pop(L, 1);
        
        return 0;
    }

    int l_flags(lua_State* L) {
        int nargs = lua_gettop(L);

        for (int i = 0; i < nargs; i++) {
            flags.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
        }
        
        return 0;
    }

    int l_build(lua_State* L) {
        printf("\e[?25l\r\033[33mBuilding...\033[0m");
        
        auto start = std::chrono::high_resolution_clock::now();
        system(getCommand().c_str());
        auto end = std::chrono::high_resolution_clock::now();
        
        printf("\r\033[32mBuild Successful! \033[33m(%lldms)\033[0m\e[?25h\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        
        return 0;
    }

    #pragma endregion

    void Register(lua_State* L) {
        lua_register(L, "standard", GPP::l_standard);
        lua_register(L, "input",    GPP::l_input);
        lua_register(L, "include",  GPP::l_include);
        lua_register(L, "output",   GPP::l_output);
        lua_register(L, "flags",    GPP::l_flags);
        lua_register(L, "build",    GPP::l_build);
    }
}