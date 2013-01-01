local core = require "ltn12ce.core"
local list = core.ciphers()
assert(type(list) == 'table', 'returned cipher list not a table')
assert(#list > 0, 'cipher list is empty')

for i, name in ipairs(list) do
	print(i, name)
end

assert(not pcall(core.cipher, 'xxxxx'), 'successfully created an unknown cipher')

assert(pcall(core.cipher, 'aes-256-cbc'), 'failed to create known cipher')

c = core.cipher 'aes-256-cbc'

assert(c.name, 'cannot determine cipher name')
assert(type(c:name()) == 'string', 'cipher name not a string')

print(c:name(), c:keysize(), c:operation())

assert(not pcall(c.setkey, c, 'somekey'), 'failed to error on incorrect key length')

assert(c.ivsize, 'cannot get cipher IV size')
assert(type(c:ivsize()) == 'number', 'IV size is not a number')

print('Key size:', c:keysize())
print('IV size:', c:ivsize())

local pass = string.rep('password', 4)
local iv = string.rep('x', c:ivsize())

assert(#pass == 256/8, 'password is not 256 bits long')

print '-- Encrypting --'

c:setkey(pass, 'encrypt')
c:reset(iv)

print(c:name(), c:keysize(), c:operation())

local buf = {}
table.insert(buf, c:update('Hello, '))
table.insert(buf, c:update(' world!'))
table.insert(buf, c:finish())

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

for i=1,#buf do
	print(i, #buf[i]..':'..buf[i]:tohex())
end

print '-- Decrypting --'

c:setkey(pass, 'decrypt')
c:reset(iv)

print(c:update(table.concat(buf)))
print(c:finish())