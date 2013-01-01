set ( SRC
	alone_decoder.c
	alone_encoder.c
	auto_decoder.c
	block_buffer_decoder.c
	block_buffer_encoder.c
	block_decoder.c
	block_encoder.c
	block_header_decoder.c
	block_header_encoder.c
	block_util.c
	common.c
	easy_buffer_encoder.c
	easy_decoder_memusage.c
	easy_encoder.c
	easy_encoder_memusage.c
	easy_preset.c
	filter_buffer_decoder.c
	filter_buffer_encoder.c
	filter_common.c
	filter_decoder.c
	filter_encoder.c
	filter_flags_decoder.c
	filter_flags_encoder.c
	hardware_physmem.c
	index.c
	index_decoder.c
	index_encoder.c
	index_hash.c
	stream_buffer_decoder.c
	stream_buffer_encoder.c
	stream_decoder.c
	stream_encoder.c
	stream_flags_common.c
	stream_flags_decoder.c
	stream_flags_encoder.c
	vli_decoder.c
	vli_encoder.c
	vli_size.c
)

set ( HDR
	alone_decoder.h
	block_decoder.h
	block_encoder.h
	common.h
	easy_preset.h
	filter_common.h
	filter_decoder.h
	filter_encoder.h
	index_encoder.h
	index.h
	stream_decoder.h
	stream_encoder.h
	stream_flags_common.h
)

add_library ( lzma_common STATIC ${SRC} ${HDR} )