<<<<<<< HEAD
#include <configuration.hpp>
#include <fstream>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
=======
#include <iostream>

#include <configuration.hpp>
#include <fstream>
#include <sstream>
#include <simdjson.h>
#include <filesystem>
namespace ondemand = simdjson::ondemand;
namespace dom = simdjson::dom;
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
namespace fs = std::filesystem;

namespace Configuration {
    #pragma region Variables

<<<<<<< HEAD
    #ifdef __CYGWIN__
        #include <Windows.h>
        #include <knownfolders.h>
        #include <shlobj.h>

        fs::path APP_DATA;
        fs::path CONFIGURATION_PATH;
        fs::path globalPluginsDirectory;
    #else
        fs::path APP_DATA = fs::path(getenv("HOME")) / ".smake";
        fs::path CONFIGURATION_PATH = APP_DATA / "smake.json";
=======
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
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
        fs::path globalPluginsDirectory = APP_DATA / "plugins";
    #endif

    const std::filesystem::path RELATIVE_CONFIGURATION_FILE = "smake.json";
    std::filesystem::path relativePluginsDirectory = "./plugins";
    std::string smakeFileName = "smake.lua";

    #pragma endregion

<<<<<<< HEAD
    void parseJsonFile(rapidjson::Document& doc, const fs::path& path) {
        std::ifstream file(path);
        rapidjson::IStreamWrapper streamWrapper(file);
        doc.ParseStream(streamWrapper);
    }

=======
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
    void loadGlobal() {
        if (!fs::exists(CONFIGURATION_PATH)) {
            return;
        }

<<<<<<< HEAD
        rapidjson::Document doc;
        parseJsonFile(doc, CONFIGURATION_PATH);

        if (doc.HasMember("pluginsDirectory") && doc["pluginsDirectory"].IsString()) {
            globalPluginsDirectory = fs::path(doc["pluginsDirectory"].GetString());
        }

        if (doc.HasMember("smakeFileName") && doc["smakeFileName"].IsString()) {
            smakeFileName = fs::path(doc["smakeFileName"].GetString());
=======
        ondemand::parser parser;
        simdjson::padded_string json = simdjson::padded_string::load(CONFIGURATION_PATH.string());
        ondemand::document config = parser.iterate(json);

        std::string_view pluginsDirectory, smakeFileName;
        
        if (config["pluginsDirectory"].get(pluginsDirectory) != simdjson::NO_SUCH_FIELD) {
            Configuration::globalPluginsDirectory = fs::path(pluginsDirectory);
        }

        if (config["smakeFileName"].get(smakeFileName) == simdjson::SUCCESS) {
            Configuration::smakeFileName = std::string(smakeFileName);
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
        }
    }

    void loadLocal() {
        if (!fs::exists(RELATIVE_CONFIGURATION_FILE)) {
            return;
        }

<<<<<<< HEAD
        rapidjson::Document doc;
        parseJsonFile(doc, RELATIVE_CONFIGURATION_FILE);

        if (doc.HasMember("pluginsDirectory") && doc["pluginsDirectory"].IsString()) {
            relativePluginsDirectory = fs::path(doc["pluginsDirectory"].GetString());
        }
    
        if (doc.HasMember("smakeFileName") && doc["smakeFileName"].IsString()) {
            smakeFileName = fs::path(doc["smakeFileName"].GetString());
=======
        ondemand::parser parser;
        simdjson::padded_string json = simdjson::padded_string::load(RELATIVE_CONFIGURATION_FILE.string());
        ondemand::document config = parser.iterate(json);

        std::string_view pluginsDirectory, smakeFileName;
        
        if (config["pluginsDirectory"].get(pluginsDirectory) == simdjson::SUCCESS) {
            Configuration::relativePluginsDirectory = fs::path(pluginsDirectory);
        }
    
        if (config["smakeFileName"].get(smakeFileName) == simdjson::SUCCESS) {
            Configuration::smakeFileName = std::string(smakeFileName);
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
        }
    }

    void Load() {
<<<<<<< HEAD
        #if __CYGWIN__
            LPWSTR programDataPath;

            if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &programDataPath)))
            {
                throw std::runtime_error("Could not get App Data path");
            }

            Configuration::APP_DATA = fs::path(programDataPath) / "Syntad/Smake";
            Configuration::CONFIGURATION_PATH = APP_DATA / "config.json";
            Configuration::globalPluginsDirectory = APP_DATA / "plugins";
        #endif

=======
>>>>>>> 841b0daed6149a5cd1f5e5c00d4a939a2eea8c5b
        loadGlobal();
        loadLocal();       
    }
}