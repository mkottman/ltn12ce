set ( SRC
	tuklib_cpucores.c
	tuklib_exit.c
	tuklib_mbstr_fw.c
	tuklib_mbstr_width.c
	tuklib_open_stdxxx.c
	tuklib_physmem.c
	tuklib_progname.c
)

set ( HDR
	mythread.h
	sysdefs.h
	tuklib_common.h
	tuklib_config.h
	tuklib_cpucores.h
	tuklib_exit.h
	tuklib_gettext.h
	tuklib_integer.h
	tuklib_mbstr.h
	tuklib_open_stdxxx.h
	tuklib_physmem.h
	tuklib_progname.h
)

add_library ( lzma_tuklib STATIC ${SRC} ${HDR} )