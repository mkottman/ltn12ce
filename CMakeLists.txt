# Copyright (C) 2013 LuaDist.
# Submitted by Michal Kottman
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( ltn12e C )
cmake_minimum_required ( VERSION 2.6 )
include ( cmake/dist.cmake )
include ( cmake/lua.cmake )

find_package ( Lua REQUIRED )

option ( BUILD_CRYPTO "Enable encryption/hashing through PolarSSL" ON )
option ( BUILD_BZIP2 "Enable Bzip2 compression" ON )
option ( BUILD_LZMA "Enable LZMA compression" ON )
option ( BUILD_LZO "Enable LZO compression" ON )
option ( BUILD_ZLIB "Enable Zlib compression" ON )

include_directories ( include )
include_directories ( include/bzip2 )
include_directories ( include/minilzo )
include_directories ( include/zlib )

add_subdirectory ( src )

file ( GLOB TESTS test/*.lua )
add_custom_target ( tests SOURCES ${TESTS} )
