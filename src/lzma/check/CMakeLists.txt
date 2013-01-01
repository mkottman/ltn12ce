set ( SRC
	check.c
	crc32_fast.c
	crc32_table.c
	crc32_tablegen.c
	crc64_fast.c
	crc64_table.c
	crc64_tablegen.c
	sha256.c
)

set ( HDR
	check.h
	crc32_table_be.h
	crc32_table_le.h
	crc64_table_be.h
	crc64_table_le.h
	crc_macros.h
)

add_library ( lzma_check STATIC ${SRC} ${HDR} )