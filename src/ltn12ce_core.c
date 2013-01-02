#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "ltn12ce_common.h"

void createmeta(lua_State *L, const char *name, const luaL_Reg *reg)
{
    int oldtop = lua_gettop(L);
    luaL_newmetatable(L, name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, reg);
    lua_settop(L, oldtop);
}

int luaopen_ltn12ce_core(lua_State *L) {
    lua_createtable(L, 0, 0);

    // encryption/hashing
    
#ifdef BUILD_CRYPTO
    init_crypto(L);
    init_digest(L);
#endif

    // compression

#ifdef BUILD_BZIP2
    init_comp_bzip2(L);
#endif

#ifdef BUILD_LZMA
    init_comp_lzma(L);
#endif

#ifdef BUILD_ZLIB
    init_comp_zlib(L);
#endif

    return 1;
}
