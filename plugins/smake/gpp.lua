local settings = {
    standard = nil,
    input = {},
    include = {},
    linkPaths = {},
    linkNames = {},
    flags = {},
    output = nil
}

local function standard(std)
    settings.standard = std
end

local function input(...)
    local args = { ... }

    for _, file in next, args do
        settings.input[#settings.input + 1] = file
    end
end

local function include(include, path, name)
    if type(include) == 'table' then
        for _, include in next, include do
            settings.include[#settings.include + 1] = include
        end

        return
    end

    settings.include[#settings.include + 1] = include
    settings.linkPaths[#settings.linkPaths + 1] = path
    settings.linkNames[#settings.linkNames + 1] = name
end

local function output(file)
    settings.output = file
end

local function flags(...)
    local args = { ... }

    for _, flag in next, args do
        settings.flags[#settings.flags + 1] = flag
    end
end

local function makeCommand()
    local cmd = 'g++';

    if settings.standard then
        cmd = cmd .. ' -std=' .. settings.standard
    end

    for _, input in next, settings.input do
        cmd = cmd ..  ' ' .. input
    end

    for _, include in next, settings.include do
        cmd = cmd .. ' -I' .. include
    end

    for _, link in next, settings.linkPaths do
        cmd = cmd .. ' -L' .. link
    end

    for _, name in next, settings.linkNames do
        cmd = cmd .. ' -l' .. name;
    end

    for _, flag in next, settings.flags do
        cmd = cmd .. ' ' .. flag;
    end

    if settings.output then
        cmd = cmd .. ' -o' .. settings.output;
    end

    return cmd;
end

local function build()
    local c = os.clock()
    run(makeCommand())
    print('Built in ' .. (os.clock() - c) * 1000 .. 'ms')
end

local module = {
    standard = standard,
    input = input,
    include = include,
    output = output,
    flags = flags,
    build = build
}

function Plugin.Command()
    local file = Plugin.Arguments[1] or 'make.lua'
    local func = assert(loadfile(file), 'File does not exist')

    for i, v in next, module do
        if i ~= 'build' then
            _G[i] = v
        end
    end

    func()
    build()
end

function Plugin.Include()
    return {
        standard = standard,
        input = input,
        include = include,
        output = output,
        flags = flags,
        build = build
    }
end