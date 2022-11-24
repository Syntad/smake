-- options
standard('c++2a')

-- includes
include('include')

if platform.is_windows then
    include('C:\\Lua\\include', '"C:\\Lua\\lib"', 'lua')
elseif platform.is_osx then
    print('TODO: mac')
elseif platform.is_linux then
    print('TODO: linux')
else
    error('Unsupported platform')
end

-- build
input('main.cpp', 'src/*.cpp')
output('out/smake.exe')
flags('-static-libgcc -static-libstdc++')
build()