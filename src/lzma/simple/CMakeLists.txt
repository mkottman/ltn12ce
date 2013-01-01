set ( SRC
	arm.c
	armthumb.c
	ia64.c
	powerpc.c
	simple_coder.c
	simple_decoder.c
	simple_encoder.c
	sparc.c
	x86.c
)

set ( HDR
	simple_coder.h
	simple_decoder.h
	simple_encoder.h
	simple_private.h
)

add_library ( lzma_simple STATIC ${SRC} ${HDR} )