local core = require 'ltn12ce.core'
local lorem = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.'

function runthrough(filter, data)
	return filter:update(data) .. filter:finish()
end
function makekey(key, bits)
	return key .. string.rep('0', bits/8 - #key)
end

-- First, compress and then encrypt a string

local compressor = core.zlib('compress', 9)
local encrypter = core.cipher('aes-256-cbc')

local pass = makekey('12345678', 256)
encrypter:setkey(pass, 'encrypt')

local compressed = runthrough(compressor, lorem)
local encrypted = runthrough(encrypter, compressed)

-- Then, decrypt and decompress it

local decrypter = core.cipher('aes-256-cbc')
decrypter:setkey(pass, 'decrypt')

local decompressor = core.zlib('decompress')

local decrypted = runthrough(decrypter, encrypted)
local decompressed = runthrough(decompressor, decrypted)

print('Output matches original:', lorem == decompressed)