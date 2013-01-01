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
    init_crypto(L);
    init_digest(L);

    // compression
    init_comp_bzip2(L);
    init_comp_lzma(L);
    init_comp_minilzo(L);
    init_comp_zlib(L);

    return 1;
}
