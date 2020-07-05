#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "FaceGroup.h"

INT facegroup_new(lua_State* L)
{
	*(CFaceGroup**)lua_newuserdata(L, sizeof(CFaceGroup*)) = new CFaceGroup();

	luaL_setmetatable(L, L_FACEGROUP);
	return 1;
}

static INT facegroup_clone(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
    *(CFaceGroup**)lua_newuserdata(L, sizeof(CFaceGroup*)) = mesh->Clone();

    luaL_setmetatable(L, L_FACEGROUP);
    return 1;
}

static INT facegroup_addvertex(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
	VERTEX* vert = (VERTEX*)luaL_checkudata(L, 2, L_VERTEX);
	mesh->AddVertex(*vert);

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_addindex(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
	SHORT index = (SHORT)luaL_checkinteger(L, 2);
	mesh->AddIndex(index);

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_addtriangle(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
	SHORT i1 = (SHORT)luaL_checkinteger(L, 2);
	SHORT i2 = (SHORT)luaL_checkinteger(L, 3);
	SHORT i3 = (SHORT)luaL_checkinteger(L, 4);
	
	mesh->AddIndex(i1);
	mesh->AddIndex(i2);
	mesh->AddIndex(i3);

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_setmaterial(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
	DWORD stage = (DWORD)luaL_checkinteger(L, 2);
	CMaterial* mat = NULL;

	if (lua_gettop(L) == 3)
	{
		mat = *(CMaterial**)luaL_checkudata(L, 3, L_MATERIAL);
		mat->AddRef();
	}

	mesh->SetMaterial(stage, mat ? mat : NULL);

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_getmaterial(lua_State* L)
{
    CFaceGroup* fgroup = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
    CMaterial* mat = fgroup->GetMaterial();

	if (mat) { LUAP(L, L_MATERIAL, CMaterial, mat); }
	else lua_pushnil(L);

    return 1;
}


static INT facegroup_getmaterialstage(lua_State* L)
{
    CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
    DWORD stage = mesh->GetMaterialStage();
	lua_pushinteger(L, stage);

    return 1;
}

static INT facegroup_draw(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
    D3DXMATRIX* mat = &D3DXMATRIX();

	if (lua_gettop(L) >= 2)
	{
		mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
	}
	else mat = NULL;

	mesh->Draw(mat);

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_build(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

	mesh->Build();

	lua_pushvalue(L, 1);
	return 1;
}

static INT facegroup_clear(lua_State* L)
{
    CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

    mesh->Clear();

    return 0;
}

static INT facegroup_calcnormals(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

	mesh->CalculateNormals();
	
	return 0;
}

static INT facegroup_delete(lua_State* L)
{
    CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

    mesh->Release();

    return 0;
}

static INT facegroup_getvertices(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

	lua_newtable(L);

	for (UINT i=0; i<mesh->GetNumVertices(); i++)
	{
		VERTEX* vert = (mesh->GetVertices() + i);
		lua_pushinteger(L, i + 1ULL);
		LUAPT(L, L_VERTEX, VERTEX, vert);
		lua_settable(L, -3);
	}

	return 1;
}

static INT facegroup_getbounds(lua_State* L)
{
	CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);
	D3DXVECTOR4* b = mesh->GetBounds();
	
	lua_newtable(L);

	lua_pushinteger(L, 1);
	D3DXVECTOR4* minVec = vector4_ctor(L);
	*minVec = b[0];
	lua_settable(L, -3);

	lua_pushinteger(L, 2);
    D3DXVECTOR4* maxVec = vector4_ctor(L);
	*maxVec = b[1];
    lua_settable(L, -3);

	return 1;
}

static INT facegroup_getindices(lua_State* L)
{
    CFaceGroup* mesh = *(CFaceGroup**)luaL_checkudata(L, 1, L_FACEGROUP);

    lua_newtable(L);

    for (UINT i = 0; i < mesh->GetNumIndices(); i++)
    {
        SHORT index = *(mesh->GetIndices() + i);
        lua_pushinteger(L, i + 1ULL);
        lua_pushinteger(L, index);
        lua_settable(L, -3);
    }

    return 1;
}

static VOID LuaFaceGroup_register(lua_State* L)
{
	lua_register(L, L_FACEGROUP, facegroup_new);
	luaL_newmetatable(L, L_FACEGROUP);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	REGC("clone", facegroup_clone);
    REGC("addVertex", facegroup_addvertex);
    REGC("addIndex", facegroup_addindex);
    REGC("addTriangle", facegroup_addtriangle);
    REGC("setMaterial", facegroup_setmaterial);
	REGC("getMaterial", facegroup_getmaterial);
	REGC("getMaterialStage", facegroup_getmaterialstage);
	REGC("getBounds", facegroup_getbounds);
    REGC("draw", facegroup_draw);
    REGC("build", facegroup_build);
	REGC("calcNormals", facegroup_calcnormals);
    REGC("clear", facegroup_clear);

	REGC("getVertices", facegroup_getvertices);
	REGC("getIndices", facegroup_getindices);

    REGC("__gc", facegroup_delete);

	lua_pop(L, 1);
}
