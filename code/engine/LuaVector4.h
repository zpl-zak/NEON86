#pragma once

#include "system.h"

#include <lua/lua.hpp>

auto vector4_ctor(lua_State* L) -> D3DXVECTOR4*
{
    const auto vec = static_cast<D3DXVECTOR4*>(lua_newuserdata(L, sizeof(D3DXVECTOR4)));
    *vec = {0, 0, 0, 0};
    luaL_setmetatable(L, L_VECTOR);
    return vec;
}

auto vector4_new(lua_State* L) -> int
{
    auto x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
    D3DXVECTOR4* vecRHS = nullptr;

    if (lua_gettop(L) == 1)
    {
        vecRHS = static_cast<D3DXVECTOR4*>(luaL_testudata(L, 1, L_VECTOR));

        if (!vecRHS)
            x = y = z = w = static_cast<float>(lua_tonumber(L, 1));
    }

    x = static_cast<float>(lua_tonumber(L, 1));
    y = static_cast<float>(lua_tonumber(L, 2));
    z = static_cast<float>(lua_tonumber(L, 3));
    w = static_cast<float>(lua_tonumber(L, 4));

    const auto vec = vector4_ctor(L);

    if (!vecRHS)
        *vec = D3DXVECTOR4(x, y, z, w);
    else
        *vec = *vecRHS;

    return 1;
}


static auto vector4_newrgba(lua_State* L) -> int
{
    const auto r = static_cast<float>(luaL_checknumber(L, 1)) / static_cast<float>(0xFF);
    const auto g = static_cast<float>(luaL_checknumber(L, 2)) / static_cast<float>(0xFF);
    const auto b = static_cast<float>(luaL_checknumber(L, 3)) / static_cast<float>(0xFF);
    auto a = 1.0f;

    if (lua_gettop(L) == 4)
        a = static_cast<float>(luaL_checknumber(L, 4)) / static_cast<float>(0xFF);

    const auto vec = static_cast<D3DXVECTOR4*>(lua_newuserdata(L, sizeof(D3DXVECTOR4)));
    *vec = D3DXVECTOR4(r, g, b, a);

    luaL_setmetatable(L, L_VECTOR);
    return 1;
}

static auto vector4_add(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    D3DXVECTOR4* vecRHS = nullptr;
    auto scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = static_cast<float>(luaL_checknumber(L, 2));

        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        *out = D3DXVECTOR4(vec->x + scalarRHS, vec->y + scalarRHS, vec->z + scalarRHS, vec->w + scalarRHS);
        return 1;
    }

    if ((vecRHS = static_cast<D3DXVECTOR4*>(luaL_testudata(L, 2, L_VECTOR))))
    {
        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        D3DXVec4Add(out, vec, vecRHS);

        return 1;
    }

    return 0;
}

static auto vector4_sub(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    D3DXVECTOR4* vecRHS = nullptr;
    auto scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = static_cast<float>(luaL_checknumber(L, 2));

        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        *out = D3DXVECTOR4(vec->x - scalarRHS, vec->y - scalarRHS, vec->z - scalarRHS, vec->w - scalarRHS);
        return 1;
    }

    if ((vecRHS = static_cast<D3DXVECTOR4*>(luaL_testudata(L, 2, L_VECTOR))))
    {
        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        D3DXVec4Subtract(out, vec, vecRHS);
        out->w = 0.0f;
        return 1;
    }

    return 0;
}

static auto vector4_cross(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    const auto vecRHS = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));

    vector4_new(L);
    const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));

    D3DXVECTOR3 a = *vec, b = *vecRHS, c;

    D3DXVec3Cross(&c, &a, &b);
    *out = D3DXVECTOR4(c, 0.0f);
    return 1;
}

static auto vector4_div(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    const auto scalarRHS = static_cast<float>(luaL_checknumber(L, 2));

    if (scalarRHS == 0.0f)
    {
        lua_pushnil(L);
        return 1;
    }

    vector4_new(L);
    const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
    vec->w = 0.0f;
    *out = *vec / scalarRHS;
    return 1;
}

static auto vector4_dot(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    D3DXVECTOR4* vecRHS = nullptr;
    D3DXMATRIX* matRHS = nullptr;
    auto scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = static_cast<float>(luaL_checknumber(L, 2));

        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        *out = *vec * scalarRHS;
        return 1;
    }

    if ((vecRHS = static_cast<D3DXVECTOR4*>(luaL_testudata(L, 2, L_VECTOR))))
    {
        const auto num = D3DXVec4Dot(vec, vecRHS);
        lua_pushnumber(L, num);
        return 1;
    }

    if ((matRHS = static_cast<D3DXMATRIX*>(luaL_testudata(L, 2, L_MATRIX))))
    {
        vector4_new(L);
        const auto out = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 3, L_VECTOR));
        D3DXVec3TransformCoord(out, (D3DXVECTOR3*)vec, matRHS);
        return 1;
    }

    return 0;
}

static auto vector4_get(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    const auto onlyThreeComponents = static_cast<unsigned int>(lua_tointeger(L, 2));
    float arr[4] = {vec->x, vec->y, vec->z, vec->w};

    lua_newtable(L);
    for (unsigned int i = 0; i < static_cast<unsigned int>(onlyThreeComponents ? 3 : 4); i++)
    {
        lua_pushinteger(L, i + 1ULL);
        lua_pushnumber(L, arr[i]);
        lua_settable(L, -3);
    }

    return 1;
}

static auto vector4_field(lua_State* L) -> int
{
    const auto vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    const auto idx = static_cast<int>(luaL_checkinteger(L, 2));
    const auto val = static_cast<float>(luaL_checknumber(L, 3));

    auto out = vector4_ctor(L);
    *out = *vec;
    switch (idx)
    {
    case 1: out->x = val;
        break;
    case 2: out->y = val;
        break;
    case 3: out->z = val;
        break;
    case 4: out->w = val;
        break;
    }
    return 1;
}

static auto vector4_normalize(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 1, L_VECTOR));

    vector4_new(L);
    auto* out = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 2, L_VECTOR));
    D3DXVec3Normalize(out, vec);

    return 1;
}

static auto vector4_color(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    BYTE color[4] = {
        static_cast<BYTE>(vec->w * 0xFF), static_cast<BYTE>(vec->x * 0xFF), static_cast<BYTE>(vec->y * 0xFF),
        static_cast<BYTE>(vec->z * 0xFF)
    };
    lua_pushinteger(L, D3DCOLOR_ARGB(color[0], color[1], color[2], color[3]));
    return 1;
}

static auto vector4_mag(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    lua_pushnumber(L, D3DXVec4Length(vec));
    return 1;
}

static auto vector4_magsq(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    lua_pushnumber(L, D3DXVec4LengthSq(vec));
    return 1;
}

static auto vector4_lerp(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    auto* vecRHS = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    const auto t = static_cast<float>(luaL_checknumber(L, 3));

    vector4_new(L);
    auto* out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 4, L_VECTOR));
    D3DXVec4Lerp(out, vec, vecRHS, t);
    return 1;
}

static auto vector4_neg(lua_State* L) -> int
{
    auto* vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));

    vector4_new(L);
    auto* out = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    *out = *vec * -1;
    return 1;
}

LUAS(D3DXVECTOR4, L_VECTOR, vector4_x, x);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_y, y);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_z, z);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_w, w);

static void LuaVector$Register(lua_State* L)
{
    // compat
    lua_register(L, "Vector3", vector4_new);
    lua_register(L, "Vector4", vector4_new);
    lua_register(L, "VectorRGBA", vector4_newrgba);
    //<
    lua_register(L, L_VECTOR, vector4_new);
    luaL_newmetatable(L, L_VECTOR);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("cross", vector4_cross);
    REGC("get", vector4_get);
    REGC("color", vector4_color);
    REGC("mag", vector4_mag);
    REGC("magSq", vector4_magsq);
    REGC("lerp", vector4_lerp);
    REGC("neg", vector4_neg);
    REGC("normalize", vector4_normalize);

    REGC("__add", vector4_add);
    REGC("__sub", vector4_sub);
    REGC("__mul", vector4_dot);
    REGC("__div", vector4_div);

    REGC("m", vector4_field);
    REGC("x", vector4_x);
    REGC("y", vector4_y);
    REGC("z", vector4_z);
    REGC("w", vector4_w);

    lua_pop(L, 1);
}
