local import = import('smake/libraryInstaller')
local spinner = import('smake/enhancedSpinner')
import('smake/gpp', true)
import('smake/dependencyInstaller', true)

spinner.SetOptions({ symbols = 'clock' })

local function benchmark(func, ...)
    local c = os.time()
    func(...)
    return (os.time() - c) * 1000
end


local function installDependencies()
    -- Lua
    InstallDependency('lua', function(installer)
        local folder = installer:DownloadAndUnzip('https://www.lua.org/ftp/lua-5.4.4.tar.gz')
        folder:RunIn('cd src && make' .. (platform.is_windows and ' mingw' or '') .. '> /dev/null > err.log')

        installer:MakeIncludeFolder()
        folder:MoveInclude('src/*.h')
        folder:MoveInclude('src/*.hpp')

        installer:MakeLibraryFolder()
        folder:MoveLibrary('src/*.a')

        folder:Delete()
    end)

    -- RapidJSON
    InstallDependency('rapidjson', function(installer)
        local folder = installer:DownloadAndUnzip('https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip')
        folder:MoveInclude()
        folder:Delete()
    end)
end

function smake.build()
    spinner.Start('Installing Dependencies')
    local time = benchmark(installDependencies)
    spinner.Stop('✅ Installed in ' .. time .. 'ms')

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
    output(platform.is_windows and 'smake.exe' or 'smake.o')

    spinner.Start('Building')
    local time = benchmark(os.execute, 'sleep 5')
    spinner.Stop('✅ Built in ' .. time .. 'ms')
end