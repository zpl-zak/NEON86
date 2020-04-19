#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Mesh.h"

static INT mesh_new(lua_State* L)
{
	*(CMesh*)lua_newuserdata(L, sizeof(CMesh)) = CMesh();

	luaL_setmetatable(L, L_MESH);
	return 1;
}

static INT mesh_addvertex(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);
	VERTEX* vert = (VERTEX*)luaL_checkudata(L, 2, L_VERTEX);
	mesh->AddVertex(*vert);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_addindex(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);
	SHORT index = (SHORT)luaL_checkinteger(L, 2);
	mesh->AddIndex(index);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_addtriangle(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);
	SHORT i1 = (SHORT)luaL_checkinteger(L, 2);
	SHORT i2 = (SHORT)luaL_checkinteger(L, 3);
	SHORT i3 = (SHORT)luaL_checkinteger(L, 4);
	
	mesh->AddIndex(i1);
	mesh->AddIndex(i2);
	mesh->AddIndex(i3);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_settexture(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);
	DWORD stage = (DWORD)luaL_checkinteger(L, 2);
	CTexture* tex = NULL;

	if (lua_gettop(L) == 3)
		tex = (CTexture*)luaL_checkudata(L, 3, L_TEXTURE);

	mesh->SetTexture(stage, tex ? tex : NULL);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_draw(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);
	
	mesh->Draw(NULL);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_build(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);

	mesh->Build();

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_clear(lua_State* L)
{
    CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);

    mesh->Clear();

    return 0;
}

static INT mesh_calcnormals(lua_State* L)
{
	CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);

	mesh->CalculateNormals();
	
	return 0;
}

static INT mesh_delete(lua_State* L)
{
    CMesh* mesh = (CMesh*)luaL_checkudata(L, 1, L_MESH);

    mesh->Release();

    return 0;
}

static VOID LuaMesh_register(lua_State* L)
{
	lua_register(L, L_MESH, mesh_new);
	luaL_newmetatable(L, L_MESH);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("addVertex", mesh_addvertex);
    REGC("addIndex", mesh_addindex);
    REGC("addTriangle", mesh_addtriangle);
    REGC("setTexture", mesh_settexture);
    REGC("draw", mesh_draw);
    REGC("build", mesh_build);
	REGC("calcNormals", mesh_calcnormals);
    REGC("clear", mesh_clear);
    REGC("__gc", mesh_delete);

	lua_pop(L, 1);
}
