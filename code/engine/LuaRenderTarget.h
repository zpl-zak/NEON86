#pragma once

#include "system.h"
#include "RenderTarget.h"

#include <lua/lua.hpp>

auto rtt_new(lua_State* L) -> int
{
    unsigned int w = 0, h = 0;
    UCHAR kind = RTKIND_COLOR;

    if (lua_gettop(L) >= 2)
    {
        w = static_cast<unsigned int>(luaL_checkinteger(L, 1));
        h = static_cast<unsigned int>(luaL_checkinteger(L, 2));

        if (lua_gettop(L) >= 3)
            kind = static_cast<UCHAR>(luaL_checkinteger(L, 3));
    }
    const auto rtt = static_cast<CRenderTarget**>(lua_newuserdata(L, sizeof(CRenderTarget*)));
    *rtt = new CRenderTarget(w, h, kind);

    luaL_setmetatable(L, L_RENDERTARGET);
    return (*rtt)->GetSurfaceHandle() != nullptr;
}

static auto rtt_gethandle(lua_State* L) -> int
{
    auto rtt = *static_cast<CRenderTarget**>(luaL_checkudata(L, 1, L_RENDERTARGET));

    lua_pushlightuserdata(L, static_cast<void*>(rtt->GetTextureHandle()));
    return 1;
}

static auto rtt_delete(lua_State* L) -> int
{
    auto rtt = *static_cast<CRenderTarget**>(luaL_checkudata(L, 1, L_RENDERTARGET));

    rtt->Release();
    return 0;
}

static auto rtt_bind(lua_State* L) -> int
{
    auto rtt = *static_cast<CRenderTarget**>(luaL_checkudata(L, 1, L_RENDERTARGET));

    rtt->Bind();
    return 0;
}

static void LuaRenderTarget$Register(lua_State* L)
{
    lua_register(L, L_RENDERTARGET, rtt_new);
    luaL_newmetatable(L, L_RENDERTARGET);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("getHandle", rtt_gethandle);
    REGC("bind", rtt_bind);
    REGC("__gc", rtt_delete);

    lua_pop(L, 1);
}
