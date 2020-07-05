#pragma once

#include "system.h"

#include <lua/lua.hpp>

D3DXVECTOR4* vector4_ctor(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)lua_newuserdata(L, sizeof(D3DXVECTOR4));
    *vec = { 0,0,0,0 };
    luaL_setmetatable(L, L_VECTOR);
    return vec;
}

INT vector4_new(lua_State* L)
{
	FLOAT x=0.0f, y=0.0f, z=0.0f, w=0.0f;
	D3DXVECTOR4* vecRHS = NULL;

	if (lua_gettop(L) == 1)
	{
		vecRHS = (D3DXVECTOR4*)luaL_testudata(L, 1, L_VECTOR);

		if (!vecRHS)
			x = y = z = w = (FLOAT)lua_tonumber(L, 1);
	}

    x = (FLOAT)lua_tonumber(L, 1);
    y = (FLOAT)lua_tonumber(L, 2);
    z = (FLOAT)lua_tonumber(L, 3);
	w = (FLOAT)lua_tonumber(L, 4);

    D3DXVECTOR4* vec = vector4_ctor(L);

    if (!vecRHS)
        *vec = D3DXVECTOR4(x, y, z, w);
    else
        *vec = *vecRHS;

	return 1;
}


static INT vector4_newrgba(lua_State* L)
{
    FLOAT r = (FLOAT)luaL_checknumber(L, 1) / (FLOAT)0xFF;
    FLOAT g = (FLOAT)luaL_checknumber(L, 2) / (FLOAT)0xFF;
    FLOAT b = (FLOAT)luaL_checknumber(L, 3) / (FLOAT)0xFF;
    FLOAT a = 1.0f;

    if (lua_gettop(L) == 4)
        a = (FLOAT)luaL_checknumber(L, 4) / (FLOAT)0xFF;

    D3DXVECTOR4* vec = (D3DXVECTOR4*)lua_newuserdata(L, sizeof(D3DXVECTOR4));
    *vec = D3DXVECTOR4(r,g,b,a);
 
    luaL_setmetatable(L, L_VECTOR);
    return 1;
}

static INT vector4_add(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXVECTOR4* vecRHS = NULL;
    FLOAT scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = (FLOAT)luaL_checknumber(L, 2);

        vector4_new(L);
        D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        *out = D3DXVECTOR4(vec->x + scalarRHS, vec->y + scalarRHS, vec->z + scalarRHS, vec->w + scalarRHS);
        return 1;
    }

    if ((vecRHS = (D3DXVECTOR4*)luaL_testudata(L, 2, L_VECTOR)))
    {
        vector4_new(L);
        D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        D3DXVec4Add(out, vec, vecRHS);

        return 1;
    }

    return 0;
}

static INT vector4_sub(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXVECTOR4* vecRHS = NULL;
    FLOAT scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = (FLOAT)luaL_checknumber(L, 2);

        vector4_new(L);
        D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
		*out = D3DXVECTOR4(vec->x - scalarRHS, vec->y - scalarRHS, vec->z - scalarRHS, vec->w - scalarRHS);
        return 1;
    }

    if ((vecRHS = (D3DXVECTOR4*)luaL_testudata(L, 2, L_VECTOR)))
    {
        vector4_new(L);
        D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        D3DXVec4Subtract(out, vec, vecRHS);
        out->w = 0.0f;
		return 1;
    }

    return 0;
}
static INT vector4_cross(lua_State* L)
{
	D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
	D3DXVECTOR4* vecRHS = (D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);

	vector4_new(L);
	D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);

	D3DXVECTOR3 a=*vec, b=*vecRHS, c;

	D3DXVec3Cross(&c, &a, &b);
	*out = D3DXVECTOR4(c, 0.0f);
	return 1;
}

static INT vector4_div(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    FLOAT scalarRHS = (FLOAT)luaL_checknumber(L, 2);

	if (scalarRHS == 0.0f)
	{
		lua_pushnil(L);
		return 1;
	}

    vector4_new(L);
    D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
    vec->w = 0.0f;
    *out = *vec / scalarRHS;
    return 1;
}

static INT vector4_dot(lua_State* L)
{
	D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
	D3DXVECTOR4* vecRHS = NULL;
	D3DXMATRIX* matRHS = NULL;
	FLOAT scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = (FLOAT)luaL_checknumber(L, 2);

        vector4_new(L);
        D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        *out = *vec * scalarRHS;
        return 1;
    }

	if ((vecRHS = (D3DXVECTOR4*)luaL_testudata(L, 2, L_VECTOR)))
	{
		FLOAT num = D3DXVec4Dot(vec, vecRHS);
		lua_pushnumber(L, num);
		return 1;	
	}

	if ((matRHS = (D3DXMATRIX*)luaL_testudata(L, 2, L_MATRIX)))
	{
		vector4_new(L);
		D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR);
		D3DXVec3TransformCoord(out, (D3DXVECTOR3*)vec, matRHS);
		return 1;
	}
	
	return 0;
}

static INT vector4_get(lua_State* L)
{
	D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    UINT onlyThreeComponents = (UINT)lua_tointeger(L, 2);
	FLOAT arr[4] = { vec->x, vec->y, vec->z, vec->w };
	
	lua_newtable(L);
	for (UINT i=0; i<(UINT)(onlyThreeComponents ? 3 : 4); i++)
	{
		lua_pushinteger(L, i+1ULL);
		lua_pushnumber(L, arr[i]);
		lua_settable(L, -3);
	}

	return 1;
}

static INT vector4_field(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    INT idx = (INT)luaL_checkinteger(L, 2);
    FLOAT val = (FLOAT)luaL_checknumber(L, 3);
    
    D3DXVECTOR4 *out = vector4_ctor(L);
    *out = *vec;
    switch (idx) {
    case 1: out->x = val; break;
    case 2: out->y = val; break;
    case 3: out->z = val; break;
    case 4: out->w = val; break;
    }
    return 1;
}

static INT vector4_normalize(lua_State* L)
{
    D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR);

    vector4_new(L);
    D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR);
    D3DXVec3Normalize(out, vec);

    return 1;
}

static INT vector4_color(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
	BYTE color[4] = { (BYTE)(vec->w*0xFF), (BYTE)(vec->x*0xFF), (BYTE)(vec->y*0xFF), (BYTE)(vec->z*0xFF) };
	lua_pushinteger(L, D3DCOLOR_ARGB(color[0], color[1], color[2], color[3]));
    return 1;
}

static INT vector4_mag(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
	lua_pushnumber(L, D3DXVec4Length(vec));
    return 1;
}

static INT vector4_magsq(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    lua_pushnumber(L, D3DXVec4LengthSq(vec));
    return 1;
}

static INT vector4_lerp(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
	D3DXVECTOR4* vecRHS = (D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
	FLOAT t = (FLOAT)luaL_checknumber(L, 3);
    
	vector4_new(L);
	D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 4, L_VECTOR);
	D3DXVec4Lerp(out, vec, vecRHS, t);
    return 1;
}

static INT vector4_neg(lua_State* L)
{
    D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    
	vector4_new(L);
	D3DXVECTOR4* out = (D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
	*out = *vec * -1;
    return 1;
}

LUAS(D3DXVECTOR4, L_VECTOR, vector4_x, x);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_y, y);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_z, z);
LUAS(D3DXVECTOR4, L_VECTOR, vector4_w, w);

static VOID LuaVector_register(lua_State* L)
{
	// compat
	lua_register(L, "Vector3", vector4_new);
	lua_register(L, "Vector4", vector4_new);
	lua_register(L, "VectorRGBA", vector4_newrgba);
	//<
	lua_register(L, L_VECTOR, vector4_new);
	luaL_newmetatable(L, L_VECTOR);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

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
