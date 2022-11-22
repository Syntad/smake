function smake.install()
    print('INSTALL')
end

function smake.build()
    --[[ standard('c++2a')
    input('main.cpp', 'src/*.cpp')
    include('include')
    include('lua', 'lua', 'lua')
    output('out/smake.exe')
    build() ]]
    print(platform.name)
end