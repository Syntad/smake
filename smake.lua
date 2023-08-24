---@diagnostic disable: need-check-nil
local import = import('smake/libraryInstaller')
local spinner = import('smake/enhancedSpinner')
import('smake/gpp', true)
import('smake/dependencyInstaller', true)

spinner.SetOptions({ symbols = 'clock' })

local function installDependencies()
    InstallDependency('lua')
    InstallDependency('rapidjson')
end

function smake.build()
    spinner.Start('Installing Dependencies')
    installDependencies()
    spinner.Stop('✅ Installed Dependencies')

    -- options
    standard('c++2a')

    -- includes
    include('include')

    -- dependencies
    include('dependencies/lua/include', 'dependencies/lua/lib', 'lua')
    include('dependencies/rapidjson/include')

    -- build files
    input('main.cpp', 'src/*.cpp')
    output(platform.is_windows and 'smake.exe' or 'smake.o')

    -- flags
    if platform.is_windows then
        flags('-static-libgcc -static-libstdc++ -luuid -lole32')
    elseif platform.is_linux then
        flags('-ldl')
    end

    spinner.Start('Building')
    build()
    spinner.Stop('✅ Built')
end