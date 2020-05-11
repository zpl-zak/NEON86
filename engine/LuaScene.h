#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Scene.h"
#include "Light.h"
#include "Node.h"

static INT scene_new(lua_State* L)
{
    LPCSTR modelPath = NULL;
    BOOL loadMaterials = TRUE;

    if (lua_gettop(L) >= 1)
        modelPath = lua_tostring(L, 1);

    if (lua_gettop(L) == 2)
        loadMaterials = (UINT)lua_toboolean(L, 2);

    CScene* model = (CScene*)lua_newuserdata(L, sizeof(CScene));
    *model = CScene();

    if (modelPath)
        model->LoadScene(modelPath, loadMaterials);

    luaL_setmetatable(L, L_SCENE);
    return 1;
}

static INT scene_getmeshes(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);

    lua_newtable(L);

    for (UINT i = 0; i < model->GetNumMeshes(); i++)
    {
        CMesh* mesh = model->GetMeshes()[i];
        lua_pushinteger(L, i + 1ULL);
        lua_pushlightuserdata(L, (LPVOID)mesh);
        luaL_setmetatable(L, L_MESH);
        lua_settable(L, -3);
    }

    return 1;
}

static INT scene_getlights(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);

    lua_newtable(L);

    for (UINT i = 0; i < model->GetNumLights(); i++)
    {
        CLight* lit = model->GetLights()[i];
        lua_pushinteger(L, i + 1ULL);
        lua_pushlightuserdata(L, (LPVOID)lit);
        luaL_setmetatable(L, L_LIGHT);
        lua_settable(L, -3);
    }

    return 1;
}

static INT scene_gettargets(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);

    lua_newtable(L);

    for (UINT i = 0; i < model->GetNumNodes(); i++)
    {
        CNode* tgt = model->GetNodes()[i];
        lua_pushstring(L, tgt->GetName().C_Str());
        matrix_new(L);
        *(D3DXMATRIX*)lua_touserdata(L, 4) = tgt->GetTransform();
        lua_settable(L, -3);
    }

    return 1;
}

static INT scene_draw(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

    model->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT scene_drawsubset(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    UINT subset = (UINT)luaL_checkinteger(L, 2) - 1;
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 3, L_MATRIX);

    model->DrawSubset(subset, *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static INT scene_loadmodel(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    LPSTR meshName = (LPSTR)luaL_checkstring(L, 2);
    BOOL loadMaterials = TRUE;

    if (lua_gettop(L) == 3)
        loadMaterials = (UINT)luaL_checkinteger(L, 3);

    model->LoadScene(meshName, loadMaterials);

    lua_pushvalue(L, 1);
    return 1;
}

static INT scene_findmesh(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    LPSTR meshName = (LPSTR)luaL_checkstring(L, 2);
    
    CMesh* mg = model->FindMesh(meshName);

    if (mg)
    {
        lua_pushlightuserdata(L, (LPVOID)mg);
        luaL_setmetatable(L, L_MESH);
    }
    else lua_pushnil(L);

    return 1;
}

static INT scene_findlight(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    LPSTR lightName = (LPSTR)luaL_checkstring(L, 2);

    CLight* mg = model->FindLight(lightName);

    if (mg)
    {
        lua_pushlightuserdata(L, (LPVOID)mg);
        luaL_setmetatable(L, L_LIGHT);
    }
    else lua_pushnil(L);

    return 1;
}

static INT scene_findtarget(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);
    LPSTR targetName = (LPSTR)luaL_checkstring(L, 2);

    CNode* mg = model->FindNode(targetName);

    if (mg) {
        matrix_new(L);
        *(D3DXMATRIX*)lua_touserdata(L, 3) = mg->GetTransform();
    }
    else    lua_pushnil(L);

    return 1;
}

static INT scene_delete(lua_State* L)
{
    CScene* model = (CScene*)luaL_checkudata(L, 1, L_SCENE);

    model->Release();

    return 0;
}


static VOID LuaScene_register(lua_State* L)
{
    lua_register(L, L_SCENE, scene_new);
    lua_register(L, "Model", scene_new);
    luaL_newmetatable(L, L_SCENE);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    luaN_setid(L);

    REGC("draw", scene_draw);
    REGC("drawSubset", scene_drawsubset);
    REGC("loadModel", scene_loadmodel);
    REGC("loadScene", scene_loadmodel);
    REGC("getMeshes", scene_getmeshes);
    REGC("getLights", scene_getlights);
    REGC("getTargets", scene_gettargets);
    REGC("findMesh", scene_findmesh);
    REGC("findLight", scene_findlight);
    REGC("findTarget", scene_findtarget);
    REGC("__gc", scene_delete);

    lua_pop(L, 1);
}
