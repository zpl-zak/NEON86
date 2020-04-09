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
	D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, -2, L_VECTOR3);
	*out = *vec + *vecRHS;
	return 1;
}

static INT vector3_sub(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);

	vector3_new(L);
	D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, -2, L_VECTOR3);
	*out = *vec - *vecRHS;
	return 1;
}

static INT vector3_cross(lua_State* L)
{
	D3DXVECTOR3* vec = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR3);
	D3DXVECTOR3* vecRHS = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);

	vector3_new(L);
	D3DXVECTOR3* out = (D3DXVECTOR3*)luaL_checkudata(L, -2, L_VECTOR3);
	D3DXVec3Cross(out, vec, vecRHS);
	return 1;
}

static VOID LuaVector_register(lua_State* L)
{
	lua_register(L, L_VECTOR3, vector3_new);
	luaL_newmetatable(L, L_VECTOR3);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, vector3_cross); lua_setfield(L, -2, "cross");

	lua_pushcfunction(L, vector3_add); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, vector3_sub); lua_setfield(L, -2, "__sub");
	lua_pop(L, 1);
}
