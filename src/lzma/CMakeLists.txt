add_definitions ( "-std=gnu99" "-DHAVE_CONFIG_H")

include_directories ( . tuklib common check delta lz lzma rangecoder simple )

# tuklib

set ( TUKLIB_SRC
	tuklib/tuklib_cpucores.c
	tuklib/tuklib_exit.c
	tuklib/tuklib_mbstr_fw.c
	tuklib/tuklib_mbstr_width.c
	tuklib/tuklib_open_stdxxx.c
	tuklib/tuklib_physmem.c
	tuklib/tuklib_progname.c
)
set ( TUKLIB_HDR
	tuklib/mythread.h
	tuklib/sysdefs.h
	tuklib/tuklib_common.h
	tuklib/tuklib_config.h
	tuklib/tuklib_cpucores.h
	tuklib/tuklib_exit.h
	tuklib/tuklib_gettext.h
	tuklib/tuklib_integer.h
	tuklib/tuklib_mbstr.h
	tuklib/tuklib_open_stdxxx.h
	tuklib/tuklib_physmem.h
	tuklib/tuklib_progname.h
)

# common
set ( COMMON_SRC
	common/alone_encoder.c
	common/alone_decoder.c
	common/auto_decoder.c
	common/block_buffer_decoder.c
	common/block_buffer_encoder.c
	common/block_decoder.c
	common/block_encoder.c
	common/block_header_decoder.c
	common/block_header_encoder.c
	common/block_util.c
	common/common.c
	common/easy_buffer_encoder.c
	common/easy_decoder_memusage.c
	common/easy_encoder.c
	common/easy_encoder_memusage.c
	common/easy_preset.c
	common/filter_buffer_decoder.c
	common/filter_buffer_encoder.c
	common/filter_common.c
	common/filter_decoder.c
	common/filter_encoder.c
	common/filter_flags_decoder.c
	common/filter_flags_encoder.c
	common/hardware_physmem.c
	common/index.c
	common/index_decoder.c
	common/index_encoder.c
	common/index_hash.c
	common/stream_buffer_decoder.c
	common/stream_buffer_encoder.c
	common/stream_decoder.c
	common/stream_encoder.c
	common/stream_flags_common.c
	common/stream_flags_decoder.c
	common/stream_flags_encoder.c
	common/vli_decoder.c
	common/vli_encoder.c
	common/vli_size.c
)
set ( COMMON_HDR
	common/alone_decoder.h
	common/block_decoder.h
	common/block_encoder.h
	common/common.h
	common/easy_preset.h
	common/filter_common.h
	common/filter_decoder.h
	common/filter_encoder.h
	common/index_encoder.h
	common/index.h
	common/stream_decoder.h
	common/stream_encoder.h
	common/stream_flags_common.h
)

# check
set ( CHECK_SRC
	check/check.c
	check/crc32_fast.c
	check/crc32_table.c
	check/crc32_tablegen.c
	check/crc64_fast.c
	check/crc64_table.c
	check/crc64_tablegen.c
	check/sha256.c
)
set ( CHECK_HDR
	check/check.h
	check/crc32_table_be.h
	check/crc32_table_le.h
	check/crc64_table_be.h
	check/crc64_table_le.h
	check/crc_macros.h
)

# delta
set ( DELTA_SRC
	delta/delta_common.c
	delta/delta_decoder.c
	delta/delta_encoder.c
)
set ( DELTA_HDR
	delta/delta_common.h
	delta/delta_decoder.h
	delta/delta_encoder.h
	delta/delta_private.h
)

# lz
set ( LZ_SRC
	lz/lz_decoder.c
	lz/lz_encoder.c
	lz/lz_encoder_mf.c
)
set ( LZ_HDR
	lz/lz_decoder.h
	lz/lz_encoder.h
	lz/lz_encoder_hash.h
	lz/lz_encoder_hash_table.h
)

# lzma
set ( LZMA_SRC
	lzma/fastpos_table.c
	lzma/fastpos_tablegen.c
	lzma/lzma2_decoder.c
	lzma/lzma2_encoder.c
	lzma/lzma_decoder.c
	lzma/lzma_encoder.c
	lzma/lzma_encoder_optimum_fast.c
	lzma/lzma_encoder_optimum_normal.c
	lzma/lzma_encoder_presets.c
)
set ( LZMA_HDR
	lzma/fastpos.h
	lzma/lzma2_decoder.h
	lzma/lzma2_encoder.h
	lzma/lzma_common.h
	lzma/lzma_decoder.h
	lzma/lzma_encoder.h
	lzma/lzma_encoder_private.h
)

# simple
set ( SIMPLE_SRC
	simple/arm.c
	simple/armthumb.c
	simple/ia64.c
	simple/powerpc.c
	simple/simple_coder.c
	simple/simple_decoder.c
	simple/simple_encoder.c
	simple/sparc.c
	simple/x86.c
)
set ( SIMPLE_HDR
	simple/simple_coder.h
	simple/simple_decoder.h
	simple/simple_encoder.h
	simple/simple_private.h
)

# rangecoder
set ( RANGECODER_SRC
	rangecoder/price_table.c
	rangecoder/price_tablegen.c
)
set ( RANGECODER_HDR
	rangecoder/price.h
	rangecoder/range_common.h
	rangecoder/range_decoder.h
	rangecoder/range_encoder.h
)

add_library ( lzma STATIC
	${TUKLIB_HDR} ${TUKLIB_SRC}
	${COMMON_HDR} ${COMMON_SRC}
	${CHECK_HDR} ${CHECK_SRC}
	${DELTA_HDR} ${DELTA_SRC}
	${LZ_HDR} ${LZ_SRC}
	${LZMA_HDR} ${LZMA_SRC}
	${SIMPLE_HDR} ${SIMPLE_SRC}
	${RANGECODER_HDR} ${RANGECODER_SRC}
)
