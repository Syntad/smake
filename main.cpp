#include <paths.hpp>
#include <plugins.hpp>
#include <api.hpp>
#include <configuration.hpp>
#include <iostream>
#include <fstream>
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
            std::cerr << "Plugin not found\n";
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

        if (Configuration::aliases.contains(argument)) { // Aliases override functions
            argument = Configuration::aliases[argument].c_str();
        }

        if (API::PushSmakeFunction(L, argument)) { // Check for function first to allow overriding help command
            for (int i = 2; i < argc; i++) {
                lua_pushstring(L, argv[i]);
            }

            lua_call(L, argc - 2, 0);
        }
        else if (!strcmp(argument, "help") || !strcmp(argument, "--help")) {
            std::string help = "\033[4mFunctions\033[0m";
            std::vector<std::string> names = getFunctionNames(L);

            for (const auto& name : names) {
                help += "\n- " + name;
            }

            if (Configuration::aliases.size() > 0) {
                help += "\n\n\033[4mAliases\033[0m";

                for (const auto& [alias, name] : Configuration::aliases) {
                    help += "\n- " + alias + ": " + name;
                }
            }

            std::cout << help << '\n';
        }
        else if (!strcmp(argument, "init") || !strcmp(argument, "--i")) {
            if (fs::exists(Configuration::smakeFileName)) {
                std::cout << "The file '" << Configuration::smakeFileName << "' already exists. Do you want to override it? (y/n): ";
                int response = tolower(getchar());

                if (response != 'y') {
                    std::cout << "❌ File not overridden. Exiting.\n";
                    return;
                }
            }

            std::ofstream file(Configuration::smakeFileName);

            file << "function smake.build()\n"
                    "  print('Hello Smake!')\n"
                    "end";

            file.close();

            std::cout << "✅ Successfully saved " << Configuration::smakeFileName << '\n';
        }
        else {
            std::cout << "Function 'smake." << argument << "' does not exist.";

            if (lua_getglobal(L, argument) != LUA_TNIL) {
                std::cout << " It exists in the global environment. Did you forget to add 'smake.' in front of it?";
            }

            std::cout << '\n';

            lua_pop(L, 1);
        }
    }
    else {
        API::CallSmakeFunction(L, Configuration::defaultCommand.c_str());
    }
}

int main(int argc, char *argv[]) {
    Paths::Init();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    API::Register(L);
    Configuration::Load(L);
    Plugins::Init(L, argc, argv);
    run(L, argc, argv);

    return 0;
}