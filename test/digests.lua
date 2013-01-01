local core = require "ltn12ce.core"
local list = core.digests()
assert(type(list) == 'table', 'returned digest list not a table')
assert(#list > 0, 'digest list is empty')

for i, name in ipairs(list) do
	print(i, name)
end

assert(not pcall(core.digest, 'xxxxx'), 'successfully created an unknown digest')

assert(pcall(core.digest, 'md5'), 'failed to create known digest')

md = core.digest 'md5'

assert(md.name, 'cannot determine digest name')
assert(type(md:name()) == 'string', 'digest name not a string')

print(md:name(), md:size())

local text = 'Hello, world!'

md:update(text)
local digest = md:finish()

function string.fromhex(str)
    return (str:gsub('..', function (cc)
        return string.char(tonumber(cc, 16))
    end))
end

function string.tohex(str)
    return (str:gsub('.', function (c)
        return string.format('%02X', string.byte(c))
    end))
end

print(text, '->', digest:tohex())

assert(digest == string.fromhex('6cd3556deb0da54bca060b4c39479839'), 'digest mismatch')