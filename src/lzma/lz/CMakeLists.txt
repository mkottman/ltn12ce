set ( SRC
	lz_decoder.c
	lz_encoder.c
	lz_encoder_mf.c
)

set ( HDR
	lz_decoder.h
	lz_encoder.h
	lz_encoder_hash.h
	lz_encoder_hash_table.h
)

add_library ( lzma_lz STATIC ${SRC} ${HDR} )