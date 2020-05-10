#pragma once

#include "system.h"

#include <lua/lua.hpp>

static INT matrix_new(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)lua_newuserdata(L, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(mat);

	luaL_setmetatable(L, L_MATRIX);
	return 1;
}

static INT matrix_translate(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR3 vec = luaH_getcomps(L);
	
	D3DXMATRIX t;
	D3DXMatrixTranslation(&t, vec.x, vec.y, vec.z);
	
	*mat *= t;

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_rotate(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR3 vec = luaH_getcomps(L);

	D3DXMATRIX t;
	D3DXMatrixRotationYawPitchRoll(&t, vec.x, vec.y, vec.z);

	*mat *= t;
	
	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_scale(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR3 vec = luaH_getcomps(L);
	D3DXMATRIX t;
	D3DXMatrixScaling(&t, vec.x, vec.y, vec.z);
	*mat *= t;

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_reflect(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    D3DXVECTOR4 vec = luaH_getcomps(L);
    D3DXMATRIX t;
	D3DXPLANE plane(vec.x, vec.y, vec.z, vec.w);
	D3DXMatrixReflect(&t, &plane);
    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static INT matrix_shadow(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    D3DXVECTOR4 vec = luaH_getcomps(L);
	D3DXVECTOR4 lit = luaH_getcomps(L, 1);
    D3DXMATRIX t;
    D3DXPLANE plane(vec.x, vec.y, vec.z, vec.w);
    D3DXMatrixShadow(&t, &lit, &plane);
    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static INT matrix_mul(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXMATRIX* matRHS = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

	D3DXMATRIX* out = (D3DXMATRIX*)lua_newuserdata(L, sizeof(D3DXMATRIX));
	*out = *mat * *matRHS;

	luaL_setmetatable(L, L_MATRIX);
	return 1;
}

static INT matrix_bind(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	FLOAT kind = (FLOAT)luaL_checknumber(L, 2);
	RENDERER->SetMatrix((UINT)kind, *mat);

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_lookat(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR3* eye = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR);
	D3DXVECTOR3* at = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR);
	D3DXVECTOR3* up = (D3DXVECTOR3*)luaL_checkudata(L, 4, L_VECTOR);
	D3DXMATRIX t;

	D3DXMatrixLookAtLH(&t, eye, at, up);

	*mat *= t;
	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_getfield(lua_State* L)
{
	D3DXMATRIX* matPtr = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	UINT row = (UINT)luaL_checkinteger(L, 2) -1;
	UINT col = (UINT)luaL_checkinteger(L, 3) -1;

	D3DXMATRIX mat = *matPtr;
	lua_pushnumber(L, mat(row, col));
	return 1;
}

static INT matrix_getrow(lua_State* L)
{
    D3DXMATRIX* matPtr = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	UINT row = (UINT)luaL_checkinteger(L, 2) - 1;

    D3DXMATRIX mat = *matPtr;
	D3DXVECTOR4* vec = (D3DXVECTOR4*)lua_newuserdata(L, sizeof(D3DXVECTOR4));
	luaL_setmetatable(L, L_VECTOR);
	*vec = D3DXVECTOR4(mat(row, 0), mat(row, 1), mat(row, 2), mat(row, 3));
    return 1;
}

static INT matrix_getcol(lua_State* L)
{
    D3DXMATRIX* matPtr = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    UINT col = (UINT)luaL_checkinteger(L, 2) - 1;

    D3DXMATRIX mat = *matPtr;
    D3DXVECTOR4* vec = (D3DXVECTOR4*)lua_newuserdata(L, sizeof(D3DXVECTOR4));
    luaL_setmetatable(L, L_VECTOR);
    *vec = D3DXVECTOR4(mat(0, col), mat(1, col), mat(2, col), mat(3, col));
    return 1;
}

static INT matrix_inverse(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXMATRIX t;
	FLOAT d;

    D3DXMATRIX* ok = D3DXMatrixInverse(&t, &d, mat);

	if (!ok)
		lua_pushnil(L);
	else
	{
		lua_newtable(L);

		lua_pushinteger(L, 1);
		lua_pushvalue(L, 1);
		lua_settable(L, 2);

		lua_pushinteger(L, 2);
		lua_pushnumber(L, d);
		lua_settable(L, 2);

		lua_pushvalue(L, 2);
	}

    return 1;
}

static VOID LuaMatrix_register(lua_State* L)
{
	lua_register(L, L_MATRIX, matrix_new);
	luaL_newmetatable(L, L_MATRIX);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	REGC("translate", matrix_translate);
	REGC("rotate", matrix_rotate);
	REGC("scale", matrix_scale);
	REGC("bind", matrix_bind);
	REGC("lookAt", matrix_lookat);
	REGC("__mul", matrix_mul);
	REGC("m", matrix_getfield);
	REGC("inverse", matrix_inverse);
	REGC("reflect", matrix_reflect);
	REGC("shadow", matrix_shadow);
	REGC("row", matrix_getrow);
	REGC("col", matrix_getcol);

	lua_pop(L, 1);
}
