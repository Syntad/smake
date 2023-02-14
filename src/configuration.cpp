#include <iostream>

#include <configuration.hpp>
#include <fstream>
#include <sstream>
#include <simdjson.h>
#include <filesystem>
namespace ondemand = simdjson::ondemand;
namespace dom = simdjson::dom;
namespace fs = std::filesystem;

namespace Configuration {
    #pragma region Variables

    #ifdef _WIN32
        #include <Windows.h>
        #include <shlobj_core.h>

        LPWSTR pProgramDataPath;

        if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &pProgramDataPath)))
        {
            throw std::runtime_error("Could not get App Data path");
        }

        const fs::path Configuration::APP_DATA = fs::path(szPath) / "smake";
        const fs::path Configuration::CONFIGURATION_PATH = APP_DATA / "config.json";
        fs::path Configuration::globalPluginsDirectory = APP_DATA / "plugins";
    #else
        const fs::path APP_DATA = fs::path(getenv("HOME")) / ".smake";
        const fs::path CONFIGURATION_PATH = APP_DATA / "smake.json";
        fs::path globalPluginsDirectory = APP_DATA / "plugins";
    #endif

    const std::filesystem::path RELATIVE_CONFIGURATION_FILE = "smake.json";
    std::filesystem::path relativePluginsDirectory = "./plugins";
    std::string smakeFileName = "smake.lua";

    #pragma endregion

    void loadGlobal() {
        if (!fs::exists(CONFIGURATION_PATH)) {
            return;
        }

        ondemand::parser parser;
        simdjson::padded_string json = simdjson::padded_string::load(CONFIGURATION_PATH.string());
        ondemand::document config = parser.iterate(json);

        std::string_view pluginsDirectory, smakeFileName;
        
        if (config["pluginsDirectory"].get(pluginsDirectory) != simdjson::NO_SUCH_FIELD) {
            Configuration::globalPluginsDirectory = fs::path(pluginsDirectory);
        }

        if (config["smakeFileName"].get(smakeFileName) == simdjson::SUCCESS) {
            Configuration::smakeFileName = std::string(smakeFileName);
        }
    }

    void loadLocal() {
        if (!fs::exists(RELATIVE_CONFIGURATION_FILE)) {
            return;
        }

        ondemand::parser parser;
        simdjson::padded_string json = simdjson::padded_string::load(RELATIVE_CONFIGURATION_FILE.string());
        ondemand::document config = parser.iterate(json);

        std::string_view pluginsDirectory, smakeFileName;
        
        if (config["pluginsDirectory"].get(pluginsDirectory) == simdjson::SUCCESS) {
            Configuration::relativePluginsDirectory = fs::path(pluginsDirectory);
        }
    
        if (config["smakeFileName"].get(smakeFileName) == simdjson::SUCCESS) {
            Configuration::smakeFileName = std::string(smakeFileName);
        }
    }

    void Load() {
        loadGlobal();
        loadLocal();       
    }
}