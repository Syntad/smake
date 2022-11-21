print('args', ...)

--[[
-- install
if is_windows then
    -- run('echo a', 'echo b')
end

-- build
input('main.cpp', 'src/*.cpp')
include('include')
include('lua', 'lua', 'lua')
output(is_windows and 'test.exe' or 'test.o')
build() ]]