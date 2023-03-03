import('smake/gpp', true)
import('smake/dependencyInstaller', true)

local function installDependencies()
    -- Lua
    InstallDependency('lua', function(installer)
        local folder = installer:DownloadAndUnzip('https://www.lua.org/ftp/lua-5.4.4.tar.gz')
        folder:RunIn('cd src && make' .. (platform.is_windows and ' mingw' or ''))

        installer:MakeIncludeFolder()
        folder:MoveInclude('src/*.h')
        folder:MoveInclude('src/*.hpp')

        installer:MakeLibraryFolder()
        folder:MoveLibrary('src/*.a')

        folder:Delete()
    end)

    -- RapidJSON
    InstallDependency('rapidjson', function(installer)
        local folder = installer:DownloadAndUnzip('https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.zip')
        folder:MoveInclude()
        folder:Delete()
    end)
end

function smake.build()
    installDependencies()

    -- options
    standard('c++2a')

    -- includes
    include('include')

    if platform.is_windows then
        flags('-static-libgcc -static-libstdc++ -luuid')
    elseif platform.is_linux then
        flags('-ldl')
    end

    -- dependencies
    include('dependencies/lua/include', 'dependencies/lua/lib', 'lua')
    include('dependencies/rapidjson/include')

    -- build
    input('main.cpp', 'src/*.cpp')
    output(platform.is_windows and 'smake.exe' or 'smake')
    build()
end