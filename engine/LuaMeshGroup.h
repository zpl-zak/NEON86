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

static INT meshgroup_getmeshes(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);

    lua_newtable(L);

    for (UINT i=0; i<meshGroup->GetNumMeshes(); i++)
    {
        CMesh* mesh = meshGroup->GetMeshes()[i];
        lua_pushinteger(L, i+1ULL);
        lua_pushlightuserdata(L, (void*)mesh);
        luaL_setmetatable(L, L_MESH);
        lua_settable(L, -3);
    }

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

static INT meshgroup_setname(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);
    LPCSTR meshName = luaL_checkstring(L, 2);

    meshGroup->SetName(aiString(meshName));

    return 0;
}

static INT meshgroup_getname(lua_State* L)
{
    CMeshGroup* meshGroup = (CMeshGroup*)luaL_checkudata(L, 1, L_MESHGROUP);

    lua_pushstring(L, meshGroup->GetName().C_Str());
    return 1;
}

static VOID LuaMeshGroup_register(lua_State* L)
{
    lua_register(L, L_MESHGROUP, meshgroup_new);
    luaL_newmetatable(L, L_MESHGROUP);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("addMesh", meshgroup_addmesh);
    REGC("draw", meshgroup_draw);
    REGC("getMeshes", meshgroup_getmeshes);
    REGC("clear", meshgroup_clear);
    REGC("setName", meshgroup_setname);
    REGC("getName", meshgroup_getname);
    REGC("__gc", meshgroup_delete);
    
    lua_pop(L, 1);
}
