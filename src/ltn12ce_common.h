#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/*
#define DEBUG
*/

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) do {} while(0)
#endif

#define BUFFER_SIZE 4096

void createmeta(lua_State *L, const char *name, const luaL_Reg *reg);

#ifdef BUILD_CRYPTO
void init_crypto(lua_State *L);
void init_digest(lua_State *L);
#endif

#ifdef BUILD_BZIP2
void init_comp_bzip2(lua_State *L);
#endif
#ifdef BUILD_LZMA
void init_comp_lzma(lua_State *L);
#endif
#ifdef BUILD_LZO
void init_comp_minilzo(lua_State *L);
#endif
#ifdef BUILD_ZLIB
void init_comp_zlib(lua_State *L);
#endif

