#pragma once

#include "system.h"

#include <lua/lua.hpp>

static INT vector3_new(lua_State* L)
{
	FLOAT x=0.0f, y=0.0f, z=0.0f;

	if (lua_gettop(L) == 3)
	{
		x = (FLOAT)luaL_checknumber(L, 1);
		y = (FLOAT)luaL_checknumber(L, 2);
		z = (FLOAT)luaL_checknumber(L, 3);
	}
	else if (lua_gettop(L) == 1)
	{
		x = y = z = (FLOAT)luaL_checknumber(L, 1);
	}

	D3DXVECTOR3* vec = (D3DXVECTOR3*)lua_newuserdata(L, sizeof(D3DXVECTOR3));
	*vec = D3DXVECTOR3(x, y, z);
	luaL_setmetatable(L, L_VECTOR3);
	return 1;
}

static INT vector3_add(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);

    vector3_new(L);
    D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
    *out = *vec + *vecRHS;
	return 1;
}

static INT vector3_sub(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);

	vector3_new(L);
	D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
	*out = *vec - *vecRHS;
	return 1;
}

static INT vector3_cross(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);

	vector3_new(L);
	D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
	D3DXVec3Cross(out, vec, vecRHS);
	return 1;
}

static INT vector3_dot(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = NULL;
	D3DXMATRIX* matRHS = NULL;
	FLOAT scalarRHS = 0.0f;

    if (lua_isnumber(L, 2))
    {
        scalarRHS = (FLOAT)luaL_checknumber(L, 2);

        vector3_new(L);
        D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
        *out = *vec * scalarRHS;
        return 1;
    }

	if ((vecRHS = (D3DXVECTOR3*)luaL_testudata(L, 2, L_VECTOR3)))
	{
		FLOAT num = D3DXVec3Dot(vec, vecRHS);
		lua_pushnumber(L, num);
		return 1;	
	}

	if ((matRHS = (D3DXMATRIX*)luaL_testudata(L, 2, L_MATRIX)))
	{
		vector3_new(L);
		D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
		D3DXVec3TransformCoord(out, vec, matRHS);
		return 1;
	}
	
	return 0;
}

static INT vector3_get(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	FLOAT arr[3] = { vec->x, vec->y, vec->z };
	
	lua_newtable(L);
	for (UINT i=0; i<3; i++)
	{
		lua_pushinteger(L, i+1ULL);
		lua_pushnumber(L, arr[i]);
		lua_settable(L, -3);
	}

	return 1;
}

static INT vector3_x(lua_State* L)
{
    D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
    lua_pushnumber(L, vec->x);
    return 1;
}

static INT vector3_y(lua_State* L)
{
    D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
    lua_pushnumber(L, vec->y);
    return 1;
}

static INT vector3_z(lua_State* L)
{
    D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
    lua_pushnumber(L, vec->z);
    return 1;
}

static VOID LuaVector_register(lua_State* L)
{
	lua_register(L, L_VECTOR3, vector3_new);
	luaL_newmetatable(L, L_VECTOR3);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, vector3_cross); lua_setfield(L, -2, "cross");
	lua_pushcfunction(L, vector3_get); lua_setfield(L, -2, "get");

	lua_pushcfunction(L, vector3_add); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, vector3_sub); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, vector3_dot); lua_setfield(L, -2, "__mul");

	lua_pushcfunction(L, vector3_x); lua_setfield(L, -2, "x");
	lua_pushcfunction(L, vector3_y); lua_setfield(L, -2, "y");
	lua_pushcfunction(L, vector3_z); lua_setfield(L, -2, "z");

	lua_pop(L, 1);
}
