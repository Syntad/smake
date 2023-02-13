---@diagnostic disable: undefined-global
local gpp = include('smake/gpp')

function smake.build()
    -- options
    gpp.standard('c++2a')

    -- includes
    gpp.include('include')

    if platform.is_windows then
        gpp.flags('-static-libgcc -static-libstdc++')
    elseif platform.is_linux then
        gpp.flags('-ldl')
    end

    gpp.include('lua', 'lua', 'lua')

    -- dependencies
    gpp.include('dependencies/include')
    gpp.input('dependencies/src/*.cpp')

    -- build
    gpp.input('main.cpp', 'src/*.cpp')
    gpp.output(platform.is_windows and 'smake.exe' or 'smake')
    gpp.build()
end

function smake.install()
    run(
        'curl "https://www.lua.org/ftp/lua-5.4.4.tar.gz" -o ./lua-5.4.4.tar',
        'tar -xf ./lua-5.4.4.tar',
        'rm ./lua-5.4.4.tar',
        'mv ./lua-5.4.4/src ./lua',
        'rm -rf ./lua-5.4.4',
        'cd lua && make',
        'rm ./lua/*.c'
    )
end