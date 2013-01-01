add_definitions ( -fPIC )

set ( SOURCES ltn12ce_core.c )
set ( LIBRARIES )

if ( BUILD_CRYPTO )
    list ( APPEND SOURCES ltn12ce_crypto.c ltn12ce_digest.c )
    list ( APPEND LIBRARIES polarssl )
    add_definitions ( -DBUILD_CRYPTO )
    add_subdirectory ( polarssl )
endif ( BUILD_CRYPTO )

if ( BUILD_BZIP2 )
    list ( APPEND SOURCES ltn12ce_comp_bzip2.c )
    list ( APPEND LIBRARIES bzip2 )
    add_definitions ( -DBUILD_BZIP2 )
    add_subdirectory ( bzip2 )
endif ( BUILD_BZIP2 )

if ( BUILD_LZMA )
    list ( APPEND SOURCES ltn12ce_comp_lzma.c )
    list ( APPEND LIBRARIES lzma )
    add_definitions ( -DBUILD_LZMA )
    add_subdirectory ( lzma )
endif ( BUILD_LZMA )


if ( BUILD_LZO )
    list ( APPEND SOURCES ltn12ce_comp_minilzo.c )
    list ( APPEND LIBRARIES minilzo )
    add_definitions ( -DBUILD_LZO )
    add_subdirectory ( minilzo )
endif ( BUILD_LZO )

if ( BUILD_ZLIB )
    list ( APPEND SOURCES ltn12ce_comp_zlib.c )
    list ( APPEND LIBRARIES zlib )
    add_definitions ( -DBUILD_ZLIB )
    add_subdirectory ( zlib )
endif ( BUILD_ZLIB )

file (GLOB_RECURSE HEADERS ../include/*.h)
set ( HEADERS ltn12ce_common.h ${HEADERS} )

install_lua_module ( ltn12ce.core ${SOURCES} ${HEADERS} LINK ${LIBRARIES} )
