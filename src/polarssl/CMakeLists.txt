set ( POLAR_SRCS
     aes.c
     arc4.c
     blowfish.c
     camellia.c
     cipher.c
     cipher_wrap.c
     des.c
     error.c
     gcm.c
     md.c
     md_wrap.c
     md2.c
     md4.c
     md5.c
     padlock.c
     sha1.c
     sha2.c
     sha4.c
     version.c
     xtea.c
)

add_library ( polarssl STATIC ${POLAR_SRCS} )