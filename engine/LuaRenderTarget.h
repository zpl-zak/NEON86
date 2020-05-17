#pragma once

#include "system.h"
#include "RenderTarget.h"

#include <lua/lua.hpp>

static INT rtt_new(lua_State* L)
{
    UINT w = 0, h = 0;
    CRenderTarget** rtt = (CRenderTarget**)lua_newuserdata(L, sizeof(CRenderTarget*));

    if (lua_gettop(L) >= 2)
    {
        BOOL depth = FALSE;
        w = (UINT)luaL_checkinteger(L, 1);
        h = (UINT)luaL_checkinteger(L, 2);

        if (lua_gettop(L) >= 3)
            depth = (BOOL)lua_toboolean(L, 3);

        *rtt = new CRenderTarget(w, h, depth);
    } else {
        *rtt = new CRenderTarget();
    }

    luaL_setmetatable(L, L_RENDERTARGET);
    return (*rtt)->GetSurfaceHandle() != NULL;
}

static INT rtt_gethandle(lua_State* L)
{
    CRenderTarget* rtt  = *(CRenderTarget**)luaL_checkudata(L, 1, L_RENDERTARGET);

    lua_pushlightuserdata(L, (VOID*)rtt->GetTextureHandle());
    return 1;
}

static INT rtt_delete(lua_State* L)
{
    CRenderTarget* rtt = (CRenderTarget*)luaL_checkudata(L, 1, L_RENDERTARGET);

    rtt->Release();
    return 0;
}

static INT rtt_bind(lua_State* L)
{
    CRenderTarget* rtt = (CRenderTarget*)luaL_checkudata(L, 1, L_RENDERTARGET);

    rtt->Bind();
    return 0;
}

static VOID LuaRenderTarget_register(lua_State* L)
{
    lua_register(L, L_RENDERTARGET, rtt_new);
    luaL_newmetatable(L, L_RENDERTARGET);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("getHandle", rtt_gethandle);
    REGC("bind", rtt_bind);
    REGC("__gc", rtt_delete);

    lua_pop(L, 1);
}
