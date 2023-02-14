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