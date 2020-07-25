#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Mesh.h"

INT mesh_new(lua_State* L)
{
    *(CMesh**)lua_newuserdata(L, sizeof(CMesh*)) = new CMesh();

    luaL_setmetatable(L, L_MESH);
    return 1;
}

static INT mesh_clone(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    *(CMesh**)lua_newuserdata(L, sizeof(CMesh*)) = mesh->Clone();

    luaL_setmetatable(L, L_MESH);
    return 1;
}

static INT mesh_addfgroup(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    CFaceGroup* fg = *(CFaceGroup**)luaL_checkudata(L, 2, L_FACEGROUP);
    D3DMATRIX* mat = (D3DMATRIX*)luaL_checkudata(L, 3, L_MATRIX);

    mesh->AddFaceGroup(fg->Clone(), *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT mesh_getfgroups(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    lua_newtable(L);

    for (UINT i=0; i<mesh->GetNumFGroups(); i++)
    {
        CFaceGroup* fg = mesh->GetFGroupData()[i];
        lua_pushinteger(L, i+1ULL);
        LUAP(L, L_FACEGROUP, CFaceGroup, fg);
        lua_settable(L, -3);
    }

    return 1;
}

static INT mesh_draw(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    D3DXMATRIX* mat = &D3DXMATRIX();

    if (lua_gettop(L) >= 2)
    {
        mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
    }

    mesh->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT mesh_delete(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

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

    mesh->SetName(meshName);

    return 0;
}

static INT mesh_getname(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);

    lua_pushstring(L, mesh->GetName().Str());
    return 1;
}

static INT mesh_setmaterial(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    CMaterial* mat = NULL;

    if (lua_gettop(L) == 2)
    {
        mat = *(CMaterial**)luaL_checkudata(L, 2, L_MATERIAL);
    }

    for (UINT i = 0; i < mesh->GetNumFGroups(); ++i)
    {
        mesh->GetFGroupData()[i]->SetMaterial(mat ? mat : NULL);
        if (mat) mat->AddRef();
    }
    
    lua_pushvalue(L, 1);
    return 1;
}

static INT mesh_getmaterial(lua_State* L)
{
    CMesh* mesh = *(CMesh**)luaL_checkudata(L, 1, L_MESH);
    DWORD matid = (DWORD)luaL_checkinteger(L, 2) - 1;
    
    if (matid < 0 || matid >= mesh->GetNumFGroups())
        lua_pushnil(L);
    else 
    {
        CFaceGroup* fg = mesh->GetFGroupData()[matid];

        if (!fg->GetMaterial())
            lua_pushnil(L);
        else LUAP(L, L_MATERIAL, CMaterial, fg->GetMaterial());
    }

    return 1;
}

static VOID LuaMesh_register(lua_State* L)
{
    lua_register(L, L_MESH, mesh_new);
    luaL_newmetatable(L, L_MESH);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("addFGroup", mesh_addfgroup);
    REGC("addPart", mesh_addfgroup);
    REGC("draw", mesh_draw);
    REGC("clone", mesh_clone);
    REGC("getFGroups", mesh_getfgroups);
    REGC("getParts", mesh_getfgroups);
    REGC("clear", mesh_clear);
    REGC("setName", mesh_setname);
    REGC("setMaterial", mesh_setmaterial);
    REGC("getMaterial", mesh_getmaterial);
    REGC("getName", mesh_getname);
    REGC("__gc", mesh_delete);
    
    lua_pop(L, 1);
}
