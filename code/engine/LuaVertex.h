#pragma once

#include "system.h"

#include <lua/lua.hpp>

INT vertex_new(lua_State* L)
{
	FLOAT x=0.0F, y=0.0F, z=0.0F;
	FLOAT nx=0.0F, ny=0.0F, nz=0.0F;
	FLOAT tx=0.0F, ty=0.0F, tz=0.0F;
	FLOAT bx=0.0F, by=0.0F, bz=0.0F;
	FLOAT su=0.0F, tv=0.0F;
	FLOAT su2=0.0F, tv2=0.0F;
	DWORD color = 0xFFFFFFFF;

	if (lua_gettop(L) >= 3)
	{
		x = (FLOAT)luaL_checknumber(L, 1);
		y = (FLOAT)luaL_checknumber(L, 2);
		z = (FLOAT)luaL_checknumber(L, 3);
	}

	if (lua_gettop(L) >= 5)
	{
		su = (FLOAT)luaL_checknumber(L, 4);
		tv = (FLOAT)luaL_checknumber(L, 5);
	}

	if (lua_gettop(L) >= 6)
	{
		color = (DWORD)luaL_checknumber(L, 6);
	}

	if (lua_gettop(L) >= 9)
	{
        nx = (FLOAT)luaL_checknumber(L, 7);
        ny = (FLOAT)luaL_checknumber(L, 8);
        nz = (FLOAT)luaL_checknumber(L, 9);
	}

	if (lua_gettop(L) >= 12)
	{
        tx = (FLOAT)luaL_checknumber(L, 10);
        ty = (FLOAT)luaL_checknumber(L, 11);
        tz = (FLOAT)luaL_checknumber(L, 12);
	}

	if (lua_gettop(L) >= 15)
	{
        bx = (FLOAT)luaL_checknumber(L, 13);
        by = (FLOAT)luaL_checknumber(L, 14);
        bz = (FLOAT)luaL_checknumber(L, 15);
	}

	if (lua_gettop(L) >= 17)
	{
        su2 = (FLOAT)luaL_checknumber(L, 16);
        tv2 = (FLOAT)luaL_checknumber(L, 17);
	}

	VERTEX* vert = (VERTEX*)lua_newuserdata(L, sizeof(VERTEX));
	ZeroMemory(vert, sizeof(VERTEX));
	vert->x = x;
	vert->y = y;
	vert->z = z;

	vert->su = su;
	vert->tv = tv;

	vert->su2 = su2;
	vert->tv2 = tv2;

	vert->nx = nx;
	vert->ny = ny;
	vert->nz = nz;

	vert->tx = tx;
	vert->ty = ty;
	vert->tz = tz;

	vert->bx = bx;
	vert->by = by;
	vert->bz = bz;
	vert->color = color;

	luaL_setmetatable(L, L_VERTEX);
	return 1;
}

static INT vertex_get(lua_State* L)
{
	VERTEX* vert = (VERTEX*)luaL_checkudata(L, 1, L_VERTEX);
	FLOAT arr[] = { vert->x, vert->y, vert->z, vert->su, vert->tv, vert->nx, vert->ny, vert->nz, vert->tx, vert->ty, vert->tz, vert->bx, vert->by, vert->bz, vert->su2, vert->tv2 };
	FLOAT col[4] = { (FLOAT)vert->r, (FLOAT)vert->g, (FLOAT)vert->b, (FLOAT)vert->a };
	UINT arrLen = sizeof(arr)/sizeof(arr[0]);

	lua_newtable(L);
	for (UINT i=0; i<arrLen; i++)
	{
		lua_pushinteger(L, i+1ULL);
		lua_pushnumber(L, arr[i]);
		lua_settable(L, -3);
	}

	lua_pushinteger(L, arrLen+1);
	lua_newtable(L);

	for (UINT i=0; i<4; i++)
	{
		lua_pushinteger(L, i+1ULL);
		lua_pushnumber(L, col[i]);
		lua_settable(L, -3);
	}
	lua_settable(L, -3);

	return 1;
}

static VOID LuaVertex_register(lua_State* L)
{
	lua_register(L, L_VERTEX, vertex_new);
	luaL_newmetatable(L, L_VERTEX);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	REGC("get", vertex_get);

	lua_pop(L, 1);
}
