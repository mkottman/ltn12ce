local core = require "ltn12ce.core"

io.stdout:setvbuf'no'

assert(core.zlib, 'no zlib ltn12ce.core')

assert(not pcall(core.zlib), 'expecting error')

local c = core.zlib('compress', 9)

local f = assert(io.open('test/lorem'))
local text = f:read('*a')

local append = table.insert

local alltext = ''

local compressed = {}
for i=1,10 do
    alltext = alltext .. text
    local p = assert(c:update(text), 'compress update produced no output')
    append(compressed, p)
end
local f = assert(c:finish(), 'compress finish produced no output')
append(compressed, f)

print('Original text:', #alltext)
print('Compressed:', #table.concat(compressed))

local d = assert(core.zlib('decompress'), 'failed to start decompression')

local decompressed = {}
for i=1,#compressed do
    local c = compressed[i]
    local p = assert(d:update(c), 'decompress update produced no output')
    append(decompressed, p)
end
local f = assert(d:finish(), 'decompress finish produced no output')
append(decompressed, f)

decompressed = table.concat(decompressed)

print('Decompressed:', #decompressed)

assert(decompressed == alltext, 'decompressed and original differ')
