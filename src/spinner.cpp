#include <spinner.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>

namespace Spinner {
    std::thread thread;
    bool running;
    std::string text;

    void hideCursor()
    {
        std::cout << "\u001b[?25l";
    }

    void showCursor()
    {
        std::cout << "\u001b[?25h";
    }

    void clearLine() {
        std::cout << "\33[2K\r";
    }

    int l_stop(lua_State* L) {
        running = false;

        if (thread.joinable()) {
            thread.join();
        }

        showCursor();
        clearLine();

        if (lua_gettop(L) > 0) {
            std::string text(luaL_checkstring(L, -1));
            lua_settop(L, 0);

            std::cout << text << std::endl;
        }

        return 0;
    }

    int l_start(lua_State* L) {
        if (running) {
            l_stop(L);
            return luaL_error(L, "Attempted to start a spinner when the previous one has not been stopped");
        }

        text = std::string(luaL_checkstring(L, -1));
        lua_pop(L, 1);

        // Get smake.spinner
        lua_getglobal(L, "smake");
        lua_pushstring(L, "spinner");
        lua_rawget(L, -2);

        // Put symbols into an array
        lua_pushstring(L, "symbols");
        lua_rawget(L, -2);

        lua_len(L, -1);
        int symbolsLen = lua_tointeger(L, -1);
        lua_pop(L, 1);

        if (!symbolsLen) {
            return 0;
        }

        std::vector<std::string> symbols;

        for (int i = 0; i < symbolsLen; i++) {
            lua_rawgeti(L, -1, i + 1);

            if (!lua_isstring(L, -1)) {
                return luaL_error(L, "smake.spinner.symbols must only consist of strings");
            }

            symbols.emplace_back(lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        lua_pop(L, 1);

        // Get interval
        lua_pushstring(L, "interval");
        lua_rawget(L, -2);

        if (!lua_isnumber(L, -1)) {
            return luaL_error(L, "smake.spinner.interval must be a number");
        }

        double interval = lua_tonumber(L, -1);
        lua_pop(L, 1);

        if (!interval) {
            return 0;
        }
        
        running = true;

        // TODO: Make this shit work
        thread = std::thread([&, symbolsLen, symbols, interval]() {
            hideCursor();

            int i = 0;

            while (running) {
                if (++i >= symbolsLen) {
                    i = 0;
                }

                std::cout << symbols[i] << " " << text << " \r";
                std::cout.flush();
                std::this_thread::sleep_for(std::chrono::duration<double, std::chrono::seconds::period>(interval));
            }
        });

        return 0;
    }

    int l_setText(lua_State* L) {
        text = std::string(luaL_checkstring(L, -1));
        lua_pop(L, 1);

        return 0;
    }

    const luaL_Reg lib[] {
        { "start", Spinner::l_start },
        { "stop", Spinner::l_stop },
        { "setText", Spinner::l_setText },
        { NULL, NULL }
    };

    void Register(lua_State* L) {
        lua_getglobal(L, "smake");

        lua_pushstring(L, "spinner");
        luaL_newlib(L, lib);
        lua_rawset(L, -3);
    }
}