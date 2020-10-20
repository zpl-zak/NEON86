#pragma once

#include "system.h"
#include "Light.h"
#include <lua/lua.hpp>

auto light_new(lua_State* L) -> int
{
    unsigned int slot = 0;

    if (lua_gettop(L) == 1)
    {
        slot = static_cast<unsigned int>(luaL_checkinteger(L, 1));
    }

    const auto lit = static_cast<CLight**>(lua_newuserdata(L, sizeof(CLight*)));

    *lit = new CLight(slot);

    luaL_setmetatable(L, L_LIGHT);
    return 1;
}


static auto light_delete(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));

    lit->Release();
    return 0;
}

static auto light_setdiffuse(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    lit->SetDiffuse(luaH_getcolorlinear(L, 1));

    return 0;
}

static auto light_setambient(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    lit->SetAmbient(luaH_getcolorlinear(L, 1));

    return 0;
}

static auto light_setspecular(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    lit->SetSpecular(luaH_getcolorlinear(L, 1));

    return 0;
}

static auto light_setposition(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const D3DXVECTOR3 pos = luaH_getcomps(L);
    lit->SetPosition(pos);

    return 0;
}

static auto light_setdirection(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const D3DXVECTOR3 dir = luaH_getcomps(L);
    lit->SetDirection(dir);

    return 0;
}

static auto light_setatten(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const D3DCOLORVALUE atten = {
        static_cast<float>(luaL_checknumber(L, 2)), static_cast<float>(luaL_checknumber(L, 3)),
        static_cast<float>(luaL_checknumber(L, 4))
    };
    lit->SetAttenuation(atten.r, atten.g, atten.b);

    return 0;
}


static auto light_setrange(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<float>(luaL_checknumber(L, 2));
    lit->SetRange(val);

    return 0;
}

static auto light_setfalloff(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<float>(luaL_checknumber(L, 2));
    lit->SetFalloff(val);

    return 0;
}

static auto light_setinnerangle(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<float>(luaL_checknumber(L, 2));
    lit->SetInnerAngle(val);

    return 0;
}

static auto light_setouterangle(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<float>(luaL_checknumber(L, 2));
    lit->SetOuterAngle(val);

    return 0;
}

static auto light_settype(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<DWORD>(luaL_checkinteger(L, 2));
    lit->SetType(val);

    return 0;
}

static auto light_setslot(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<DWORD>(luaL_checkinteger(L, 2));
    lit->SetSlot(val);

    return 0;
}

static auto light_enable(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    const auto val = static_cast<bool>(lua_toboolean(L, 2));

    if (lua_gettop(L) == 3)
        lit->SetSlot(static_cast<unsigned int>(luaL_checkinteger(L, 3)));

    lit->Enable(val);

    return 0;
}


static auto light_getslot(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    lua_pushinteger(L, lit->GetSlot());

    return 1;
}

static auto light_gettype(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    lua_pushinteger(L, lit->GetLightData().Type);

    return 1;
}

static auto light_getowner(lua_State* L) -> int
{
    auto lit = *static_cast<CLight**>(luaL_checkudata(L, 1, L_LIGHT));
    if (!lit->GetOwner())
        lua_pushnil(L);
    else
        LUAP(L, L_NODE, CNode, lit->GetOwner());
    return 1;
}

static void LuaLight$Register(lua_State* L)
{
    lua_register(L, L_LIGHT, light_new);
    luaL_newmetatable(L, L_LIGHT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("enable", light_enable);
    REGC("setDiffuse", light_setdiffuse);
    REGC("setAmbient", light_setambient);
    REGC("setSpecular", light_setspecular);
    REGC("setDirection", light_setdirection);
    REGC("setPosition", light_setposition);
    REGC("setRange", light_setrange);
    REGC("setFalloff", light_setfalloff);
    REGC("setAttenuation", light_setatten);
    REGC("setInnerAngle", light_setinnerangle);
    REGC("setOuterAngle", light_setouterangle);
    REGC("setType", light_settype);
    REGC("setSlot", light_setslot);
    REGC("getType", light_gettype);
    REGC("getSlot", light_getslot);
    REGC("getOwner", light_getowner);

    REGC("__gc", light_delete);

    lua_pop(L, 1);
}
