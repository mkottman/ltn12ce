#include "ltn12ce_common.h"

#include "minilzo.h"

#define WORK_MEM_SIZE (1<<15)

static char workmem[WORK_MEM_SIZE];

static int llzo_compress(lua_State *L)
{
    size_t len = 0;
    const char *data = luaL_checklstring(L, 1, &len);
    size_t newlen = len * (1024+64) / 1024;
    char *buf = lua_newuserdata(L, newlen);
    int res = lzo1x_1_compress(data, len, buf, &newlen, workmem);

    LOG("[LZO] compress %ld -> %ld - %d\n", len, newlen, res);

    if (res < 0)
        return luaL_error(L, "LZO: error %d", res);

    lua_pushlstring(L, buf, newlen);
    return 1;
}

static int llzo_decompress(lua_State *L)
{
    size_t len = 0;
    const char *data = luaL_checklstring(L, 1, &len);
    size_t newlen = WORK_MEM_SIZE;
    int res = lzo1x_decompress_safe(data, len, workmem, &newlen, NULL);

    LOG("[LZO] decompress %ld -> %ld - %d\n", len, newlen, res);

    if (res < 0)
        return luaL_error(L, "LZO: error %d", res);

    lua_pushlstring(L, workmem, newlen);
    return 1;
}

static luaL_Reg module_functions[] = {
    {"lzo_compress", llzo_compress},
    {"lzo_decompress", llzo_decompress},
    {NULL, NULL},
};

void init_comp_minilzo(lua_State *L)
{
    lzo_init();
    luaL_register(L, NULL, module_functions);
}
