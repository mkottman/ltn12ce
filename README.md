# ltn12ce

`ltn12ce` is a Lua module which provides compression, decompression, encryption, decryption and hashing using a common interface. These are provided using the following libraries:

* [bzip2](http://www.bzip.org/)
* [miniLZO](http://www.oberhumer.com/opensource/lzo/)
* [PolarSSL](https://polarssl.org/)
* [XZ Utils](http://tukaani.org/xz/)
* [zlib](http://www.zlib.net/)

The list of currently supported algorithms:

 * Compression - bzip2 (.bz2), LZO1X-1, LZMA (.lzma), LZMA2 (.xz), DEFLATE, gzip (.gz)
 * Encryption - AES, Blowfish, Camellia, DES, 3-DES
 * Hashing - MD4, MD5, SHA1, SHA-224/256/385/512

One of the design goals of this module is to have no external dependencies. This means that every dependent library code is embedded and there are no prerequisites (except for Lua). In addition with CMake, this is a very portable way to handle compressed and/or encrypted data.

This is not a module to handle compressed archives (.zip, .rar, .tar), only compressed streams.

## Installing

This module is created with [LuaDist](http://luadist.org/) in mind, so once it is in the LuaDist package repository, you can simply install it using:

    luadist install ltn12ce

## Building

It is possible to build `ltn12ce` separately from LuaDist. The build process is based on [CMake](http://www.cmake.org/). All external libraries' build processes have been converted to CMake, so there are no autotools/configure scripts necessary. It should be possible to build the module for every build system CMake supports, which includes building for Windows, Linux and Mac OS X (in case of problems, please report).

The preferred way to build `ltn12ce` is using an out-of-source directory and running `cmake` inside it:

    ltn12ce$ mkdir build; cd build
    ltn12ce/build$ cmake ..
    ltn12ce/build$ make

Installation is simply by copying the `build/src/ltn12` somewhere into your Lua `package.cpath`.

It is possible to selectively compile the libraries in case of size/memory constraints. The following CMake variables control what gets built:

 * `BUILD_CRYPTO` - encryption and hashing using PolarSSL
 * `BUILD_BZIP2` - bzip2 compression
 * `BUILD_LZMA` - LZMA and XZ compression
 * `BUILD_LZO` - LZO compression
 * `BUILD_ZLIB` - zlib compression

All flags are turned on by default.

## API

All algorithms implement a common API. An algorithm constructor creates an object, which acts as a filter that takes `string` input and returns processed `string` output. True LTN12 filter interface will be implemented later using pure Lua module.

The object (let's name it `filter`), when initialized, provides the following methods:

 * `filter:update(data : string) : string` - call this method (at least once, but maybe repeatedly) to provide additional input to the filter. `data` must be a string, but it may be an empty string (`#data == 0`). It returns a string representing additional output from this filter. Given the nature of the algorithms, which may buffer the data until something can be processed, `update` may return an empty string, but it never returns `nil`.

 * `filter:finish() : string` - after the entire input has been consumed, there may be some data which is buffered. Call this method to finish the processing and return any leftover data. This may be an empty string.

Imagine a process `input -> filter -> output`. `input` can be provided in one or multiple chunks by calls to `filter:update(chunk)` followed by `filter:finish()`. The concatenation of the results of `update()` and `finish()` calls is the entire `output` of the filter.

### ltn12.core

This module is a C binding to the embedded libraries. Here is a list of functions/objects/methods it provides. Every object implements the `update` and `finish` functions documented above, and they are not repeated here.

 * `core.ciphers() : { string }` - an array of valid cipher identifiers for `core.cipher`.
 * `core.cipher(cipherid : string) : cipher` - create a cipher, where
   `cipherid` is one of the identifiers from `core.ciphers()`. An example of
   such `cipherid` is `"AES-256-CBC"`. `cipherid` is case-insensitive. The
   operation (encryption/decryption) and keys are set using
   `cipher:setkeys()`.
 * `cipher:name() : string` - the cipher identifier of the given object (in case you forget ;-)
 * `cipher:keysize() : number` - the length of expected encryption/decryption key.
 * `cipher:setkey(key : string, operation : string)` - sets the keys and operation for the given cipher. `operation` is one of two strings `"encrypt"` and `"decrypt"`, and `key` is the binary encryption/decryption key. It must be of the length reported by `cipher:keysize()`.

 * `core.digests() : { string }` - an array of valid digest identifiers for `core.digest()`.
 * `core.digest(digestid : string) : digest` - create a digest (hash)
   algorithm using a digest identifier reported by `core.digests()`.
   `digestid` is case-insensitive.
 * `digest:name() : string` - the digest identifier.
 * `digest:size() : number` - the size of the resulting digest in bytes.
 * `digest:update(data : string) : string` - a little implementation note on
   this method - this always returns an empty string. No output is produced
   until `digest:finish()`, which returns the resulting digest.

Compression constructors (`bzip2`, `lzma`, `zlib`) have the following interface:

 * `core.compressor(operation : string, level : number) : filter` - create a (de)compressor, where `compressor` is one of `bzip2`, `lzma`, `zlib`. `operation` must be one of `"compress"` or `"decompress"`. When compressing, the second parameter represents the compression level, and is a number between `1` and `9` including.

LZO is just a block compressor (no streams), so it has a different interface (streaming will be implemented later in a `lzop`-compatible way):

 * `core.lzo_compress(data : string) : string` - compresses `data` using the LZO compressor
 * `core.lzo_decompress(data : string) : string` - decompresses `data` using the LZO compressor

## Sample

	function makekey(key, bits)
		return key .. string.rep('0', bits/8 - #key)
	end
	local compressor = core.zlib('compress', 9)
	local encrypter = core.cipher('aes-256-cbc')
	encrypter:setkey(makekey('12345678', 256), 'encrypt')

	function runthrough(f, data) return f:update(data) .. f:finish() end

	local input = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
	local compressed = runthrough(compressor, input)
	local output = runthrough(encrypter, compressed)

## License

This module is licensed using the GPLv2 license.

Copyright (C) 2013, Michal Kottman

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

For each embedded library's license, see the `docs/` folder.