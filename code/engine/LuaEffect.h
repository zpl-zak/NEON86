#pragma once

#include "system.h"
#include "Effect.h"

#include <lua/lua.hpp>

auto effect_new(lua_State* L) -> int
{
    const auto* const effectPath = LuaGetInline<LPCSTR>(L);
    const auto debugMode = LuaGetInline<bool>(L);

    auto* const fx = static_cast<CEffect**>(lua_newuserdata(L, sizeof(CEffect*)));
    *fx = new CEffect();
    (*fx)->LoadEffect(effectPath, debugMode);

    luaL_setmetatable(L, L_EFFECT);
    return 1;
}

static auto effect_begin(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const technique = LuaGetInline<LPCSTR>(L);

    lua_pushinteger(L, fx->Begin(technique));
    return 1;
}

static auto effect_end(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);

    lua_pushinteger(L, fx->End());
    return 1;
}

static auto effect_beginpass(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    unsigned int pass = -1;

    if (lua_isinteger(L, 1) == 0)
    {
        const auto* const passName = LuaGetInline<LPCSTR>(L);
        pass = fx->FindPass(passName);
    }
    else
    {
        pass = LuaGetInline<uint32_t>(L) - 1;
    }

    lua_pushinteger(L, fx->BeginPass(pass));
    return 1;
}

static auto effect_endpass(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);

    lua_pushinteger(L, fx->EndPass());
    return 1;
}

static auto effect_commit(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);

    fx->CommitChanges();
    return 0;
}

static auto effect_setbool(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    const auto value = LuaGetInline<bool>(L);

    fx->SetBool(name, value);
    return 0;
}

static auto effect_setfloat(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    const auto value = LuaGetInline<float>(L);

    fx->SetFloat(name, value);
    return 0;
}

static auto effect_setmatrix(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    auto const value = LuaGetInline<D3DXMATRIX>(L);

    fx->SetMatrix(name, value);
    return 0;
}

static auto effect_setvector3(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    auto const value = LuaGetInline<D3DXVECTOR3>(L);

    fx->SetVector3(name, value);
    return 0;
}

static auto effect_setinteger(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    const auto value = LuaGetInline<DWORD>(L);

    fx->SetInteger(name, value);
    return 0;
}

static auto effect_setlight(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);
    CLight* value = nullptr;

    if (LuaTestClass(L, L_LIGHT))
    {
        value = LuaGetInline<CLight*>(L);
    }

    fx->SetLight(name, value);
    return 0;
}

static auto effect_setvector4(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);

    if (LuaLength(L) >= 2)
    {
        auto const value = LuaGetInline<D3DXVECTOR3>(L);
        const auto value2 = LuaGetInline<float>(L);
        fx->SetVector4(name, D3DXVECTOR4(value, value2));
    }
    else
    {
        auto const value = LuaGetInline<D3DXVECTOR4>(L);
        fx->SetVector4(name, value);
    }

    return 0;
}

static auto effect_settexture(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);
    const auto* const name = LuaGetInline<LPCSTR>(L);

    if (LuaTestClass(L, L_RENDERTARGET))
    {
        auto* rtt = LuaGetInline<CRenderTarget*>(L);

        fx->SetTexture(name, rtt->GetTextureHandle());
    }
    else if (LuaTestClass(L, L_MATERIAL))
    {
        unsigned int slot = TEXTURESLOT_ALBEDO;
        auto* mat = LuaGetInline<CMaterial*>(L);

        if (LuaLength(L) >= 1)
        {
            slot = LuaGetInline<uint32_t>(L);
        }

        fx->SetTexture(name, mat->GetTextureHandle(slot));
    }
    else
    {
        auto* const handle = LuaGetInline<LPDIRECT3DTEXTURE9>(L);
        fx->SetTexture(name, handle);
    }

    return 0;
}

static auto effect_delete(lua_State* L) -> int
{
    auto* fx = LuaGetInline<CEffect*>(L);

    fx->Release();
    return 0;
}

static void LuaEffect$Register(lua_State* L)
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
    REGC("setInteger", effect_setinteger);
    REGC("setMatrix", effect_setmatrix);
    REGC("setVector3", effect_setvector3);
    REGC("setVector4", effect_setvector4);
    REGC("setTexture", effect_settexture);
    REGC("setLight", effect_setlight);

    REGC("__gc", effect_delete);

    lua_pop(L, 1);
}
