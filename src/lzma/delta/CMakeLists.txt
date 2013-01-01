set ( SRC
	delta_common.c
	delta_decoder.c
	delta_encoder.c
)

set ( HDR
	delta_common.h
	delta_decoder.h
	delta_encoder.h
	delta_private.h
)

add_library ( lzma_delta STATIC ${SRC} ${HDR} )