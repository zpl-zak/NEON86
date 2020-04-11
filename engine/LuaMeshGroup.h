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

    lua_pushcfunction(L, meshgroup_addmesh); lua_setfield(L, -2, "addMesh");
    lua_pushcfunction(L, meshgroup_draw); lua_setfield(L, -2, "draw");
    lua_pushcfunction(L, meshgroup_clear); lua_setfield(L, -2, "clear");
    lua_pushcfunction(L, meshgroup_delete); lua_setfield(L, -2, "__gc");

    lua_pop(L, 1);
}
