---@diagnostic disable: need-check-nil
local import = import('smake/libraryInstaller')
---@type enhancedSpinner
local spinner = import('smake/enhancedSpinner')
local gppCompiler = import('smake/gpp')
local ninja = import('smake/ninja')
import('smake/dependencyInstaller', true)
import('smake/dependencyIncluder', true)

spinner.SetOptions({ symbols = 'clock' })

function smake.install()
    InstallDependency('libarchive', function(installer)
        local folder = installer:DownloadAndUntar('https://github.com/libarchive/libarchive/releases/download/v3.7.1/libarchive-3.7.1.tar.gz')
        folder:RunIn('perl configure && make')
        folder:MoveHeaders('libarchive')
        folder:Copy('.libs/libarchive.so', installer:MakeLibraryFolder())
    end)
end

function smake.build()
    local ninjaGen = ninja.fromGlobalCompiler()
    spinner.Call(InstallDependencies, 'Installing Dependencies', '✅ Installed Dependencies', 'lua', 'rapidjson')

    standard('c++2a')
    include('include')
    IncludeDependencies('lua', 'rapidjson', 'curl', 'openssl')

    input('main.cpp', 'src/*.cpp')
    output(platform.is_windows and 'smake.exe' or 'smake.o')

    if platform.is_windows then
        flags('-static-libgcc -static-libstdc++')
        link { 'uuid', 'ole32' }
    elseif platform.is_linux then
        link { 'dl' }
    end

    spinner.Call(ninjaGen.generateBuildFile, 'Creating build file', '✅ Created Ninja file', ninjaGen, 'build')
    spinner.Call(run, 'Building', '✅ Built', 'ninja')
end