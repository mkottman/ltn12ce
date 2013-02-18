#include "ltn12ce_common.h"

#include "polarssl/md.h"
#include "polarssl/error.h"

#define META_DIGEST "ltn12ce_digest"

static int ldigest_error(lua_State *L, int err)
{
    char buffer[256];
    error_strerror(err, buffer, sizeof(buffer));
    lua_pushstring(L, buffer);
    return lua_error(L);
}

static int ldigest_create(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    const md_info_t *info = md_info_from_string(name);
    int err = 0;

    if (info == NULL) {
        lua_pushliteral(L, "No such digest: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        int top = lua_gettop(L);
        return luaL_argerror(L, 1, lua_tostring(L, top));
    }

    md_context_t *ctx = (md_context_t*) lua_newuserdata(L, sizeof(md_context_t));
    err = md_init_ctx(ctx, info);
    if (err) return ldigest_error(L, err);

    err = md_starts(ctx);
    if (err) return ldigest_error(L, err);

    luaL_getmetatable(L, META_DIGEST);
    lua_setmetatable(L, -2);
    return 1;
}

static int ldigest_gc(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    md_free_ctx(ctx);
    return 0;
}

static int ldigest_name(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    lua_pushstring(L, md_get_name(ctx->md_info));
    return 1;
}

static int ldigest_size(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    lua_pushinteger(L, md_get_size(ctx->md_info));
    return 1;
}

static int ldigest_reset(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    int err = md_starts(ctx);
    if (err) return ldigest_error(L, err);
    return 0;
}

static int ldigest_update(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    size_t len = 0;
    const char *data = luaL_checklstring(L, 2, &len);
    int err = md_update(ctx, data, len);
    if (err) return ldigest_error(L, err);
    return 0;
}

static int ldigest_finish(lua_State *L)
{
    md_context_t *ctx = luaL_checkudata(L, 1, META_DIGEST);
    int buflen = md_get_size(ctx->md_info);
    char *output = lua_newuserdata(L, buflen);
    int err = md_finish(ctx, output);
    if (err) return ldigest_error(L, err);
    lua_pushlstring(L, output, buflen);
    return 1;
}


static int ldigests(lua_State *L)
{
    int i = 0;
    const int *list = md_list();
    lua_createtable(L, 0, 0);
    for (; *list; i++, list++) {
        const md_info_t *info = md_info_from_type(*list);
        lua_pushstring(L, info->name);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

static luaL_Reg cipher_functions[] = {
    {"__gc", ldigest_gc},
    {"name", ldigest_name},
    {"size", ldigest_size},
    {"reset", ldigest_reset},
    {"update", ldigest_update},
    {"finish", ldigest_finish},
    {NULL, NULL},
};

static luaL_Reg module_functions[] = {
    {"digests", ldigests},
    {"digest", ldigest_create},
    {NULL, NULL},
};

void init_digest(lua_State *L)
{
    luaL_register(L, NULL, module_functions);
    createmeta(L, META_DIGEST, cipher_functions);
}
