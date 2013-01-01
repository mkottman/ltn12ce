#include "ltn12ce_common.h"

#include "lzma.h"

#include <stdio.h>

#define META_LZMA "ltn12ce_lzma"

#define BUFFER(strm) ((char*)(strm + 1))

extern LZMA_API(lzma_ret) lzma_easy_encoder(lzma_stream *strm, uint32_t preset, lzma_check check);
extern LZMA_API(lzma_ret) lzma_auto_decoder(lzma_stream *strm, uint64_t memlimit, uint32_t flags);

int lzma_error(lua_State *L, lzma_ret err)
{
    switch (err) {
    case LZMA_UNSUPPORTED_CHECK: return luaL_error(L, "LZMA: Unsupported check type");
    case LZMA_MEM_ERROR: return luaL_error(L, "LZMA: Cannot allocate memory, allocation failed");
    case LZMA_MEMLIMIT_ERROR: return luaL_error(L, "LZMA: Memory usage limit was reached");
    case LZMA_FORMAT_ERROR: return luaL_error(L, "LZMA: File format not recognized");
    case LZMA_OPTIONS_ERROR: return luaL_error(L, "LZMA: Invalid or unsupported options");
    case LZMA_DATA_ERROR: return luaL_error(L, "LZMA: Data is corrypt, check failed");
    case LZMA_BUF_ERROR: return luaL_error(L, "LZMA: No progress is possible");
    case LZMA_PROG_ERROR: return luaL_error(L, "LZMA: Programming error");
    }
    return 0;
}


int llzma_gc(lua_State *L)
{
    lzma_stream *strm = luaL_checkudata(L, 1, META_LZMA);
    lzma_end(strm);
    return 0;
}

int llzma_update(lua_State *L)
{
    lzma_stream *strm = luaL_checkudata(L, 1, META_LZMA);
    size_t len = 0;
    const char *data = luaL_checklstring(L, 2, &len);
    lzma_ret ret = LZMA_OK;
    char *buf = BUFFER(strm);
    luaL_Buffer luabuf;

    luaL_buffinit(L, &luabuf);

    LOG("[UPDATE] start len: %ld, buf: %d in: %ld, out: %ld (%ld)\n", len, BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out);

    strm->avail_in = len;
    strm->next_in = data;
    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;

    while (strm->avail_in > 0 && ret != LZMA_STREAM_END) {
        ret = lzma_code(strm, LZMA_RUN);

        LOG("[UPDATE] code in: %ld, out: %ld (%ld), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (strm->avail_out == 0) {
            LOG("[UPDATE] flushing buffer in: %ld, out: %ld (%ld), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);
            luaL_addlstring(&luabuf, buf, BUFFER_SIZE);
            strm->avail_out = BUFFER_SIZE;
            strm->next_out = buf;
        }

        if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
            return lzma_error(L, ret);
        }
    }

    LOG("[UPDATE] end in: %ld, out: %ld (%ld), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    luaL_pushresult(&luabuf);

    return 1;
}

int llzma_finish(lua_State *L)
{
    lzma_stream *strm = luaL_checkudata(L, 1, META_LZMA);
    lzma_ret ret;
    char *buf = BUFFER(strm);
    luaL_Buffer luabuf;

    luaL_buffinit(L, &luabuf);

    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;

    LOG("[FINISH] start buf: %d in: %ld, out: %ld (%ld), ret: %d\n",
           BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    while (1) {
        ret = lzma_code(strm, LZMA_FINISH);

        LOG("[FINISH] code buf: %d in: %ld, out: %ld (%ld), ret: %d\n",
               BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

        if (strm->avail_out == 0) {
            LOG("[FINISH] flushing buffer in: %ld, out: %ld (%ld), ret: %d\n", strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);
            luaL_addlstring(&luabuf, buf, BUFFER_SIZE);
            strm->avail_out = BUFFER_SIZE;
            strm->next_out = buf;
        }

        if (ret == LZMA_STREAM_END)
            break;

        if (ret != LZMA_OK) {
            return lzma_error(L, ret);
        }
    }

    LOG("[FINISH] end buf: %d in: %ld, out: %ld (%ld), ret: %d\n",
           BUFFER_SIZE, strm->avail_in, strm->avail_out, BUFFER_SIZE - strm->avail_out, ret);

    luaL_addlstring(&luabuf, buf, BUFFER_SIZE - strm->avail_out);
    luaL_pushresult(&luabuf);

    return 1;
}

static lzma_stream * createstream(lua_State *L)
{
    lzma_stream *strm = lua_newuserdata(L, sizeof(lzma_stream) + BUFFER_SIZE);
    char *buf = BUFFER(strm);
    lzma_stream tmp = LZMA_STREAM_INIT;
    *strm = tmp;

    strm->avail_out = BUFFER_SIZE;
    strm->next_out = buf;
    return strm;
}

int llzma(lua_State *L)
{
    const char *options[] = { "compress", "decompress", NULL };
    int opt = luaL_checkoption(L, 1, NULL, options);
    lzma_ret err;

    if (opt == 0) {
        int preset = luaL_optinteger(L, 2, LZMA_PRESET_DEFAULT);
        const char *checks[] = { "none", "crc32", "crc64", "sha256", NULL };
        lzma_check lchecks[] = { LZMA_CHECK_NONE, LZMA_CHECK_CRC32,
                                 LZMA_CHECK_CRC64, LZMA_CHECK_SHA256 };
        int check = luaL_checkoption(L, 3, "none", checks);

        lzma_stream *strm = createstream(L);
        err = lzma_easy_encoder(strm, preset, lchecks[check]);
    } else {
        lzma_stream *strm = createstream(L);
        err = lzma_auto_decoder(strm, 18446744073709551615ULL, 0);
    }

    if (err != LZMA_OK) {
        return lzma_error(L, err);
    }

    luaL_getmetatable(L, META_LZMA);
    lua_setmetatable(L, -2);
    return 1;
}


static luaL_Reg lzma_functions[] = {
    {"__gc", llzma_gc},
    {"update", llzma_update},
    {"finish", llzma_finish},
    {NULL, NULL},
};

static luaL_Reg module_functions[] = {
    {"lzma", llzma},
    {NULL, NULL},
};

void init_comp_lzma(lua_State *L)
{
    luaL_register(L, NULL, module_functions);
    createmeta(L, META_LZMA, lzma_functions);
}
