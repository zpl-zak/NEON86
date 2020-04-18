#pragma once

#include "system.h"

#include <lua/lua.hpp>

static INT vertex_new(lua_State* L)
{
	FLOAT x=0.0f, y=0.0f, z=0.0f;
	FLOAT s=0.0f, t=0.0f;
	UINT color = 0xFFFFFFFF;

	if (lua_gettop(L) >= 3)
	{
		x = (FLOAT)luaL_checknumber(L, 1);
		y = (FLOAT)luaL_checknumber(L, 2);
		z = (FLOAT)luaL_checknumber(L, 3);
	}

	if (lua_gettop(L) >= 5)
	{
		s = (FLOAT)luaL_checknumber(L, 4);
		t = (FLOAT)luaL_checknumber(L, 5);
	}

	if (lua_gettop(L) == 6)
	{
		color = (UINT)luaL_checknumber(L, 6);
	}

	VERTEX* vert = (VERTEX*)lua_newuserdata(L, sizeof(VERTEX));
	vert->x = x;
	vert->y = y;
	vert->z = z;
	vert->s = s;
	vert->t = t;
	vert->color = color;

	luaL_setmetatable(L, L_VERTEX);
	return 1;
}

static INT vertex_get(lua_State* L)
{
	VERTEX* vert = (VERTEX*)luaL_checkudata(L, 1, L_VERTEX);
	FLOAT arr[5] = { vert->x, vert->y, vert->z, vert->s, vert->t };
	FLOAT col[4] = { (FLOAT)vert->r, (FLOAT)vert->g, (FLOAT)vert->b, (FLOAT)vert->a };

	lua_newtable(L);
	for (UINT i=0; i<5; i++)
	{
		lua_pushinteger(L, i+1ULL);
		lua_pushnumber(L, arr[i]);
		lua_settable(L, -3);
	}

	lua_pushinteger(L, 6);
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
