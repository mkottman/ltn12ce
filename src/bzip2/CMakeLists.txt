add_definitions ( -D_FILE_OFFSET_BITS=64 )

set ( BIPZ2_SRCS
	blocksort.c huffman.c crctable.c randtable.c compress.c decompress.c bzlib.c
	bzlib_private.h
)
add_library ( bzip2 STATIC ${BIPZ2_SRCS} libbz2.def )
