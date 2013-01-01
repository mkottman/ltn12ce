set ( SRC
	fastpos_table.c
	fastpos_tablegen.c
	lzma2_decoder.c
	lzma2_encoder.c
	lzma_decoder.c
	lzma_encoder.c
	lzma_encoder_optimum_fast.c
	lzma_encoder_optimum_normal.c
	lzma_encoder_presets.c
)

set ( HDR
	fastpos.h
	lzma2_decoder.h
	lzma2_encoder.h
	lzma_common.h
	lzma_decoder.h
	lzma_encoder.h
	lzma_encoder_private.h
)

add_library ( lzma_lzma STATIC ${SRC} ${HDR} )