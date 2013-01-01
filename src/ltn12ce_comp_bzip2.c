#include "ltn12ce_common.h"

#include "bzlib.h"

#include <stdio.h>
#include <string.h>

#define META_BZIP2 "ltn12ce_bzip2"

#define ISCOMPRESSING(strm) (*((char*)(strm + 1)))
#define BUFFER(strm) ((char*)(strm + 1) + 1)

int bzip2_error(lua_State *L, int err)
{
    switch (err) {
    case BZ_SEQUENCE_ERROR: return luaL_error(L, "bzip2: Sequence error");
    case BZ_PARAM_ERROR: return luaL_error(L, "bzip2: Invalid parameters");
    case BZ_MEM_ERROR: return luaL_error(L, "bzip2: Failed to allocate memory");
    case BZ_DATA_ERROR: return luaL_error(L, "bzip2: Data corrupted");
    case BZ_DATA_ERROR_MAGIC: return luaL_error(L, "bzip2: Magic corrupted");
    case BZ_IO_ERROR: return luaL_error(L, "bzip2: IO error");
    case BZ_UNEXPECTED_EOF: return luaL_error(L, "bzip2: Unexpected end of input");
    case BZ_OUTBUFF_FULL: return luaL_error(L, "bzip2: Output buffer full");
    case BZ_CONFIG_ERROR: return luaL_error(L, "bzip2: Configuration error");
    }
    return 0;
}


int lbzip2_gc(lua_State *L)
{
    bz_stream *strm = luaL_checkudata(L, 1, META_BZIP2);
    if (ISCOMPRESSING(strm)) {
        BZ2_bzCompressEnd(strm);
    } else {
        BZ2_bzDecompressEnd(strm);
    }
    return 0;
}

int lbzip2_update(lua_State *L)
{
    bz_stream *strm = luaL_checkudata(L, 1, META_BZIP2);
    size_t len = 0;
    const char *data = luaL_checklstring(L, 2, &len);
    int ret = BZ_OK;
    char *buf = BUFFER(strm);
    luaL_Buffer luabuf;

    luaL_buffinit(L, &luabuf);

    LOG("[UPDATE] start len: %ld, buf: %d in: %u, out: %u (%u)\n", len, BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out);

    strm->avail_in = len;
    strm->next_in = (char*) data;
    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;

    while (strm->avail_in > 0 && ret != BZ_STREAM_END) {
        if (ISCOMPRESSING(strm)) {
            ret = BZ2_bzCompress(strm, BZ_RUN);
        } else {
            ret = BZ2_bzDecompress(strm);
        }

        LOG("  [UPDATE] in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (strm->avail_out == 0) {
            LOG("  [UPDATE] flushing buffer in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);
            luaL_addlstring(&luabuf, buf, BUFFER_SIZE);
            strm->avail_out = BUFFER_SIZE;
            strm->next_out = buf;
        }

        if (ret < 0) {
            return bzip2_error(L, ret);
        }
    }

    LOG(" [UPDATE] end in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    luaL_pushresult(&luabuf);

    return 1;
}

int lbzip2_finish(lua_State *L)
{
    bz_stream *strm = luaL_checkudata(L, 1, META_BZIP2);
    int ret;
    char *buf = BUFFER(strm);
    luaL_Buffer luabuf;

    if (!ISCOMPRESSING(strm)) {
        // everything should be already decompressed
        lua_pushliteral(L, "");
        return 1;
    }

    luaL_buffinit(L, &luabuf);

    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;

    LOG("[FINISH] start buf: %d in: %u, out: %u (%u), ret: %d\n",
           BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    while (1) {
        if (ISCOMPRESSING(strm)) {
            ret = BZ2_bzCompress(strm, BZ_FINISH);
        } else {
            ret = BZ2_bzDecompress(strm);
        }

        LOG("  [FINISH] code buf: %d in: %u, out: %u (%u), ret: %d\n",
               BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (strm->avail_out == 0) {
            LOG("  [FINISH] flushing buffer in: %u, out: %u (%u), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);
            luaL_addlstring(&luabuf, buf, BUFFER_SIZE);
            strm->avail_out = BUFFER_SIZE;
            strm->next_out = buf;
        }

        if (ret == BZ_STREAM_END)
            break;

        if (ret < 0) {
            return bzip2_error(L, ret);
        }
    }

    LOG(" [FINISH] end buf: %d in: %u, out: %u (%u), ret: %d\n",
           BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    luaL_pushresult(&luabuf);

    return 1;
}

static bz_stream * createstream(lua_State *L)
{
    bz_stream *strm = lua_newuserdata(L, sizeof(bz_stream) + 1 + BUFFER_SIZE);
    memset(strm, 0, sizeof(bz_stream));
    char *buf = BUFFER(strm);
    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;
    return strm;
}

int lbzip2(lua_State *L)
{
    const char *options[] = { "compress", "decompress", NULL };
    int opt = luaL_checkoption(L, 1, NULL, options);
    int err;

    if (opt == 0) {
        int preset = luaL_optinteger(L, 2, 6);

        bz_stream *strm = createstream(L);
        err = BZ2_bzCompressInit(strm, preset, 0, 0);
        ISCOMPRESSING(strm) = 1;
    } else {
        bz_stream *strm = createstream(L);
        err = BZ2_bzDecompressInit(strm, 0, 0);
        ISCOMPRESSING(strm) = 0;
    }

    if (err != BZ_OK) {
        return bzip2_error(L, err);
    }

    luaL_getmetatable(L, META_BZIP2);
    lua_setmetatable(L, -2);
    return 1;
}


static luaL_Reg bzip2_functions[] = {
    {"__gc", lbzip2_gc},
    {"update", lbzip2_update},
    {"finish", lbzip2_finish},
    {NULL, NULL},
};

static luaL_Reg module_functions[] = {
    {"bzip2", lbzip2},
    {NULL, NULL},
};

void init_comp_bzip2(lua_State *L)
{
    luaL_register(L, NULL, module_functions);
    createmeta(L, META_BZIP2, bzip2_functions);
}
