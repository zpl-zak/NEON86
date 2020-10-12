#pragma once

#include "system.h"
#include "Effect.h"

#include <lua/lua.hpp>

auto effect_new(lua_State* L) -> int
{
    const auto* const effectPath = luaL_checkstring(L, 1);
    const auto debugMode = lua_toboolean(L, 2) == 1;

    auto* const fx = static_cast<CEffect**>(lua_newuserdata(L, sizeof(CEffect*)));
    *fx = new CEffect();
    (*fx)->LoadEffect(effectPath, debugMode);

    luaL_setmetatable(L, L_EFFECT);
    return 1;
}

static auto effect_begin(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const technique = luaL_checkstring(L, 2);

    lua_pushinteger(L, fx->Begin(technique));
    return 1;
}

static auto effect_end(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));

    lua_pushinteger(L, fx->End());
    return 1;
}

static auto effect_beginpass(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    unsigned int pass = -1;

    if (lua_isinteger(L, 2) == 0)
    {
        const auto* const passName = luaL_checkstring(L, 2);
        pass = fx->FindPass(passName);
    }
    else
    {
        pass = static_cast<unsigned int>(luaL_checkinteger(L, 2)) - 1;
    }

    lua_pushinteger(L, fx->BeginPass(pass));
    return 1;
}

static auto effect_endpass(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));

    lua_pushinteger(L, fx->EndPass());
    return 1;
}

static auto effect_commit(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));

    lua_pushinteger(L, fx->CommitChanges());
    return 1;
}

static auto effect_setbool(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    const auto value = static_cast<float>(lua_toboolean(L, 3));

    fx->SetBool(name, value == 1.0F);
    return 0;
}

static auto effect_setfloat(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    const auto value = static_cast<float>(luaL_checknumber(L, 3));

    fx->SetFloat(name, value);
    return 0;
}

static auto effect_setmatrix(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    auto* const value = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 3, L_MATRIX));

    fx->SetMatrix(name, *value);
    return 0;
}

static auto effect_setvector3(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    auto* const value = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 3, L_VECTOR));

    fx->SetVector3(name, *value);
    return 0;
}

static auto effect_setinteger(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    const auto value = static_cast<DWORD>(luaL_checkinteger(L, 3));

    fx->SetInteger(name, value);
    return 0;
}

static auto effect_setlight(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);
    CLight* value = nullptr;

    if (lua_gettop(L) == 3 && (luaL_testudata(L, 3, L_LIGHT) != nullptr))
    {
        value = *static_cast<CLight**>(lua_touserdata(L, 3));
    }

    fx->SetLight(name, value);
    return 0;
}

static auto effect_setvector4(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);

    if (lua_gettop(L) == 4)
    {
        auto* const value = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 3, L_VECTOR));
        const auto value2 = static_cast<float>(lua_tonumber(L, 4));
        fx->SetVector4(name, D3DXVECTOR4(*value, value2));
    }
    else
    {
        auto* const value = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        fx->SetVector4(name, *value);
    }

    return 0;
}

static auto effect_settexture(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));
    const auto* const name = luaL_checkstring(L, 2);

    if (luaL_testudata(L, 3, L_RENDERTARGET) != nullptr)
    {
        auto* rtt = *static_cast<CRenderTarget**>(lua_touserdata(L, 3));

        fx->SetTexture(name, rtt->GetTextureHandle());
    }
    else if (luaL_testudata(L, 3, L_MATERIAL) != nullptr)
    {
        unsigned int slot = TEXTURESLOT_ALBEDO;
        auto* mat = *static_cast<CMaterial**>(lua_touserdata(L, 3));

        if (lua_gettop(L) == 4)
        {
            slot = static_cast<unsigned int>(luaL_checkinteger(L, 4)) - 1;
        }

        fx->SetTexture(name, mat->GetTextureHandle(slot));
    }
    else
    {
        auto* const handle = static_cast<LPDIRECT3DTEXTURE9>(lua_touserdata(L, 3));
        fx->SetTexture(name, handle);
    }

    return 0;
}

static auto effect_delete(lua_State* L) -> int
{
    auto* fx = *static_cast<CEffect**>(luaL_checkudata(L, 1, L_EFFECT));

    fx->Release();
    return 0;
}

static void LuaEffect_register(lua_State* L)
{
    lua_register(L, L_EFFECT, effect_new);
    luaL_newmetatable(L, L_EFFECT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("start", effect_begin);
    REGC("finish", effect_end);

    REGC("begin", effect_begin);
    REGC("flush", effect_end);
    REGC("done", effect_end);

    REGC("beginPass", effect_beginpass);
    REGC("endPass", effect_endpass);
    REGC("commit", effect_commit);

    REGC("setBool", effect_setbool);
    REGC("setFloat", effect_setfloat);
    REGC("setInt", effect_setinteger);
    REGC("setMatrix", effect_setmatrix);
    REGC("setVector3", effect_setvector3);
    REGC("setVector4", effect_setvector4);
    REGC("setTexture", effect_settexture);
    REGC("setLight", effect_setlight);

    REGC("__gc", effect_delete);

    lua_pop(L, 1);
}
