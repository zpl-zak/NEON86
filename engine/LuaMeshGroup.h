#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "MeshGroup.h"

static INT meshgroup_new(lua_State* L)
{
    *(CMeshGroup*)lua_newuserdata(L, sizeof(CMeshGroup)) = CMeshGroup();

    luaL_setmetatable(L, L_MESHGROUP);
    return 1;
}

static INT meshgroup_addmesh(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);
    CMesh* mesh = (CMesh*)luaL_checkudata(L, 2, L_MESH);
    D3DMATRIX* mat = (D3DMATRIX*)luaL_checkudata(L, 3, L_MATRIX);
    meshGroup->AddMesh(mesh, *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT meshgroup_draw(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

    meshGroup->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT meshgroup_loadmesh(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);
    LPSTR meshName = (LPSTR)luaL_checkstring(L, 2);
    UINT texFlags = TEXF_ANISOTROPIC;

    if (lua_gettop(L) == 3)
        texFlags = (UINT)luaL_checkinteger(L, 3);

    meshGroup->LoadMesh(meshName, texFlags);

    lua_pushvalue(L, 1);
    return 1;
}

static INT meshgroup_delete(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);

    meshGroup->Release();

    return 0;
}

static INT meshgroup_clear(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);

    meshGroup->Clear();

    return 0;
}

static VOID LuaMeshGroup_register(lua_State* L)
{
    lua_register(L, L_MESHGROUP, meshgroup_new);
    luaL_newmetatable(L, L_MESHGROUP);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("addMesh", meshgroup_addmesh);
    REGC("draw", meshgroup_draw);
    REGC("loadMesh", meshgroup_loadmesh);
    REGC("clear", meshgroup_clear);
    REGC("__gc", meshgroup_delete);
    
    lua_pop(L, 1);
}
