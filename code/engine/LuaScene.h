#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Scene.h"
#include "Light.h"
#include "Node.h"

auto scene_new(lua_State* L) -> INT
{
    LPCSTR modelPath = nullptr;
    auto loadMaterials = TRUE;
    auto optimizeMesh = FALSE;

    if (lua_gettop(L) >= 1)
        modelPath = lua_tostring(L, 1);

    if (lua_gettop(L) >= 2)
        loadMaterials = static_cast<UINT>(lua_toboolean(L, 2));

    if (lua_gettop(L) >= 3)
        optimizeMesh = static_cast<UINT>(lua_toboolean(L, 3));

    const auto scene = static_cast<CScene**>(lua_newuserdata(L, sizeof(CScene*)));
    *scene = new CScene();

    if (modelPath)
        if (!(*scene)->LoadScene(modelPath, loadMaterials, optimizeMesh))
            return 0;

    luaL_setmetatable(L, L_SCENE);
    return 1;
}

static auto scene_getmeshes(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    lua_newtable(L);

    for (UINT i = 0; i < scene->GetNumMeshes(); i++)
    {
        const auto mesh = scene->GetMeshes()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_MESH, CMesh, mesh);
        lua_settable(L, -3);
    }

    return 1;
}

static auto scene_getlights(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    lua_newtable(L);

    for (UINT i = 0; i < scene->GetNumLights(); i++)
    {
        const auto lit = scene->GetLightData()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_LIGHT, CLight, lit);
        lua_settable(L, -3);
    }

    return 1;
}

static auto scene_gettargets(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    lua_newtable(L);

    for (UINT i = 0; i < scene->GetNumNodes(); i++)
    {
        auto tgt = scene->GetNodeData()[i];

        if (!tgt->IsEmpty())
            continue;

        lua_pushstring(L, tgt->GetName().Str());
        matrix_new(L);
        *static_cast<D3DXMATRIX*>(lua_touserdata(L, 4)) = tgt->GetFinalTransform();
        lua_settable(L, -3);
    }

    return 1;
}

static auto scene_getflattennodes(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    lua_newtable(L);

    for (UINT i = 0; i < scene->GetNumNodes(); i++)
    {
        const auto tgt = scene->GetNodeData()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_NODE, CNode, tgt);
        lua_settable(L, -3);
    }

    return 1;
}

static auto scene_draw(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    auto mat = &D3DXMATRIX();
    D3DXMatrixIdentity(mat);

    if (lua_gettop(L) >= 2)
    {
        mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));
    }

    scene->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto scene_drawsubset(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    const auto subset = static_cast<UINT>(luaL_checkinteger(L, 2)) - 1;
    auto mat = &D3DXMATRIX();
    D3DXMatrixIdentity(mat);

    if (lua_gettop(L) >= 3)
    {
        mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 3, L_MATRIX));
    }

    scene->DrawSubset(subset, *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto scene_loadmodel(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    const auto meshName = (LPSTR)luaL_checkstring(L, 2);
    auto loadMaterials = TRUE;
    auto optimizeMesh = FALSE;

    if (lua_gettop(L) >= 3)
        loadMaterials = static_cast<UINT>(luaL_checkinteger(L, 3));

    if (lua_gettop(L) >= 4)
        optimizeMesh = static_cast<UINT>(lua_toboolean(L, 4));

    scene->LoadScene(meshName, loadMaterials, optimizeMesh);

    lua_pushvalue(L, 1);
    return 1;
}

static auto scene_findmesh(lua_State* L) -> INT
{
    auto scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    const auto meshName = (LPSTR)luaL_checkstring(L, 2);

    CMesh* mg = scene->FindMesh(meshName);

    if (mg)
    {
        LUAP(L, L_MESH, CMesh, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto scene_findlight(lua_State* L) -> INT
{
    CScene* scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    const auto lightName = (LPSTR)luaL_checkstring(L, 2);

    CLight* mg = scene->FindLight(lightName);

    if (mg)
    {
        LUAP(L, L_LIGHT, CLight, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto scene_findtarget(lua_State* L) -> INT
{
    CScene* scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));
    const auto targetName = (LPSTR)luaL_checkstring(L, 2);

    CNode* mg = scene->FindNode(targetName);

    if (mg && mg->IsEmpty())
    {
        matrix_new(L);
        *static_cast<D3DXMATRIX*>(lua_touserdata(L, 3)) = mg->GetFinalTransform();
    }
    else lua_pushnil(L);

    return 1;
}

static auto scene_getrootnode(lua_State* L) -> INT
{
    CScene* scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    CNode* mg = scene->GetRootNode();

    if (mg)
    {
        LUAP(L, L_NODE, CNode, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto scene_delete(lua_State* L) -> INT
{
    CScene* scene = *static_cast<CScene**>(luaL_checkudata(L, 1, L_SCENE));

    scene->Release();

    return 0;
}


static VOID LuaScene_register(lua_State* L)
{
    lua_register(L, L_SCENE, scene_new);
    lua_register(L, "Model", scene_new);
    luaL_newmetatable(L, L_SCENE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("draw", scene_draw);
    REGC("drawSubset", scene_drawsubset);
    REGC("loadModel", scene_loadmodel);
    REGC("loadScene", scene_loadmodel);
    REGC("getMeshes", scene_getmeshes);
    REGC("getLights", scene_getlights);
    REGC("getFlattenNodes", scene_getflattennodes);
    REGC("getTargets", scene_gettargets);
    REGC("findMesh", scene_findmesh);
    REGC("findLight", scene_findlight);
    REGC("findTarget", scene_findtarget);
    REGC("getRootNode", scene_getrootnode);
    REGC("__gc", scene_delete);

    lua_pop(L, 1);
}
