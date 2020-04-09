#pragma once

#include "system.h"

#include <lua/lua.hpp>

static INT vertex_new(lua_State* L)
{
	FLOAT x=0.0f, y=0.0f, z=0.0f;
	UINT color = 0xFFFFFFFF;

	if (lua_gettop(L) >= 3)
	{
		x = (FLOAT)luaL_checknumber(L, 1);
		y = (FLOAT)luaL_checknumber(L, 2);
		z = (FLOAT)luaL_checknumber(L, 3);
	}

	if (lua_gettop(L) == 4)
	{
		color = (UINT)luaL_checknumber(L, 4);
	}

	VERTEX* vert = (VERTEX*)lua_newuserdata(L, sizeof(VERTEX));
	vert->x = x;
	vert->y = y;
	vert->z = z;
	vert->color = color;

	luaL_setmetatable(L, L_VERTEX);
	return 1;
}

static VOID LuaVertex_register(lua_State* L)
{
	lua_register(L, L_VERTEX, vertex_new);
	luaL_newmetatable(L, L_VERTEX);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
}
