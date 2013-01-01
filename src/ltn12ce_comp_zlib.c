#include "ltn12ce_common.h"

#include "zlib.h"

#include <stdio.h>
#include <string.h>

#define META_ZLIB "ltn12ce_zlib"

#define AUTODETECT_GZIP 32

#define ISCOMPRESSING(strm) (*((char*)(strm + 1)))
#define BUFFER(strm) ((char*)(strm + 1) + 1)

int zlib_error(lua_State* L, int err)
{
    switch (err) {
        case Z_ERRNO:
            return luaL_error(L, "ERRNO");
        case Z_STREAM_ERROR:
            return luaL_error(L, "Invalid parameters or stream inconsistent");
        case Z_DATA_ERROR:
            return luaL_error(L, "DATA_ERROR");
        case Z_MEM_ERROR:
            return luaL_error(L, "MEM_ERROR");
        case Z_BUF_ERROR:
            return luaL_error(L, "BUF_ERROR");
        case Z_VERSION_ERROR:
            return luaL_error(L, "VERSION_ERROR");
    }
    return 0;
}


int lzlib_gc(lua_State* L)
{
    z_stream* strm = luaL_checkudata(L, 1, META_ZLIB);
    if (ISCOMPRESSING(strm)) {
    }
    else {
    }
    return 0;
}

int lzlib_update(lua_State* L)
{
    z_stream* strm = luaL_checkudata(L, 1, META_ZLIB);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    int ret = Z_OK;
    char* buf = BUFFER(strm);
    luaL_Buffer luabuf;

    if (len == 0) {
        /* do not update with zero length data */
        LOG("[UPDATE] skipping zero data\n");
        lua_pushliteral(L, "");
        return 1;
    }

    luaL_buffinit(L, &luabuf);

    strm->avail_in = len;
    strm->next_in = (char*) data;

    LOG("[UPDATE] start len: %ld, buf: %d in: %u\n", len, BUFFER_SIZE, strm->avail_in);

    do {
        strm->avail_out = BUFFER_SIZE;
        strm->next_out = buf;

        if (ISCOMPRESSING(strm))
            ret = deflate(strm, Z_NO_FLUSH);
        else
            ret = inflate(strm, Z_NO_FLUSH);


        LOG("  [UPDATE] flushing buffer in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (ret < 0)
            return zlib_error(L, ret);

        luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    } while (strm->avail_out == 0);

    LOG(" [UPDATE] end in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_pushresult(&luabuf);
    return 1;
}

int lzlib_finish(lua_State* L)
{
    z_stream* strm = luaL_checkudata(L, 1, META_ZLIB);
    int ret;
    char* buf = BUFFER(strm);
    luaL_Buffer luabuf;

    luaL_buffinit(L, &luabuf);

    strm->avail_in = 0;
    strm->next_in = NULL;

    LOG("[FINISH] start\n");

    do {
        strm->avail_out = BUFFER_SIZE;
        strm->next_out = buf;

        if (ISCOMPRESSING(strm))
            ret = deflate(strm, Z_FINISH);
        else
            ret = inflate(strm, Z_FINISH);

        LOG("  [FINISH] code in: %u, out: %u (%u), ret: %d\n",
            strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (ret < 0)
            return zlib_error(L, ret);

        luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    } while (strm->avail_out == 0 && ret != Z_STREAM_END);

    LOG(" [FINISH] end buf: %d in: %u, out: %u (%u), ret: %d\n",
        BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_pushresult(&luabuf);
    return 1;
}

static z_stream* createstream(lua_State* L)
{
    z_stream* strm = lua_newuserdata(L, sizeof(z_stream) + 1 + BUFFER_SIZE);
    memset(strm, 0, sizeof(z_stream));
    char* buf = BUFFER(strm);
    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;
    return strm;
}

int lzlib(lua_State* L)
{
    const char* options[] = { "compress", "decompress", NULL };
    int opt = luaL_checkoption(L, 1, NULL, options);
    int err;

    if (opt == 0) {
        int preset = luaL_optinteger(L, 2, 6);

        z_stream* strm = createstream(L);
        err = deflateInit(strm, preset);
        ISCOMPRESSING(strm) = 1;
    }
    else {
        z_stream* strm = createstream(L);
        err = inflateInit2(strm, AUTODETECT_GZIP + MAX_WBITS);
        ISCOMPRESSING(strm) = 0;
    }

    if (err != Z_OK) {
        return zlib_error(L, err);
    }

    luaL_getmetatable(L, META_ZLIB);
    lua_setmetatable(L, -2);
    return 1;
}


static luaL_Reg zlib_functions[] = {
    {"__gc", lzlib_gc},
    {"update", lzlib_update},
    {"finish", lzlib_finish},
    {NULL, NULL},
};

static luaL_Reg module_functions[] = {
    {"zlib", lzlib},
    {NULL, NULL},
};

void init_comp_zlib(lua_State* L)
{
    luaL_register(L, NULL, module_functions);
    createmeta(L, META_ZLIB, zlib_functions);
}
