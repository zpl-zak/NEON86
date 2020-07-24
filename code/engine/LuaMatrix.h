#pragma once

#include "system.h"

#include <lua/lua.hpp>

INT matrix_new(lua_State* L)
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
	D3DXMatrixMultiply(out, mat, matRHS);

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

static INT matrix_persp(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    FLOAT fov = (FLOAT)luaL_checknumber(L, 2);
    FLOAT zNear = 0.1f, zFar = 1000.0f;
    BOOL flipHandedness = FALSE;

    if (lua_gettop(L) >= 4)
    {
        zNear = (FLOAT)luaL_checknumber(L, 3);
        zFar = (FLOAT)luaL_checknumber(L, 4);
    }

    if (lua_gettop(L) >= 5)
    {
        flipHandedness = (BOOL)lua_toboolean(L, 5);
    }

    RECT res = RENDERER->GetSurfaceResolution();

    if (flipHandedness)
    {
        D3DXMatrixPerspectiveFovRH(mat,
            D3DXToRadian(fov),
            (FLOAT)res.right / (FLOAT)res.bottom,
            zNear,
            zFar);
    }
    else
    {
        D3DXMatrixPerspectiveFovLH(mat,
            D3DXToRadian(fov),
            (FLOAT)res.right / (FLOAT)res.bottom,
            zNear,
            zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}

static INT matrix_ortho(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    RECT res = RENDERER->GetSurfaceResolution();
    FLOAT w = (FLOAT)res.right, h = (FLOAT)res.bottom;
    BOOL flipHandedness = FALSE;

    if (lua_gettop(L) >= 3)
    {
        w = (FLOAT)luaL_checknumber(L, 2) * ((FLOAT)res.right / (FLOAT)res.bottom);
        h = (FLOAT)luaL_checknumber(L, 3);
    }

    FLOAT zNear = 0.01f, zFar = 100.0f;

    if (lua_gettop(L) >= 5)
    {
        zNear = (FLOAT)luaL_checknumber(L, 4);
        zFar = (FLOAT)luaL_checknumber(L, 5);
    }

    if (lua_gettop(L) >= 6)
    {
        flipHandedness = (BOOL)lua_toboolean(L, 6);
    }

    if (flipHandedness)
    {
        D3DXMatrixOrthoRH(mat,
            w,
            h,
            zNear,
            zFar);
    }
    else
    {
        D3DXMatrixOrthoLH(mat,
            w,
            h,
            zNear,
            zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}


static INT matrix_orthoex(lua_State* L)
{
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    RECT res = RENDERER->GetSurfaceResolution();
    FLOAT l = 0.0f, t = 0.0f;
    FLOAT r = (FLOAT)res.right, b = (FLOAT)res.bottom;
    BOOL flipHandedness = FALSE;

    if (lua_gettop(L) >= 5)
    {
        l = (FLOAT)luaL_checknumber(L, 2);
        r = (FLOAT)luaL_checknumber(L, 3);
        b = (FLOAT)luaL_checknumber(L, 4);
        t = (FLOAT)luaL_checknumber(L, 5);
    }

    FLOAT zNear = 0.01f, zFar = 100.0f;

    if (lua_gettop(L) >= 6)
    {
        zNear = (FLOAT)luaL_checknumber(L, 6);
        zFar = (FLOAT)luaL_checknumber(L, 7);
    }

    if (lua_gettop(L) >= 8)
    {
        flipHandedness = (BOOL)lua_toboolean(L, 8);
    }

    if (flipHandedness)
    {
        D3DXMatrixOrthoOffCenterRH(mat,
            l, r, b, t,
            zNear,
            zFar);
    }
    else
    {
        D3DXMatrixOrthoOffCenterLH(mat,
            l, r, b, t,
            zNear,
            zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}

static INT matrix_getfield(lua_State* L)
{
	D3DXMATRIX* matPtr = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	UINT row = (UINT)luaL_checkinteger(L, 2) -1;
	UINT col = (UINT)luaL_checkinteger(L, 3) -1;
	FLOAT val = 0.0f;

	if (lua_gettop(L) >= 4)
	{
		val = (FLOAT)luaL_checknumber(L, 4);
		matPtr->m[row][col] = val;
		matrix_new(L);
		D3DXMATRIX* newMat = (D3DXMATRIX*)luaL_checkudata(L, 5, L_MATRIX);
		*newMat = *matPtr;
		return 1;
	}

	D3DXMATRIX mat = *matPtr;
	lua_pushnumber(L, mat(row, col));
	return 1;
}

static INT matrix_getrow(lua_State* L)
{
    D3DXMATRIX* matPtr = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	UINT row = (UINT)luaL_checkinteger(L, 2) - 1;

	if (lua_gettop(L) >= 3)
	{
		D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        matrix_new(L);
        D3DXMATRIX* newMat = (D3DXMATRIX*)luaL_checkudata(L, 4, L_MATRIX);
        *newMat = *matPtr;

		newMat[row][0] = vec->x;
		newMat[row][1] = vec->y;
		newMat[row][2] = vec->z;
		newMat[row][3] = vec->w;
		return 1;
	}

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

    if (lua_gettop(L) >= 3)
    {
        D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        matrix_new(L);
        D3DXMATRIX* newMat = (D3DXMATRIX*)luaL_checkudata(L, 4, L_MATRIX);
        *newMat = *matPtr;

        newMat[0][col] = vec->x;
        newMat[0][col] = vec->y;
        newMat[0][col] = vec->z;
        newMat[0][col] = vec->w;
        return 1;
    }

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
	REGC("persp", matrix_persp);
	REGC("ortho", matrix_ortho);
	REGC("orthoEx", matrix_orthoex);
	REGC("__mul", matrix_mul);
	REGC("m", matrix_getfield);
	REGC("inverse", matrix_inverse);
	REGC("reflect", matrix_reflect);
	REGC("shadow", matrix_shadow);
	REGC("row", matrix_getrow);
	REGC("col", matrix_getcol);

	lua_pop(L, 1);
}
