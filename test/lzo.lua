local core = require 'ltn12ce.core'

assert(core.lzo_compress, 'no lzo_compress function')
assert(core.lzo_decompress, 'no lzo_decompress function')

local f = assert(io.open('test/lorem'))
local text = f:read('*a')

print('Original:', #text)

local comp = core.lzo_compress(text)

print('Compressed:', #comp)

assert(#text > #comp, 'compressed data is bigger than original')

local decomp = core.lzo_decompress(comp)

assert(decomp, 'lzo_decompress did not return anything')

print('Decompressed:', #decomp)

assert(text == decomp, 'decompressed data is not the same as original')