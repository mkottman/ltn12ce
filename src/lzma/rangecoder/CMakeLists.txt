set ( SRC
	price_table.c
	price_tablegen.c
)

set ( HDR
	price.h
	range_common.h
	range_decoder.h
	range_encoder.h
)

add_library ( lzma_rangecoder STATIC ${SRC} ${HDR} )