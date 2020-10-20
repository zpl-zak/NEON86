#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "Light.h"
#include "Node.h"
#include "Scene.h"

auto node_new(lua_State* L) -> int
{
    *static_cast<CNode**>(lua_newuserdata(L, sizeof(CNode*))) = new CNode();
    luaL_setmetatable(L, L_NODE);
    return 1;
}

static auto node_clone(lua_State* L) -> int
{
    auto node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    *static_cast<CNode**>(lua_newuserdata(L, sizeof(CNode*))) = node->Clone();
    luaL_setmetatable(L, L_NODE);
    return 1;
}

static auto node_getname(lua_State* L) -> int
{
    auto node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    lua_pushstring(L, node->GetName().Str());

    return 1;
}

static auto node_setname(lua_State* L) -> int
{
    auto node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto name = (LPSTR)luaL_checkstring(L, 2);
    node->SetName(name);

    return 0;
}

static auto node_getmeshes(lua_State* L) -> int
{
    auto node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    lua_newtable(L);

    for (unsigned int i = 0; i < node->GetNumMeshes(); i++)
    {
        const auto mesh = node->GetMeshes()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_MESH, CMesh, mesh);
        lua_settable(L, -3);
    }

    return 1;
}

static auto node_getmeshparts(lua_State* L) -> int
{
    auto node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    lua_newtable(L);

    for (unsigned int i = 0; i < node->GetNumMeshes(); i++)
    {
        CMesh* mesh = node->GetMeshes()[i];
        lua_pushinteger(L, i + 1ULL);
        {
            lua_newtable(L);
            for (unsigned int i = 0; i < mesh->GetNumFGroups(); i++)
            {
                CFaceGroup* fg = mesh->GetFGroupData()[i];
                lua_pushinteger(L, i + 1ULL);
                LUAP(L, L_FACEGROUP, CFaceGroup, fg);
                lua_settable(L, -3);
            }
        }
        lua_settable(L, -3);
    }

    return 1;
}

static auto node_getlights(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    lua_newtable(L);

    for (unsigned int i = 0; i < node->GetNumLights(); i++)
    {
        CLight* lit = node->GetLightData()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_LIGHT, CLight, lit);
        lua_settable(L, -3);
    }

    return 1;
}

static auto node_getnodes(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    lua_newtable(L);

    for (unsigned int i = 0; i < node->GetNumNodes(); i++)
    {
        CNode* mg = node->GetNodeData()[i];
        lua_pushinteger(L, i + 1ULL);
        LUAP(L, L_NODE, CNode, mg);
        lua_settable(L, -3);
    }

    return 1;
}

static auto node_gettargets(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    lua_newtable(L);

    for (unsigned int i = 0; i < node->GetNumNodes(); i++)
    {
        CNode* tgt = node->GetNodeData()[i];

        if (!tgt->IsEmpty())
            continue;

        lua_pushstring(L, tgt->GetName().Str());
        matrix_new(L);
        *static_cast<D3DXMATRIX*>(lua_touserdata(L, 4)) = tgt->GetFinalTransform();
        lua_settable(L, -3);
    }

    return 1;
}

static auto node_findmesh(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto meshName = (LPSTR)luaL_checkstring(L, 2);

    CMesh* mg = node->FindMesh(meshName);

    if (mg)
    {
        LUAP(L, L_MESH, CMesh, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto node_findlight(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto lightName = (LPSTR)luaL_checkstring(L, 2);

    CLight* mg = node->FindLight(lightName);

    if (mg)
    {
        LUAP(L, L_LIGHT, CLight, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto node_findnode(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto childName = (LPSTR)luaL_checkstring(L, 2);

    CNode* mg = node->FindNode(childName);

    if (mg)
    {
        LUAP(L, L_NODE, CNode, mg);
    }
    else lua_pushnil(L);

    return 1;
}

static auto node_findtarget(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto targetName = (LPSTR)luaL_checkstring(L, 2);

    CNode* mg = node->FindNode(targetName);

    if (mg && mg->IsEmpty())
    {
        matrix_new(L);
        *static_cast<D3DXMATRIX*>(lua_touserdata(L, 3)) = mg->GetFinalTransform();
    }
    else lua_pushnil(L);

    return 1;
}

static auto node_gettransform(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    matrix_new(L);
    *static_cast<D3DXMATRIX*>(lua_touserdata(L, 2)) = node->GetTransform();

    return 1;
}

static auto node_settransform(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const D3DXMATRIX mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));

    node->SetTransform(mat);
    return 0;
}

static auto node_getfinaltransform(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    matrix_new(L);
    *static_cast<D3DXMATRIX*>(lua_touserdata(L, 2)) = node->GetFinalTransform();

    return 1;
}

static auto node_draw(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    D3DXMATRIX* mat = &D3DXMATRIX();
    D3DXMatrixIdentity(mat);

    if (lua_gettop(L) >= 2)
    {
        mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));
    }

    node->Draw(*mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto node_addnode(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    CNode* child = *static_cast<CNode**>(luaL_checkudata(L, 2, L_NODE));

    child->AddRef();
    child->SetParent(node);
    node->AddNode(child);

    lua_pushvalue(L, 1);
    return 1;
}

static auto node_addmesh(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    if (luaL_testudata(L, 2, L_MESH))
    {
        CMesh* mesh = *static_cast<CMesh**>(luaL_checkudata(L, 2, L_MESH));
        CMesh* clonedMesh = mesh->Clone();

        if (mesh->GetOwner())
        {
            for (auto& t : clonedMesh->GetTransforms())
            {
                t = mesh->GetOwner()->GetFinalTransform();
            }
        }

        node->AddMesh(clonedMesh);
    }

    if (luaL_testudata(L, 2, L_SCENE))
    {
        CScene* child = *static_cast<CScene**>(luaL_checkudata(L, 2, L_SCENE));

        for (auto& m : child->GetMeshes())
        {
            CMesh* clonedMesh = m->Clone();

            for (auto& t : clonedMesh->GetTransforms())
            {
                t = m->GetOwner()->GetFinalTransform();
            }

            node->AddMesh(clonedMesh);
        }
    }

    lua_pushvalue(L, 1);
    return 1;
}

static auto node_drawsubset(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const unsigned int subset = static_cast<unsigned int>(luaL_checkinteger(L, 2)) - 1;
    D3DXMATRIX* mat = &D3DXMATRIX();
    D3DXMatrixIdentity(mat);

    if (lua_gettop(L) >= 3)
    {
        mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 3, L_MATRIX));
    }

    node->DrawSubset(subset, *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto node_getmeta(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));
    const auto meta = static_cast<LPCSTR>(luaL_checkstring(L, 2));

    const METADATA_RESULT res = node->GetMetadata(meta);

    if (res.found)
        lua_pushstring(L, res.value.c_str());
    else lua_pushnil(L);

    return 1;
}

static auto node_delete(lua_State* L) -> int
{
    CNode* node = *static_cast<CNode**>(luaL_checkudata(L, 1, L_NODE));

    node->Release();

    return 0;
}


static void LuaNode$Register(lua_State* L)
{
    lua_register(L, L_NODE, node_new);
    luaL_newmetatable(L, L_NODE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("clone", node_clone);
    REGC("getName", node_getname);
    REGC("setName", node_setname);
    REGC("getTransform", node_gettransform);
    REGC("setTransform", node_settransform);
    REGC("getFinalTransform", node_getfinaltransform);
    REGC("addNode", node_addnode);
    REGC("addMesh", node_addmesh);
    REGC("draw", node_draw);
    REGC("drawSubset", node_drawsubset);
    REGC("getMeshes", node_getmeshes);
    REGC("getMeshParts", node_getmeshparts);
    REGC("getLights", node_getlights);
    REGC("getNodes", node_getnodes);
    REGC("getTargets", node_gettargets);
    REGC("findMesh", node_findmesh);
    REGC("findLight", node_findlight);
    REGC("findTarget", node_findtarget);
    REGC("findNode", node_findnode);
    REGC("getMeta", node_getmeta);
    REGC("__gc", node_delete);

    lua_pop(L, 1);
}
