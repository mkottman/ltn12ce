#include "ltn12ce_common.h"

#include "polarssl/cipher.h"
#include "polarssl/error.h"

#define META_CIPHER "ltn12ce_cipher"

static int lcipher_error(lua_State *L, int err)
{
    char buffer[256];
    error_strerror(err, buffer, sizeof(buffer));
    lua_pushstring(L, buffer);
    return lua_error(L);
}

static int lcipher_create(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    const cipher_info_t *info = cipher_info_from_string(name);

    if (info == NULL) {
        lua_pushliteral(L, "No such cipher: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        int top = lua_gettop(L);
        return luaL_argerror(L, 1, lua_tostring(L, top));
    }

    cipher_context_t *ctx = (cipher_context_t*) lua_newuserdata(L, sizeof(cipher_context_t));
    int err = cipher_init_ctx(ctx, info);
    if (err) return lcipher_error(L, err);

    luaL_getmetatable(L, META_CIPHER);
    lua_setmetatable(L, -2);
    return 1;
}

static int lcipher_gc(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    cipher_free_ctx(ctx);
    return 0;
}

static int lcipher_name(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    lua_pushstring(L, cipher_get_name(ctx));
    return 1;
}

static int lcipher_key_size(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    lua_pushinteger(L, cipher_get_key_size(ctx));
    return 1;
}

static int lcipher_iv_size(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    lua_pushinteger(L, cipher_get_iv_size(ctx));
    return 1;
}


static int lcipher_operation(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    switch (cipher_get_operation(ctx)) {
        case POLARSSL_OPERATION_NONE: lua_pushliteral(L, "none"); break;
        case POLARSSL_ENCRYPT: lua_pushliteral(L, "encrypt"); break;
        case POLARSSL_DECRYPT: lua_pushliteral(L, "decrypt"); break;
    }
    return 1;
}

static int lcipher_setkey(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    size_t len = 0;
    const char *key = luaL_checklstring(L, 2, &len);
    const char *modes[] = { "encrypt", "decrypt", NULL };
    int mode = luaL_checkoption(L, 3, NULL, modes);
    int err = cipher_setkey(ctx, key, len * 8, mode == 0 ? POLARSSL_ENCRYPT : POLARSSL_DECRYPT );
    if (err) return lcipher_error(L, err);
    lua_pushvalue(L, 1);
    return 1;
}

static int lcipher_reset(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    size_t len = 0;
    const char *iv = luaL_checklstring(L, 2, &len);
    int err = cipher_reset(ctx, iv);
    if (err) return lcipher_error(L, err);
    lua_pushvalue(L, 1);
    return 1;
}


static int lcipher_update(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    size_t len = 0;
    const char *data = luaL_checklstring(L, 2, &len);
    char *buf = lua_newuserdata(L, len + cipher_get_block_size(ctx));
    size_t olen = 0;
    int err = cipher_update(ctx, data, len, buf, &olen);
    if (err) return lcipher_error(L, err);
    lua_pushlstring(L, buf, olen);
    return 1;
}

static int lcipher_finish(lua_State *L)
{
    cipher_context_t *ctx = luaL_checkudata(L, 1, META_CIPHER);
    char *buf = lua_newuserdata(L, cipher_get_block_size(ctx));
    size_t olen = 0;
    int err = cipher_finish(ctx, buf, &olen);
    if (err) return lcipher_error(L, err);
    lua_pushlstring(L, buf, olen);
    return 1;
}

static int lciphers(lua_State *L)
{
    int i = 0;
    const int *list = cipher_list();
    lua_createtable(L, 0, 0);
    for (; *list; i++, list++) {
        const cipher_info_t *info = cipher_info_from_type(*list);
        lua_pushstring(L, info->name);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

static luaL_Reg cipher_functions[] = {
    {"__gc", lcipher_gc},
    {"name", lcipher_name},
    {"keysize", lcipher_key_size},
    {"ivsize", lcipher_iv_size},
    {"operation", lcipher_operation},
    {"setkey", lcipher_setkey},
    {"reset", lcipher_reset},
    {"update", lcipher_update},
    {"finish", lcipher_finish},
    {NULL, NULL},
};

static luaL_Reg module_functions[] = {
    {"ciphers", lciphers},
    {"cipher", lcipher_create},
    {NULL, NULL},
};

void init_crypto(lua_State *L)
{
    luaL_register(L, NULL, module_functions);
    createmeta(L, META_CIPHER, cipher_functions);
}
