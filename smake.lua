---@diagnostic disable: need-check-nil
local import = import('smake/libraryInstaller')
---@type enhancedSpinner
local spinner = import('smake/enhancedSpinner')
import('smake/gpp', true)
import('smake/dependencyInstaller', true)
import('smake/dependencyIncluder', true)

spinner.SetOptions({ symbols = 'clock' })

function smake.build()
    spinner.Call(InstallDependencies, 'Installing Dependencies', '✅ Installed Dependencies', 'lua', 'rapidjson')

    standard('c++2a')
    include('include')
    IncludeDependencies('lua', 'rapidjson', 'curl', 'openssl')

    input('main.cpp', 'src/*.cpp')
    output(platform.is_windows and 'smake.exe' or 'smake.o')

    if platform.is_windows then
        flags('-static-libgcc -static-libstdc++ -luuid -lole32')
    elseif platform.is_linux then
        flags('-ldl')
    end

    spinner.Call(build, 'Building', '✅ Built')
end