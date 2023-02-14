---@diagnostic disable: undefined-global
local gpp = include('smake/gpp')

-- Make gpp global
for i, v in next, gpp do
    _G[i] = v
end

function smake.build()
    -- options
    standard('c++2a')

    -- includes
    include('include')

    if platform.is_windows then
        flags('-static-libgcc -static-libstdc++')
    elseif platform.is_linux then
        flags('-ldl')
    end

    include('lua', 'lua', 'lua')

    -- dependencies
    include('dependencies/include')
    input('dependencies/src/*.cpp')

    -- build
    input('main.cpp', 'src/*.cpp')
    output(platform.is_windows and 'smake.exe' or 'smake')
    build()
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