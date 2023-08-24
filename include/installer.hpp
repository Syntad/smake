#pragma once

#include <filesystem>
namespace fs = std::filesystem;

namespace Installer {
    extern fs::path APP_DATA;
    extern fs::path CONFIGURATION_PATH;
    extern fs::path GLOBAL_PLUGINS_DIRECTORY;
    
    extern void Init();
    extern void InstallPlugins();
    extern bool CheckForUpdates();
    extern void Update();
};