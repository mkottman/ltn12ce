local core = require 'ltn12ce.core'

local f = assert(io.open('test/lorem'))
local text = f:read('*a')

local tests = {
	['test/lorem.bz2'] = 'bzip2',
	['test/lorem.xz'] = 'lzma',
	['test/lorem.lzma'] = 'lzma',
	['test/lorem.gz'] = 'zlib'
}

for fn, compressor in pairs(tests) do
	local f = assert(io.open(fn))
	local filedata = f:read('*a')
	local comp = assert(core[compressor], 'no such compressor: ' .. compressor)
	local decomp = comp('decompress')
	local data = decomp:update(filedata)
	data = data .. decomp:finish()
	assert(data == text, 'decompressed data from ' .. compressor .. ' input ' .. fn .. ' does not match original')
	print('Data from', fn, #data)
end
