#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Mesh.h"

static INT mesh_new(lua_State* L)
{
    *(CMesh**)lua_newuserdata(L, sizeof(CMesh*)) = new CMesh();

    luaL_setmetatable(L, L_MESH);
    return 1;
}

static INT mesh_addfgroup(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    CFaceGroup* fg = (CFaceGroup*)luaL_checkudata(L, 2, L_FACEGROUP);
    D3DMATRIX* mat = (D3DMATRIX*)luaL_checkudata(L, 3, L_MATRIX);
    luaN_ref(L, 1, 2);
    mesh->AddFaceGroup(fg, *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT mesh_getfgroups(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    lua_newtable(L);

    for (UINT i=0; i<mesh->GetNumFGroups(); i++)
    {
        CFaceGroup* fg = mesh->GetFGroups()[i];
        lua_pushinteger(L, i+1ULL);
        LUAP(L, L_FACEGROUP, CFaceGroup, fg);
        lua_settable(L, -3);
    }

    return 1;
}

static INT mesh_draw(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

    mesh->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT mesh_delete(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    luaN_unref(L, 1);

    mesh->Release();

    return 0;
}

static INT mesh_clear(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    mesh->Clear();

    return 0;
}

static INT mesh_setname(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    LPCSTR meshName = luaL_checkstring(L, 2);

    mesh->SetName(aiString(meshName));

    return 0;
}

static INT mesh_getname(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    lua_pushstring(L, mesh->GetName().C_Str());
    return 1;
}

static INT mesh_setmaterial(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    DWORD stage = (DWORD)luaL_checkinteger(L, 2);
    CMaterial* mat = NULL;

    if (lua_gettop(L) == 3)
    {
        mat = *(CMaterial**)luaL_checkudata(L, 3, L_MATERIAL);
        luaN_ref(L, 1, 3);
    }

    for (UINT i = 0; i < mesh->GetNumFGroups(); ++i)
    {
        mesh->GetFGroups()[i]->SetMaterial(stage, mat ? mat : NULL);
    }
    
    lua_pushvalue(L, 1);
    return 1;
}

static VOID LuaMesh_register(lua_State* L)
{
    lua_register(L, L_MESH, mesh_new);
    luaL_newmetatable(L, L_MESH);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    luaN_setid(L);

    REGC("addFGroup", mesh_addfgroup);
    REGC("addPart", mesh_addfgroup);
    REGC("draw", mesh_draw);
    REGC("getFGroups", mesh_getfgroups);
    REGC("getParts", mesh_getfgroups);
    REGC("clear", mesh_clear);
    REGC("setName", mesh_setname);
    REGC("setMaterial", mesh_setmaterial);
    REGC("getName", mesh_getname);
    REGC("__gc", mesh_delete);
    
    lua_pop(L, 1);
}
