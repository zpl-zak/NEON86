#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Model.h"

static INT model_new(lua_State* L)
{
    LPCSTR modelPath = NULL;
    BOOL loadMaterials = TRUE;

    if (lua_gettop(L) >= 1)
        modelPath = lua_tostring(L, 1);

    if (lua_gettop(L) == 2)
        loadMaterials = (UINT)lua_toboolean(L, 2);

    CModel* model = (CModel*)lua_newuserdata(L, sizeof(CModel));
    *model = CModel();

    if (modelPath)
        model->LoadModel(modelPath, loadMaterials);

    luaL_setmetatable(L, L_MODEL);
    return 1;
}

static INT model_getmeshes(lua_State* L)
{
    CModel* model = (CModel*)luaL_checkudata(L, 1, L_MODEL);

    lua_newtable(L);

    for (UINT i = 0; i < model->GetNumMeshes(); i++)
    {
        CMesh* mesh = model->GetMeshes()[i];
        lua_pushinteger(L, i + 1ULL);
        lua_pushlightuserdata(L, (VOID*)mesh);
        luaL_setmetatable(L, L_MESH);
        lua_settable(L, -3);
    }

    return 1;
}

static INT model_draw(lua_State* L)
{
    CModel* model = (CModel*)luaL_checkudata(L, 1, L_MODEL);
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

    model->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT model_loadmodel(lua_State* L)
{
    CModel* model = (CModel*)luaL_checkudata(L, 1, L_MODEL);
    LPSTR meshName = (LPSTR)luaL_checkstring(L, 2);
    BOOL loadMaterials = TRUE;

    if (lua_gettop(L) == 3)
        loadMaterials = (UINT)luaL_checkinteger(L, 3);

    model->LoadModel(meshName, loadMaterials);

    lua_pushvalue(L, 1);
    return 1;
}

static INT model_findmesh(lua_State* L)
{
    CModel* model = (CModel*)luaL_checkudata(L, 1, L_MODEL);
    LPSTR meshName = (LPSTR)luaL_checkstring(L, 2);
    
    CMesh* mg = model->FindMesh(meshName);

    if (mg)
    {
        lua_pushlightuserdata(L, (VOID*)mg);
        luaL_setmetatable(L, L_MESH);
    }
    else lua_pushnil(L);

    return 1;
}

static INT model_delete(lua_State* L)
{
    CModel* model = (CModel*)luaL_checkudata(L, 1, L_MODEL);

    model->Release();

    return 0;
}


static VOID LuaModel_register(lua_State* L)
{
    lua_register(L, L_MODEL, model_new);
    luaL_newmetatable(L, L_MODEL);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("draw", model_draw);
    REGC("loadModel", model_loadmodel);
    REGC("getMeshes", model_getmeshes);
    REGC("findMesh", model_findmesh);
    REGC("__gc", model_delete);

    lua_pop(L, 1);
}
